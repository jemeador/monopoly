#include "Game.h"
#include "TestInterface.h"
using namespace monopoly;

#include "catch2/catch.hpp"

#include <cassert>
#include <thread>

SCENARIO("On a player's turn, they roll the dice and move the indicated number of spaces", "[turn]") {
	TestInterface interface;
	Game game (&interface);

    GIVEN ("Player 1 starts on GO") {
		REQUIRE(game.get_state ().get_player(Player::p1).position == Space::Go);

		WHEN("player 1 rolls a 4 and a 6") {
			interface.roll_loaded_dice(Player::p1, { 4, 6 });
			game.wait_for_processing();

			THEN("player 1 advances 10 spaces") {
				REQUIRE(game.get_state().get_player(Player::p1).position == Space::Jail);
			}
		}
    }

    GIVEN ("Player 1 starts on Short Line") {
		GameState state;
		state.force_position(Player::p1, Space::Railroad_4);
		REQUIRE(state.get_player(Player::p1).position == Space::Railroad_4);

		game.set_state(state);

		WHEN("player 1 rolls a 2 and a 3") {
			interface.roll_loaded_dice(Player::p1, { 2, 3 });
			game.wait_for_processing();

			THEN("player 1 advances 5 spaces, wrapping around to GO") {
				REQUIRE(game.get_state().get_player(Player::p1).position == Space::Go);
			}
		}
    }
}

SCENARIO("On a player's turn, they roll doubles and get extra rolls", "[turn]") {
	TestInterface interface;
	Game game (&interface);

    GIVEN ("Player 1 starts on GO") {
		REQUIRE(game.get_state ().get_player(Player::p1).position == Space::Go);

		WHEN("player 1 rolls a 5 and a 5") {
			interface.roll_loaded_dice(Player::p1, { 5, 5 });
			game.wait_for_processing();

			THEN("player 1 advances 10 spaces and gets another roll") {
				REQUIRE(game.get_state ().get_player(Player::p1).position == Space::Jail);
				REQUIRE(game.get_state ().get_turn_phase() == TurnPhase::TurnStart);
				REQUIRE(game.get_state ().get_active_player_index() == Player::p1);
			}

			AND_WHEN ("player 1 rolls doubles again") {
				interface.roll_loaded_dice(Player::p1, { 5, 5 });
				game.wait_for_processing();

				REQUIRE(game.get_state ().get_player(Player::p1).position == Space::FreeParking);
				REQUIRE(game.get_state ().get_turn_phase() == TurnPhase::TurnStart);
				REQUIRE(game.get_state ().get_active_player_index() == Player::p1);

				AND_WHEN ("player 1 rolls doubles a third time") {
					interface.roll_loaded_dice(Player::p1, { 1, 1 });
					game.wait_for_processing();

					THEN("the player immediately goes to jail for rolling 3 doubles in a row and the game moves on") {
						REQUIRE(game.get_state ().get_player(Player::p1).position == Space::Jail);
						REQUIRE(game.get_state ().get_player(Player::p1).turnsRemainingInJail == 3);
					}
				}
			}
			AND_WHEN("player 1 stops rolling doubles") {
				interface.roll_loaded_dice(Player::p1, { 4, 6 });
				game.wait_for_processing();

				THEN("the player simply moves, they aren't jailed, and the game moves on") {
					REQUIRE(game.get_state ().get_player(Player::p1).position == Space::FreeParking);
					REQUIRE(game.get_state ().get_player(Player::p1).turnsRemainingInJail == 0);
				}
			}
		}
    }
}

SCENARIO("It's a player's turn and they are in jail", "[turn]") {
	TestInterface interface;
	Game game (&interface);

    GIVEN ("Player 1 is in jail") {
		GameState state;
		state.force_go_to_jail(Player::p1);
		state.force_turn_start(Player::p1);
		REQUIRE(state.get_player(Player::p1).position == Space::Jail);
		REQUIRE(state.get_player(Player::p1).turnsRemainingInJail == 3);
		game.set_state(state);

		WHEN("player 1 rolls non-doubles") {
			interface.roll_loaded_dice(Player::p1, { 4, 6 });
			game.wait_for_processing();

			THEN("player 1 must stay in jail") {
				REQUIRE(game.get_state ().get_player(Player::p1).position == Space::Jail);
			}
			AND_WHEN("player 1 rolls non-doubles 2 more times") {
				// Skip to Player 1 for second roll
				state = game.get_state();
				state.force_turn_start(Player::p1);
				game.set_state(state);

				interface.roll_loaded_dice(Player::p1, { 4, 6 });
				game.wait_for_processing();

				// Verify in jail
				state = game.get_state();
				REQUIRE(state.get_player(Player::p1).position == Space::Jail);

				// Skip to Player 1 for third roll
				state.force_turn_start(Player::p1);
				game.set_state(state);

				interface.roll_loaded_dice(Player::p1, { 4, 6 });
				game.wait_for_processing();

				THEN("player 1 is forced to pay a fine and released") {
					REQUIRE(game.get_state ().get_player(Player::p1).position == Space::FreeParking);
					REQUIRE(game.get_state ().get_player(Player::p1).funds == state.get_player(Player::p1).funds - 50);
				}
			}
		}
		WHEN("player 1 rolls doubles") {
			interface.roll_loaded_dice(Player::p1, { 5, 5 });
			game.wait_for_processing();

			THEN("player 1 is able to leave jail") {
				REQUIRE(game.get_state().get_player(Player::p1).position == Space::FreeParking);
				REQUIRE(game.get_state().get_player(Player::p1).turnsRemainingInJail == 0);
			}
		}
		AND_GIVEN("player has a Get Out of Jail Free card") {
			state = game.get_state();
			state.force_get_out_of_jail_free_card_keep(Player::p1, Deck::Type::Chance);
			game.set_state(state);

			WHEN("player 1 uses a get out of jail free card") {
				interface.use_get_out_of_jail_free_card(Player::p1);
				game.wait_for_processing();

				THEN("player 1 is able to leave jail") {
					REQUIRE(game.get_state().get_player(Player::p1).position == Space::Jail);
					REQUIRE(game.get_state().get_player(Player::p1).turnsRemainingInJail == 0);
				}
			}
		}
    }
}
