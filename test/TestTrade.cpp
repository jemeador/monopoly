#include "Test.h"
#include "DisplayStrings.h"
using namespace monopoly;

#include "catch2/catch.hpp"

SCENARIO("Players can trade cash, deeds, and GOJF cards ", "[trade]") {
    Test test;
    int const startingFunds = 1500;

    GIVEN("Player 1 has 2 orange properties and a blue and player 2 has 1 blue property and 1 orange property") {
        test.give_deeds(Player::p1, {Property::Orange_1, Property::Orange_3, Property::Blue_1});
        test.give_deeds(Player::p2, {Property::Orange_2, Property::Blue_2});
        test.give_get_out_of_jail_free_card(Player::p2, DeckType::Chance);
        test.set_player_funds(Player::p1, startingFunds);
        test.set_player_funds(Player::p2, startingFunds);

        WHEN("player 1 proposes a trade to player 2") {
            Trade trade;
            trade.offeringPlayer = Player::p1;
            trade.offer = { 0, {Property::Blue_1}, {} };
            trade.consideringPlayer = Player::p2;
            trade.consideration = { 100, {Property::Orange_2}, {} };
            test.propose_trade(trade);

            THEN("player 2 is given the option to accept the trade") {
                test.require_phase(TurnPhase::WaitingForTradeOfferResponse);
            }
            AND_WHEN("player 2 accepts the trade") {
                test.propose_trade(reciprocal_trade(trade));

                THEN("the trade is executed") {
                    test.require_does_not_have_deed(Player::p1, Property::Blue_1);
                    test.require_does_not_have_deed(Player::p2, Property::Orange_2);
                    test.require_has_deed(Player::p1, Property::Orange_2);
                    test.require_has_deed(Player::p2, Property::Blue_1);
                    test.require_funds(Player::p1, startingFunds + 100);
                    test.require_funds(Player::p2, startingFunds - 100);
                }
            }
            AND_WHEN("player 2 proposes a counter offer") {
                auto newTrade = reciprocal_trade(trade);
                newTrade.offer.cash = 1;
                test.propose_trade(newTrade);

                THEN("player 1 is given the option to accept the trade") {
                    test.require_phase(TurnPhase::WaitingForTradeOfferResponse);
                }

                AND_WHEN("player 1 accepts") {
                    test.propose_trade(reciprocal_trade(newTrade));

                    THEN("the trade is executed") {
                        test.require_does_not_have_deed(Player::p1, Property::Blue_1);
                        test.require_does_not_have_deed(Player::p2, Property::Orange_2);
                        test.require_has_deed(Player::p1, Property::Orange_2);
                        test.require_has_deed(Player::p2, Property::Blue_1);
                        test.require_funds(Player::p1, startingFunds + 1);
                        test.require_funds(Player::p2, startingFunds - 1);
                    }
                }
            }
            AND_WHEN("player 2 proposes a counter offer with a get out of jail free card") {
                auto newTrade = reciprocal_trade(trade);
                newTrade.offer.cash = 1;
                newTrade.offer.getOutOfJailFreeCards = {DeckType::Chance};
                test.propose_trade(newTrade);

                THEN("player 1 is given the option to accept the trade") {
                    test.require_phase(TurnPhase::WaitingForTradeOfferResponse);
                }

                AND_WHEN("player 1 accepts") {
                    test.propose_trade(reciprocal_trade(newTrade));

                    THEN("player 1 gets the get out of jail free card") {
                        test.require_has_get_out_of_jail_free(Player::p1, DeckType::Chance);
                    }
                }
                AND_WHEN("player 1 declines") {
                    test.decline_trade(Player::p1);

                    THEN("play resumes") {
                        test.require_phase(TurnPhase::WaitingForRoll);
                    }
                }
            }
        }
    }
}

SCENARIO("Trades with promises that can't be fulfilled, trades with empty promises, or trades would bankrupt a player due to closing costs are invalid", "[trade, bankruptcy]") {
    Test test;

    GIVEN("Player 1 has a mortgaged property and player 1 and player 2 are broke") {
        test.give_deed(Player::p1, Property::Blue_2);
        test.mortgage_property(Player::p1, Property::Blue_2);
        test.set_player_funds(Player::p1, 0);
        test.set_player_funds(Player::p2, 0);

        WHEN("player 1 proposes a tricky trade to player 2") {
            Trade trade;
            trade.offeringPlayer = Player::p1;
            trade.offer = { 0, {Property::Blue_1}, {} };
            trade.consideringPlayer = Player::p2;
            trade.consideration = { 0, {}, {} };
            test.propose_trade(trade);

            THEN("nothing happens because player 2 can't afford the closing costs") {
                test.require_phase(TurnPhase::WaitingForRoll);
            }
        }
    }
    GIVEN("Player 2 has a property") {
        test.give_deed(Player::p2, Property::Blue_2);
        test.set_player_funds(Player::p1, 1000);

        WHEN("player 1 promises more cash than they have") {
            Trade trade;
            trade.offeringPlayer = Player::p1;
            trade.offer = { 1001, {}, {} };
            trade.consideringPlayer = Player::p2;
            trade.consideration = { 0, {Property::Blue_2}, {} };
            test.propose_trade(trade);

            THEN("nothing happens") {
                test.require_phase(TurnPhase::WaitingForRoll);
            }
        }

        WHEN("player 1 asks for something for nothing") {
            Trade trade;
            trade.offeringPlayer = Player::p1;
            trade.offer = { 0, {}, {} };
            trade.consideringPlayer = Player::p2;
            trade.consideration = { 0, {Property::Blue_2}, {} };
            test.propose_trade(trade);

            THEN("nothing happens") {
                test.require_phase(TurnPhase::WaitingForRoll);
            }
        }

        WHEN("player 1 tries to offer a GOJF card they don't have") {
            Trade trade;
            trade.offeringPlayer = Player::p1;
            trade.offer = { 0, {}, {DeckType::CommunityChest} };
            trade.consideringPlayer = Player::p2;
            trade.consideration = { 0, {Property::Blue_2}, {} };
            test.propose_trade(trade);

            THEN("nothing happens") {
                test.require_phase(TurnPhase::WaitingForRoll);
            }
        }

        WHEN("player 1 tries to offer a deed they don't have") {
            Trade trade;
            trade.offeringPlayer = Player::p1;
            trade.offer = { 0, {Property::Orange_2}, {} };
            trade.consideringPlayer = Player::p2;
            trade.consideration = { 0, {Property::Blue_2}, {} };
            test.propose_trade(trade);

            THEN("nothing happens") {
                test.require_phase(TurnPhase::WaitingForRoll);
            }
        }

        AND_GIVEN("player 2 has a property with a house") {
            test.give_deed(Player::p2, Property::Blue_1);
            test.set_buildings({ {Property::Blue_1, 1} });

            WHEN("player 1 tries to request for consideration a deed in a property group with buidings") {
                Trade trade;
                trade.offeringPlayer = Player::p1;
                trade.offer = { 100, {}, {} };
                trade.consideringPlayer = Player::p2;
                trade.consideration = { 0, {Property::Blue_2}, {} };
                test.propose_trade(trade);

                THEN("nothing happens") {
                    test.require_phase(TurnPhase::WaitingForRoll);
                }
            }
        }
    }
}
