#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

#include <cassert>

SCENARIO("A player draws a card that advances them to a specific location", "[cards]") {
    Test test;

    int const startingFunds = 1500;
    test.set_player_funds(Player::p1, startingFunds);

    GIVEN("the Community Chest deck is stacked with Advance to GO") {
        test.stack_deck(DeckType::CommunityChest, { Card::CommunityChest_AdvanceToGo });

        WHEN("player 1 lands on Community Chest") {
            test.land_on_space(Player::p1, Space::CommunityChest_1);

            THEN("player 1 draws a Community Chest card (Advance to GO), advances to GO, and collects GO salary") {
                test.require_position(Player::p1, Space::Go);
                test.require_funds(Player::p1, startingFunds + GoSalary);
            }
        }
    }
}

SCENARIO("A player draws the Go to Jail card", "[cards]") {
    Test test;

    int const startingFunds = 1500;
    test.set_player_funds(Player::p1, startingFunds);

    GIVEN("the Chance deck is stacked with Go To Jail") {
        test.stack_deck(DeckType::Chance, { Card::Chance_GoToJail });

        WHEN("player 1 rolls (2,2) and lands on Chance") {
            test.land_on_space(Player::p1, Space::Chance_1);

            THEN("player 1 goes directly to jail, does not pass go, does not collect $200") {
                test.require_position(Player::p1, Space::Jail);
                test.require_jailed(Player::p1, true);
                test.require_funds(Player::p1, startingFunds);
            }
        }
    }
}

SCENARIO("A player draws Advance to the Nearest Railroad", "[cards]") {
    Test test;

    int const startingFunds = 1500;
    test.set_player_funds(Player::p1, startingFunds);
    test.set_player_funds(Player::p2, startingFunds);

    GIVEN("The Chance deck is stacked with Advance to Nearest Railroad") {
        test.stack_deck(DeckType::Chance, { Card::Chance_AdvanceToNearestRailroad });

        WHEN("player 1 lands on Chance 1") {
            test.land_on_space(Player::p1, Space::Chance_1);

            THEN("player 1 advances to Pennsylvania Railroad") {
                test.require_position(Player::p1, Space::Railroad_2);
                test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
            }
        }
        WHEN("player 1 lands on Chance 2") {
            test.land_on_space(Player::p1, Space::Chance_2);

            THEN("player 1 advances to B. & O. Railroad") {
                test.require_position(Player::p1, Space::Railroad_3);
                test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
            }
        }
        WHEN("player 1 lands on Chance 3") {
            test.land_on_space(Player::p1, Space::Chance_3);

            THEN("player 1 advances to Reading Railroad") {
                test.require_position(Player::p1, Space::Railroad_1);
                test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
            }
        }
        WHEN("player 2 owns B. & O. Railroad") {
            test.give_deed(Player::p2, Property::Railroad_3);
            AND_WHEN("player 1 lands on Chance 3") {
                test.land_on_space(Player::p1, Space::Chance_2);

                THEN("player 1 advances to B. & 0. Railroad and pays double rent") {
                    auto const modifiedRent = 2 * rent_price_of_railroad(1);
                    test.require_position(Player::p1, Space::Railroad_3);
                    test.require_funds(Player::p1, startingFunds - modifiedRent);
                    test.require_funds(Player::p2, startingFunds + modifiedRent);
                }
            }
        }
    }
}

SCENARIO("A player draws Advance to the Nearest Utility", "[cards]") {
    Test test;

    int const startingFunds = 1500;
    test.set_player_funds(Player::p1, startingFunds);
    test.set_player_funds(Player::p2, startingFunds);

    GIVEN("The Chance deck is stacked with Advance to Nearest Railroad") {
        test.stack_deck(DeckType::Chance, { Card::Chance_AdvanceToNearestUtility });

        WHEN("player 1 lands on Chance 1") {
            test.land_on_space(Player::p1, Space::Chance_1);

            THEN("player 1 advances to Electric Company") {
                test.require_position(Player::p1, Space::Utility_1);
                test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
            }
        }
        WHEN("player 1 lands on Chance 2") {
            test.land_on_space(Player::p1, Space::Chance_2);

            THEN("player 1 advances to Water Works") {
                test.require_position(Player::p1, Space::Utility_2);
                test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
            }
        }
        WHEN("player 1 lands on Chance 3") {
            test.land_on_space(Player::p1, Space::Chance_3);

            THEN("player 1 advances to Electric Company") {
                test.require_position(Player::p1, Space::Utility_1);
                test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
            }
        }
        WHEN("player 2 owns Water Works") {
            test.give_deed(Player::p2, Property::Utility_2);
            AND_WHEN("player 1 lands on Chance 3") {
                test.land_on_space(Player::p1, Space::Chance_2);

                THEN("player 1 advances to Water Works and pays double the value of a random roll") {
                    auto const diceRoll = test.game.get_state().get_last_dice_roll();
                    auto const sum = diceRoll.first + diceRoll.second;
                    auto const rent = sum * 10;
                    test.require_position(Player::p1, Space::Utility_2);
                    test.require_funds(Player::p1, startingFunds - rent);
                    test.require_funds(Player::p2, startingFunds + rent);
                }
            }
        }
    }
}

SCENARIO("A player draws Go Back 3 Spaces", "[cards]") {
    Test test;

    GIVEN("The Chance deck is stacked with Go Back 3 Spaces") {
        test.stack_deck(DeckType::Chance, { Card::Chance_GoBack3Spaces });
        test.stack_deck(DeckType::CommunityChest, { Card::CommunityChest_Gain10 });

        WHEN("player 1 lands on Chance 1") {
            test.land_on_space(Player::p1, Space::Chance_1);

            THEN("player 1 goes back 3 spaces to Income Tax") {
                test.require_position(Player::p1, Space::IncomeTax);
            }
        }
        WHEN("player 1 lands on Chance 2") {
            test.land_on_space(Player::p1, Space::Chance_2);

            THEN("player 1 goes back 3 spaces New York Avenue") {
                test.require_position(Player::p1, Space::Orange_3);
            }
        }
        WHEN("player 1 lands on Chance 3") {
            test.land_on_space(Player::p1, Space::Chance_3);

            THEN("player 1 goes back 3 spaces to Community Chest") {
                test.require_position(Player::p1, Space::CommunityChest_3);
            }
        }
    }
}

SCENARIO("A player draws Get Out of Jail Free", "[cards]") {
    Test test;

    GIVEN("The Chance deck is stacked with Get Out of Jail Free") {
        test.stack_deck(DeckType::Chance, { Card::Chance_GetOutOfJailFree });

        WHEN("player 1 lands on Chance 1") {
            test.land_on_space(Player::p1, Space::Chance_1);

            THEN("player 1 has a Chance GOJF card") {
                test.require_has_get_out_of_jail_free(Player::p1, DeckType::Chance);
                test.require_does_not_have_get_out_of_jail_free(Player::p1, DeckType::CommunityChest);
            }
        }
    }
    GIVEN("The Community Chest deck is stacked with Get Out of Jail Free") {
        test.stack_deck(DeckType::CommunityChest, { Card::CommunityChest_GetOutOfJailFree });

        WHEN("player 1 lands on Community Chest 1") {
            test.land_on_space(Player::p1, Space::CommunityChest_1);

            THEN("player 1 has a Community Chest GOJF card") {
                test.require_has_get_out_of_jail_free(Player::p1, DeckType::CommunityChest);
                test.require_does_not_have_get_out_of_jail_free(Player::p1, DeckType::Chance);
            }
        }
    }
}

SCENARIO("Players land on Community Chest throughout the game, a cyclic pattern is observed as cards are simply placed on the bottom after drawn", "[cards]") {
    Test test;

    WHEN("Any player lands on Community Chest enough times to cycle the deck, eventually drawing a GOJF card") {
        for (auto i = 0; i < CommunityChestCards.size(); ++i)
            test.land_on_space(Player::p1, Space::CommunityChest_1);
        THEN("player 1 has a Community Chest GOJF card") {
            test.require_has_get_out_of_jail_free(Player::p1, DeckType::CommunityChest);
        }

        AND_WHEN("a player returns the GOJF card") {
            test.jail_player(Player::p1);
            test.set_active_player(Player::p1);
            test.use_get_out_of_jail_free_card();

            THEN("player 1 doesn't have a Community Chest GOJF card") {
                test.require_does_not_have_get_out_of_jail_free(Player::p1, DeckType::CommunityChest);
            }

            AND_WHEN("any player lands on Community Chest enough times to cycle the deck with one more landing") {
                for (auto i = 0; i < CommunityChestCards.size() - 1; ++i)
                    test.land_on_space(Player::p1, Space::CommunityChest_1);

                THEN("player 1 doesn't have a Community Chest GOJF card") {
                    test.require_does_not_have_get_out_of_jail_free(Player::p1, DeckType::CommunityChest);
                }

                AND_WHEN("a player lands on community chest one more time") {
                    test.land_on_space(Player::p1, Space::CommunityChest_1);

                    THEN("player 1 has a Community Chest GOJF card") {
                        test.require_has_get_out_of_jail_free(Player::p1, DeckType::CommunityChest);
                    }
                }
            }
        }
    }
}

SCENARIO("A player draws one of the building repair cards", "[cards]") {
    Test test;
    auto const startingFunds = 1500;
    test.set_player_funds(Player::p1, startingFunds);

    GIVEN("The decks are stacked with General Repairs") {
        test.stack_deck(DeckType::Chance, { Card::Chance_Repairs });
        test.stack_deck(DeckType::CommunityChest, { Card::CommunityChest_Repairs });

        WHEN("player 1 lands on Chance 1") {
            test.land_on_space(Player::p1, Space::Chance_1);

            THEN("player 1 doesn't lose money if they have no buildings") {
                test.require_funds(Player::p1, startingFunds);
            }
        }

        AND_GIVEN("player 1 has 4 houses and 2 hotels and player 2 has 4 houses and 1 hotel") {
            test.give_deeds(Player::p1, { Property::Green_1, Property::Green_2, Property::Green_3 });
            test.give_deeds(Player::p2, { Property::Blue_1, Property::Blue_2 });
            test.set_buildings({
                { Property::Green_1, 5 },
                { Property::Green_2, 4 },
                { Property::Green_3, 5 },
                { Property::Blue_1, 4 },
                { Property::Blue_2, 5 },
                });

            WHEN("player 1 lands on Chance 1") {
                test.land_on_space(Player::p1, Space::Chance_1);

                THEN("player 1 loses 45 per house and 115 per hotel") {
                    int const repairCost = 25 * 4 + 100 * 2;
                    test.require_funds(Player::p1, startingFunds - repairCost);
                }
            }
            WHEN("player 2 lands on Community Community Chest 1") {
                test.land_on_space(Player::p2, Space::CommunityChest_1);

                THEN("player 2 loses 40 per house and 115 per hotel") {
                    int const repairCost = 40 * 4 + 115 * 1;
                    test.require_funds(Player::p2, startingFunds - repairCost);
                }
            }
        }
    }
}

