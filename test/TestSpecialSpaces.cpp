#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

#include <cassert>

SCENARIO("When a player lands on Income Tax, they pay the lesser of $200 or 10% of their net worth", "[special]") {
    Test test;

    GIVEN("Player 1's funds are less than $2000") {
        int const startingFunds = 1990;
        test.set_player_funds(Player::p1, startingFunds);

        WHEN("player 1 lands on Income Tax") {
            test.land_on_space(Player::p1, Space::IncomeTax);

            THEN("player 1 loses 10 % of their funds") {
                test.require_funds(Player::p1, startingFunds - 199);
            }
        }
    }

    GIVEN("Player 1's funds are more than $2000") {
        int const startingFunds = 2010;
        test.set_player_funds(Player::p1, startingFunds);

        WHEN("player 1 lands on Income Tax") {
            test.land_on_space(Player::p1, Space::IncomeTax);

            THEN("player 1 loses $200 of their funds") {
                test.require_funds(Player::p1, startingFunds - 200);
            }
        }
    }

    GIVEN("Player 1's has 5 blue buildings, 2 mortgaged railroads") {
        test.give_deeds(Player::p1, { Property::Blue_1, Property::Blue_2, Property::Railroad_1, Property::Railroad_2 });
        test.set_buildings({
            { Property::Blue_1, 2 },
            { Property::Blue_2, 3 },
            });
        test.mortgage_property(Player::p1, Property::Railroad_1);
        test.mortgage_property(Player::p1, Property::Railroad_2);

        AND_GIVEN("player 1 has $21") {
            int const startingFunds = 21;
            test.set_player_funds(Player::p1, startingFunds);

            WHEN("player 1 lands on Income Tax") {
                test.land_on_space(Player::p1, Space::IncomeTax);

                THEN("player 1 loses %10 of their net worth") {
                    // Blue 1 @ 350
                    // Blue 2 @ 400
                    // 4th row buildings @ $200 x 5
                    // Mortgaged railroads @ 100 x 2
                    // Cash on hand 21
                    //
                    // Total = 1971
                    // Tax = 197.1, rounded up to 198
                    test.require_funds(Player::p1, startingFunds - 198);
                }
            }
        }
        AND_GIVEN("player 1 has $200") {
            int const startingFunds = 200;
            test.set_player_funds(Player::p1, startingFunds);

            WHEN("player 1 lands on Income Tax") {
                test.land_on_space(Player::p1, Space::IncomeTax);

                THEN("player 1 loses $200") {
                    // Blue 1 @ 350
                    // Blue 2 @ 400
                    // 4th row buildings @ $200 x 5
                    // Mortgaged railroads @ 100 x 2
                    // Cash on hand 200
                    //
                    // Total = 2150
                    // Tax = 215, capped at 200
                    test.require_funds(Player::p1, startingFunds - 200);
                }
            }
        }
    }
}

SCENARIO("When a player lands on luxury tax they lose $100", "[special]") {
    Test test;

    GIVEN("Player 1's has $1500") {
        int const startingFunds = 1500;
        test.set_player_funds(Player::p1, startingFunds);

        WHEN("player 1 lands on Luxury Tax") {
            test.land_on_space(Player::p1, Space::LuxuryTax);

            THEN("player 1 loses $100 of their funds") {
                test.require_funds(Player::p1, startingFunds - 100);
            }
        }
    }
}

SCENARIO("When a player lands on the \"Go to Jail\" space, they go directly to jail!", "[special]") {
    Test test;

    GIVEN("Player 1 has $1500 and isn't in jail") {
        int const startingFunds = 1500;
        test.set_player_funds(Player::p1, startingFunds);

        WHEN("player 1 lands on Go to Jail") {
            test.land_on_space(Player::p1, Space::GoToJail);

            THEN("player 1 goes directly to jail, does not pass go, does not collect $200") {
                test.require_position(Player::p1, Space::Jail);
                test.require_jailed(Player::p1, true);
                test.require_funds(Player::p1, startingFunds);
                test.require_phase(TurnPhase::WaitingForTurnEnd);
            }
        }
        WHEN("player 1 rolls doubles and lands on Go to Jail") {
            test.move_player(Player::p1, Space::Utility_2);
            test.roll(Player::p1, 1, 1);

            THEN("player 1 goes directly to jail, their turn is over despite rolling doubles") {
                test.require_position(Player::p1, Space::Jail);
                test.require_jailed(Player::p1, true);
                test.require_funds(Player::p1, startingFunds);
                test.require_phase(TurnPhase::WaitingForTurnEnd);
            }
        }
    }
}

SCENARIO("When a player lands on the \"Free Parking\" space, nothing happens", "[special]") {
    Test test;

    GIVEN("Player 1 has $1500") {
        int const startingFunds = 1500;
        test.set_player_funds(Player::p1, startingFunds);

        WHEN("player 1 lands on Free Parking") {
            test.land_on_space(Player::p1, Space::FreeParking);

            THEN("player 1 gains no money and does not move") {
                test.require_position(Player::p1, Space::FreeParking);
                test.require_funds(Player::p1, startingFunds);
            }
        }
    }
}
