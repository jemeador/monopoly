#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

#include <cassert>

SCENARIO("When a player lands on Income Tax, they pay the lesser of $200 or 10% of their net worth", "[special]") {
	Test test;

    GIVEN ("Player 1's net worth is less than $2000") {
		int const startingFunds = 1990;
		test.set_player_funds (Player::p1, startingFunds);
		test.move_player (Player::p1, Space::Go);

		WHEN("player 1 lands on Income Tax") {
			test.roll(1, 3);

			THEN("player 1 loses 10 % of their funds") {
				test.require_funds(Player::p1, startingFunds - 199);
			}
		}
    }

    GIVEN ("Player 1's net worth is more than $2000") {
		int const startingFunds = 2010;
		test.set_player_funds (Player::p1, startingFunds);
		test.move_player (Player::p1, Space::Go);

		WHEN("player 1 lands on Income Tax") {
			test.roll(1, 3);

			THEN("player 1 loses $200 of their funds") {
				test.require_funds(Player::p1, startingFunds - 200);
			}
		}
    }

	GIVEN("Player 1 is on GO") {
		test.move_player (Player::p1, Space::Go);

		WHEN("player 1 rolls (1,1) and lands on Community Chest") {
			test.roll(1, 2);

			THEN("player 1 draws a Community Chest card") {
			}
		}
	}
}
