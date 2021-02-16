#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

SCENARIO("When a player can't afford to pay a debt, they are eliminated", "[bankruptcy]") {
    Test test;

    GIVEN("Player 1 has $0 and player 2 owns Boardwalk") {
        test.set_player_funds(Player::p1, 0);
        test.give_deed(Player::p2, Property::Blue_2);

        WHEN("player 1 lands on Boardwalk") {
            test.land_on_space(Player::p1, Space::Blue_2);

            THEN("player 1 is bankrupted") {
                test.require_eliminated(Player::p1, true);
            }
        }
    }
    GIVEN("Player 1 has $0") {
        test.set_player_funds(Player::p1, 0);

        WHEN("player 1 lands on Luxury Tax") {
            test.land_on_space(Player::p1, Space::LuxuryTax);

            THEN("player 1 is bankrupted") {
                test.require_eliminated(Player::p1, true);
            }
        }
    }
}
