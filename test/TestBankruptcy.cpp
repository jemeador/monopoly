#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

SCENARIO("When a player can't afford to pay a debt, they must acquire funds or be eliminated", "[bankruptcy]") {
    Test test;

    GIVEN("Player 1 has $0 and player 2 owns Boardwalk") {
        test.set_player_funds(Player::p1, 1);
        test.set_player_funds(Player::p2, 1500);
        test.set_player_funds(Player::p3, 100);
        test.give_deed(Player::p1, Property::Brown_1);
        test.give_get_out_of_jail_free_card(Player::p1, DeckType::Chance);
        test.give_deed(Player::p2, Property::Blue_1);
        test.give_deed(Player::p2, Property::Blue_2);

        WHEN("player 1 lands on Boardwalk") {
            test.land_on_space(Player::p1, Space::Blue_2);

            THEN("player 1 must settle debts") {
                test.require_phase(TurnPhase::WaitingForDebtSettlement);
            }

            AND_WHEN("player 1 trades a property for the required funds") {
                Trade trade;
                trade.offeringPlayer = Player::p1;
                trade.offer = { 0, {Property::Brown_1}, {} };
                trade.consideringPlayer = Player::p3;
                trade.consideration = { 100, {}, {} };
                test.propose_trade(trade);
                test.propose_trade(reciprocal_trade(trade));

                THEN("The debt is settled") {
                    test.require_not_phase(TurnPhase::WaitingForDebtSettlement);
                }
            }

            AND_WHEN("player 1 resigns") {
				test.resign(Player::p1);

                THEN("player 1 is bankrupt! player 2 gets everything he owns and the game moves on") {
                    test.require_eliminated(Player::p1, true);
                    test.require_has_deed(Player::p2, Property::Brown_1);
                    test.require_has_get_out_of_jail_free(Player::p2, DeckType::Chance);
                    test.require_funds(Player::p2, 1501);
                    test.require_active_player(Player::p2);
                }
            }
        }
    }
    GIVEN("Player 1 has $0") {
        test.set_player_funds(Player::p1, 0);
		test.give_deed(Player::p1, Property::Brown_1);

        WHEN("player 1 lands on Luxury Tax") {
            test.land_on_space(Player::p1, Space::LuxuryTax);

            THEN("player 1 must settle debts") {
                test.require_phase(TurnPhase::WaitingForDebtSettlement);
            }
        }
		AND_WHEN("player 1 resigns") {
			test.resign(Player::p1);

			THEN("player 1 is bankrupt! his deeds are auctioned by the bank") {
				test.require_eliminated(Player::p1, true);
				test.require_phase(TurnPhase::WaitingForBids);
			}
		}
    }
}
