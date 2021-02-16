#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

#include <cassert>

SCENARIO("On a player's turn, they roll the dice and move the indicated number of spaces", "[movement]") {
	Test test;
	auto const startingFunds = test.game.get_state().get_player(Player::p1).funds;

    GIVEN ("Player 1 starts on GO") {
		test.move_player (Player::p1, Space::Go);

		WHEN("player 1 rolls a 4 and a 6") {
			test.roll(4, 6);

			THEN("player 1 advances 10 spaces") {
				test.require_position(Player::p1, Space::Jail);
			}
		}
    }

    GIVEN ("Player 1 starts on Short Line") {
		test.move_player(Player::p1, Space::Railroad_4);

		WHEN("player 1 rolls a 2 and a 3") {
			test.roll(2, 3);

			THEN("player 1 advances 5 spaces, passing GO and collecting 200 dollars") {
				test.require_position(Player::p1, Space::Go);
				test.require_funds(Player::p1, startingFunds + GoSalary);
			}
		}
    }
}

SCENARIO("On a player's turn, they roll doubles and get extra rolls", "[movement]") {
	Test test;

    GIVEN ("Player 1 starts on GO") {
		test.move_player (Player::p1, Space::Go);

		WHEN("player 1 rolls a 5 and a 5") {
			test.roll(5, 5);

			THEN("player 1 advances 10 spaces and gets another roll") {
				test.require_position(Player::p1, Space::Jail);
				test.require_phase(TurnPhase::WaitingForRoll);
				test.require_active_player(Player::p1);
			}

			AND_WHEN ("player 1 rolls doubles again") {
				test.roll(5, 5);
				test.require_position(Player::p1, Space::FreeParking);
				test.require_phase(TurnPhase::WaitingForRoll);
				test.require_active_player(Player::p1);

				AND_WHEN ("player 1 rolls doubles a third time") {
					test.roll(1, 1);

					THEN("the player immediately goes to jail for rolling 3 doubles in a row and the game moves on") {
						test.require_jailed(Player::p1, true);
					}
				}
			}
			AND_WHEN("player 1 stops rolling doubles") {
				test.roll(4, 6);

				THEN("the player simply moves, they aren't jailed, and the game moves on") {
					test.require_position(Player::p1, Space::FreeParking);
					test.require_jailed(Player::p1, false);
				}
			}
		}
    }
}

SCENARIO("Players can't leave jail without rolling doubles, using a GOJF card, or paying bail", "[movement]") {
	Test test;
	auto const startingFunds = test.game.get_state().get_player(Player::p1).funds;

    GIVEN ("Player 1 is in jail") {
		test.jail_player(Player::p1);
		test.set_active_player(Player::p1);

		WHEN("player 1 rolls non-doubles") {
			test.roll(4, 6);

			THEN("player 1 must stay in jail") {
				test.require_jailed(Player::p1, true);
			}
			AND_WHEN("player 1 rolls non-doubles 2 more times") {
				test.set_active_player(Player::p1);
				test.roll(4, 6);
				test.require_jailed(Player::p1, true);
				test.set_active_player(Player::p1);
				test.roll(4, 6);

				THEN("player 1 is forced to pay a fine and released") {
					test.require_funds(Player::p1, startingFunds - BailCost);
					test.require_jailed(Player::p1, false);
					test.require_position(Player::p1, Space::FreeParking);
				}
			}
		}
		WHEN("player 1 rolls doubles") {
			test.roll (5, 5);

			THEN("player 1 is able to leave jail and moves") {
				test.require_jailed(Player::p1, false);
				test.require_position(Player::p1, Space::FreeParking);
			}
		}
		AND_GIVEN("player 1 has a Get Out of Jail Free card") {
			test.give_get_out_of_jail_free_card(Player::p1);

			WHEN("player 1 uses a get out of jail free card") {
				test.use_get_out_of_jail_free_card();

				THEN("player 1 is able to leave jail") {
					test.require_jailed(Player::p1, false);
				}
			}
		}
		AND_GIVEN("player 1 has enough money to pay bail") {
			test.set_player_funds(Player::p1, startingFunds);

			WHEN("player 1 pays bail") {
				test.pay_bail();

				THEN("player 1 loses cost of bail and is able to leave jail") {
					test.require_funds(Player::p1, startingFunds - BailCost);
					test.require_jailed(Player::p1, false);
					test.require_phase(TurnPhase::WaitingForRoll);
				}
			}
		}
    }
}

SCENARIO("Rolling random dice always yields a valid dice roll", "[movement]") {
	Test test;

	for (auto i = 0; i < 50; ++i) {
		test.set_active_player(Player::p1);
		test.roll();
		auto const dice = test.game.get_state ().get_last_dice_roll();
		REQUIRE(1 <= dice.first);
		REQUIRE(dice.first <= 6);
		REQUIRE(1 <= dice.second);
		REQUIRE(dice.second <= 6);
	}
}
