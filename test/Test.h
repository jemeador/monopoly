#pragma once

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

        inline void change_state(std::function<void(GameState&)> func) {
            auto state = game.get_state();
            func(state);
            game.set_state(state);
        }

        // Most state information can be inferred from monopoly's predictable
        // rules and cheating a bit with the dice.

        inline void set_active_player(int playerIndex) {
            change_state(std::bind(&GameState::force_start_turn, std::placeholders::_1, playerIndex));
        }
        inline void roll(int playerIndex, int d6a, int d6b) {
            change_state(std::bind(&GameState::force_roll, std::placeholders::_1, playerIndex, std::make_pair(d6a, d6b)));
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
        inline void stack_deck(DeckType deckType, DeckContainer const& cardsToPlaceOnTop) {
            change_state(std::bind(&GameState::force_stack_deck, std::placeholders::_1, deckType, cardsToPlaceOnTop));
        }
        inline void land_on_space(int playerIndex, Space space) {
            change_state(std::bind(&GameState::force_land, std::placeholders::_1, playerIndex, space));
        }

        inline void roll() {
            auto const pi = game.get_state().get_active_player_index();
            interface.roll_dice(pi);
            game.process();
        }
        inline void buy_property() {
            auto const pi = game.get_state().get_active_player_index();
            interface.buy_property(pi);
            game.process();
        }
        inline void auction_property() {
            auto const pi = game.get_state().get_active_player_index();
            interface.auction_property(pi);
            game.process();
        }
        inline void mortgage_property(int playerIndex, Property property) {
            interface.mortgage_property(playerIndex, property);
            game.process();
        }
        inline void unmortgage_property(Property property) {
            auto const pi = game.get_state().get_active_player_index();
            interface.unmortgage_property(pi, property);
            game.process();
        }
        inline void buy_building(Property property) {
            auto const pi = game.get_state().get_active_player_index();
            interface.buy_building(pi, property);
            game.process();
        }
        inline void sell_building(Property property) {
            auto const pi = game.get_state().get_active_player_index();
            interface.sell_building(pi, property);
            game.process();
        }
        inline void sell_all_buildings(PropertyGroup group) {
            auto const pi = game.get_state().get_active_player_index();
            interface.sell_all_buildings(pi, group);
            game.process();
        }
        inline void pay_bail() {
            auto const pi = game.get_state().get_active_player_index();
            interface.pay_bail(pi);
            game.process();
        }
        inline void use_get_out_of_jail_free_card() {
            auto const pi = game.get_state().get_active_player_index();
            interface.use_get_out_of_jail_free_card(pi);
            game.process();
        }
        inline void bid(int playerIndex, int bid) {
            interface.bid(playerIndex, bid);
            game.process();
        }
        inline void decline_bid(int playerIndex) {
            interface.decline_bid(playerIndex);
            game.process();
        }
        inline void propose_trade(Trade trade) {
            interface.propose_trade(trade);
            game.process();
        }
        inline void decline_trade(int playerIndex) {
            interface.decline_trade(playerIndex);
            game.process();
        }
        inline void resign(int playerIndex) {
            interface.resign(playerIndex);
            game.process();
        }

        inline void require_eliminated(int playerIndex, bool eliminated) {
            REQUIRE(game.get_state().get_player_eliminated(playerIndex) == eliminated);
        }
        inline void require_position(int playerIndex, Space space) {
            REQUIRE(game.get_state().get_player_position(playerIndex) == space);
        }
        inline void require_funds(int playerIndex, int funds) {
            REQUIRE(game.get_state().get_player_funds(playerIndex) == funds);
        }
        inline void require_has_deed(int playerIndex, Property property) {
            auto d = game.get_state().get_player_deeds(playerIndex);
            REQUIRE(std::any_of(d.begin(), d.end(),
                [property](Property p) { return p == property; }));
        }
        inline void require_does_not_have_deed(int playerIndex, Property property) {
            auto d = game.get_state().get_player_deeds(playerIndex);
            REQUIRE(std::none_of(d.begin(), d.end(),
                [property](Property p) { return p == property; }));
        }
        inline void require_is_mortgaged(Property property, bool mortgaged = true) {
            REQUIRE(game.get_state().get_property_is_mortgaged(property) == mortgaged);
        }
        inline void require_building_level(Property property, int buildingLevel) {
            REQUIRE(game.get_state().get_building_level(property) == buildingLevel);
        }

        inline void require_has_get_out_of_jail_free(int playerIndex, DeckType deckType) {
            auto c = game.get_state().get_player_get_out_of_jail_free_cards(playerIndex);
            REQUIRE(c.count(deckType) == 1);
        }
        inline void require_does_not_have_get_out_of_jail_free(int playerIndex, DeckType deckType) {
            auto c = game.get_state().get_player_get_out_of_jail_free_cards(playerIndex);
            REQUIRE(c.count(deckType) == 0);
        }

        inline void require_jailed(int playerIndex, bool jailed) {
            if (jailed) {
                REQUIRE(game.get_state().get_player_turns_remaining_in_jail (playerIndex) > 0);
                require_position(playerIndex, Space::Jail);
            }
            else {
                REQUIRE(game.get_state().get_player_turns_remaining_in_jail (playerIndex) == 0);
            }
        }
        inline void require_phase(TurnPhase phase) {
            REQUIRE(game.get_state().get_turn_phase() == phase);
        }
        inline void require_not_phase(TurnPhase phase) {
            REQUIRE(game.get_state().get_turn_phase() != phase);
        }
        inline void require_active_player(int playerIndex) {
            REQUIRE(game.get_state().get_active_player_index() == playerIndex);
        }
        inline void require_highest_bid(int bidderIndex, int bid) {
            auto const auction = game.get_state().get_current_auction();
            REQUIRE(auction);
            REQUIRE(auction.highestBid == bid);
            REQUIRE(auction.biddingOrder.back () == bidderIndex);
        }
    };
}
