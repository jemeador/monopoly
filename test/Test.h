#include "Game.h"
#include "TestInterface.h"
#include "Board.h"

#include "catch2/catch.hpp"

#include <functional>

namespace monopoly
{
	struct Test {
		TestInterface interface;
		Game game;

		Test()
			: interface()
			, game(&interface) {
		}

		inline void change_state(std::function<void(GameState &)> func) {
			auto state = game.get_state();
			func(state);
			game.set_state(state);
		}

		// Most state information can be inferred from monopoly's predictable
		// rules and cheating a bit with the dice.

		inline std::pair<int, int> get_last_dice_roll(int playerIndex) const {
		}

		inline void set_active_player(int playerIndex) {
			change_state(std::bind(&GameState::force_turn_start, std::placeholders::_1, playerIndex));
		}
		inline void move_player(int playerIndex, Space space) {
			change_state(std::bind(&GameState::force_position, std::placeholders::_1, playerIndex, space));
		}
		inline void jail_player(int playerIndex) {
			change_state(std::bind(&GameState::force_go_to_jail, std::placeholders::_1, playerIndex));
		}
		inline void set_player_funds(int playerIndex, int funds) {
			change_state(std::bind(&GameState::force_funds, std::placeholders::_1, playerIndex, funds));
		}
		inline void give_deed(int playerIndex, Property property) {
			change_state(std::bind(&GameState::force_give_deed, std::placeholders::_1, playerIndex, property));
		}
		inline void give_deeds(int playerIndex, std::set<Property> properties) {
			change_state(std::bind(&GameState::force_give_deeds, std::placeholders::_1, playerIndex, properties));
		}
		inline void set_buildings(std::map<Property, int> buildingLevels) {
			change_state(std::bind(&GameState::force_set_building_levels, std::placeholders::_1, buildingLevels));
		}
		inline void give_get_out_of_jail_free_card(int playerIndex, DeckType deckType = DeckType::Chance) {
			change_state(std::bind(&GameState::force_give_get_out_of_jail_free_card, std::placeholders::_1, playerIndex, deckType));
		}
		inline void stack_deck(DeckType deckType, DeckContainer const &cardsToPlaceOnTop) {
			change_state(std::bind(&GameState::force_stack_deck, std::placeholders::_1, deckType, cardsToPlaceOnTop));
		}
		inline void land_on_space(int playerIndex, Space space) {
			change_state(std::bind(&GameState::force_land, std::placeholders::_1, playerIndex, space));
		}

		inline void roll() {
			auto const pi = game.get_state().get_active_player_index();
			interface.roll_dice(pi);
			game.wait_for_processing();
		}
		inline void roll(int a, int b) {
			auto const pi = game.get_state().get_active_player_index();
			interface.roll_loaded_dice(pi, {a, b});
			game.wait_for_processing();
		}
		inline void buy_property() {
			auto const pi = game.get_state().get_active_player_index();
			interface.buy_property(pi);
			game.wait_for_processing();
		}
		inline void auction_property() {
			auto const pi = game.get_state().get_active_player_index();
			interface.auction_property(pi);
			game.wait_for_processing();
		}
		inline void pay_bail() {
			auto const pi = game.get_state().get_active_player_index();
			interface.pay_bail(pi);
			game.wait_for_processing();
		}
		inline void use_get_out_of_jail_free_card() {
			auto const pi = game.get_state().get_active_player_index();
			interface.use_get_out_of_jail_free_card(pi);
			game.wait_for_processing();
		}

		inline void require_eliminated(int playerIndex, bool eliminated) {
			REQUIRE(game.get_state().get_player(playerIndex).eliminated == eliminated);
		}
		inline void require_position(int playerIndex, Space space) {
			REQUIRE(game.get_state().get_player(playerIndex).position == space);
		}
		inline void require_funds(int playerIndex, int funds) {
			REQUIRE(game.get_state().get_player(playerIndex).funds == funds);
		}
		inline void require_has_deed(int playerIndex, Property property) {
			auto d = game.get_state ().get_player(playerIndex).deeds;
			REQUIRE(std::any_of(d.begin(), d.end(),
				[property](Property p) { return p == property; }));
		}
		inline void require_does_not_have_deed(int playerIndex, Property property) {
			auto d = game.get_state ().get_player(playerIndex).deeds;
			REQUIRE(std::none_of(d.begin(), d.end(),
				[property](Property p) { return p == property; }));
		}

		inline void require_has_get_out_of_jail_free(int playerIndex, DeckType deckType) {
			auto c = game.get_state ().get_player(playerIndex).getOutOfJailFreeCards;
			REQUIRE(c.count(deckType) ==  1);
		}
		inline void require_does_not_have_get_out_of_jail_free(int playerIndex, DeckType deckType) {
			auto c = game.get_state ().get_player(playerIndex).getOutOfJailFreeCards;
			REQUIRE(c.count(deckType) ==  0);
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
