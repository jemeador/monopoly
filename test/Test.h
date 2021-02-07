#include "Game.h"
#include "TestInterface.h"
#include "Board.h"

#include "catch2/catch.hpp"

namespace monopoly
{
	struct Test {
		TestInterface interface;
		Game game;

		Test()
			: interface()
			, game(&interface) {
		}

		inline void set_active_player(int playerIndex) {
			auto state = game.get_state();
			state.force_turn_start(playerIndex);
			game.set_state(state);
		}
		inline void move_player(int playerIndex, Space space) {
			auto state = game.get_state();
			state.force_position(playerIndex, space);
			game.set_state(state);
		}
		inline void jail_player(int playerIndex) {
			auto state = game.get_state();
			state.force_go_to_jail(Player::p1);
			game.set_state(state);
		}
		inline void set_player_funds(int playerIndex, int funds) {
			auto state = game.get_state();
			state.force_funds(Player::p1, funds);
			game.set_state(state);
		}
		inline void give_get_out_of_jail_free_card(int playerIndex, Deck::Type deckType = Deck::Type::Chance) {
			auto state = game.get_state();
			state.force_get_out_of_jail_free_card_keep(Player::p1, deckType);
			game.set_state(state);
		}


		inline void roll(int a, int b) {
			auto const pi = game.get_state().get_active_player_index();
			interface.roll_loaded_dice(pi, { a, b });
			game.wait_for_processing();
		}
		inline void pay_bail() {
			auto const pi = game.get_state().get_active_player_index();
			interface.pay_bail(Player::p1);
			game.wait_for_processing();
		}
		inline void use_get_out_of_jail_free_card() {
			auto const pi = game.get_state().get_active_player_index();
			interface.use_get_out_of_jail_free_card(Player::p1);
			game.wait_for_processing();
		}

		inline void require_position(int playerIndex, Space space) {
			REQUIRE(game.get_state().get_player(playerIndex).position == space);
		}
		inline void require_funds(int playerIndex, int funds) {
			REQUIRE(game.get_state().get_player(playerIndex).funds == funds);
		}
		inline void require_jailed(int playerIndex, bool jailed) {
			if (jailed) {
				REQUIRE(game.get_state().get_player(playerIndex).turnsRemainingInJail > 0);
				require_position(playerIndex, Space::Jail);
			}
			else {
				REQUIRE(game.get_state().get_player(playerIndex).turnsRemainingInJail == 0);
			}
		}
		inline void require_phase(TurnPhase phase) {
			REQUIRE(game.get_state().get_turn_phase() == phase);
		}
		inline void require_active_player(int playerIndex) {
			REQUIRE(game.get_state().get_active_player_index() == playerIndex);
		}

	};
}
