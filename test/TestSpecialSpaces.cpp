#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

#include <cassert>

SCENARIO("When a player lands on Income Tax, they pay the lesser of $200 or 10% of their net worth", "[special]") {
	Test test;

	GIVEN("Player 1's net worth is less than $2000") {
		int const startingFunds = 1990;
		test.set_player_funds(Player::p1, startingFunds);
		test.move_player(Player::p1, Space::Go);

		WHEN("player 1 lands on Income Tax") {
			test.roll(1, 3);

			THEN("player 1 loses 10 % of their funds") {
				test.require_funds(Player::p1, startingFunds - 199);
			}
		}
	}

	GIVEN("Player 1's net worth is more than $2000") {
		int const startingFunds = 2010;
		test.set_player_funds(Player::p1, startingFunds);
		test.move_player(Player::p1, Space::Go);

		WHEN("player 1 lands on Income Tax") {
			test.roll(1, 3);

			THEN("player 1 loses $200 of their funds") {
				test.require_funds(Player::p1, startingFunds - 200);
			}
		}
	}
}

SCENARIO("When a player lands on luxury tax they lose $100", "[special]") {
	Test test;

	GIVEN("Player 1's is on Short Line") {
		int const startingFunds = 1500;
		test.set_player_funds(Player::p1, startingFunds);
		test.move_player(Player::p1, Space::Railroad_4);

		WHEN("player 1 rolls a (1,2) and lands on Luxury Tax") {
			test.roll(1, 2);

			THEN("player 1 loses $100 of their funds") {
				test.require_funds(Player::p1, startingFunds - 100);
			}
		}
	}
}

SCENARIO("When a player lands on the \"Go to Jail\" space, they go directly to jail!", "[special]") {
	Test test;

	GIVEN("Player 1 is on Yellow 1") {
		int const startingFunds = 1500;
		test.move_player(Player::p1, Space::Yellow_2);

		WHEN("player 1 rolls (1,2) and lands on Go to Jail") {
			test.roll(1, 2);

			THEN("player 1 goes directly to jail, does not pass go, does not collect $200") {
				test.require_position(Player::p1, Space::Jail);
				test.require_jailed(Player::p1, true);
				test.require_funds(Player::p1, startingFunds);
			}
		}
	}
}
