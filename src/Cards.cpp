#include "Cards.h"
#include "GameState.h"
#include "Strings.h"
#include "Board.h"
using namespace monopoly;

#include "nlohmann/json.hpp"

#include <algorithm>
#include <iostream>

namespace {
	int calculate_building_repair_cost(GameState const& state, int playerIndex, int pricePerHouse, int pricePerHotel) {
		int cost = 0;
		for (auto propertyBuildingLevelPair : state.get_building_levels()) {
			auto const& property = propertyBuildingLevelPair.first;
			if (state.get_property_owner_index(property) == playerIndex) {
				auto const buildingLevel = propertyBuildingLevelPair.second;
				if (buildingLevel <= 4)
					cost += buildingLevel * pricePerHouse;
				else
					cost += pricePerHotel;
			}
		}
		return cost;
	}
}

CardData const& monopoly::card_data(Card card) {
	static std::unordered_map<Card, CardData> const card_data = []() {
		std::unordered_map<Card, CardData> ret;
		for (auto i = 0; i < ChanceCards.size() + CommunityChestCards.size(); ++i) {
			Card c = static_cast<Card> (i);
			auto& singleCardData = ret[c];
			try {
				singleCardData.flavorText = card_flavor_text(c);
				singleCardData.effectText = card_effect_text(c);
			}
			catch (...) {
				std::cerr << "Bad definition for card \"" << to_string(c) << "\"" << std::endl;
			}
		}
		return ret;
	} ();
	return card_data.at (card);
}

void monopoly::apply_card_effect (GameState& state, int playerIndex, Card card) {
	static std::unordered_map<Card, CardEffect> const card_effects = []() {
		std::unordered_map<Card, CardEffect> ret;
		ret[Card::Chance_AdvanceToBlue2] = [](GameState& state, int playerIndex) {
			state.force_advance_to(playerIndex, Space::Blue_2);
		};
		ret[Card::Chance_AdvanceToGo] = [](GameState& state, int playerIndex) {
			state.force_advance_to(playerIndex, Space::Go);
		};
		ret[Card::Chance_AdvanceToMagenta1] = [](GameState& state, int playerIndex) {
			state.force_advance_to(playerIndex, Space::Magenta_1);
		};
		ret[Card::Chance_AdvanceToNearestRailroad] = [](GameState& state, int playerIndex) {
			auto const pos = state.get_player(playerIndex).position;
			auto const dest = nearest_space(pos, { Space::Railroad_1, Space::Railroad_2, Space::Railroad_3, Space::Railroad_4 });
			state.force_advance_to_without_landing(playerIndex, dest);
			auto const property = space_to_property(dest);
			if (auto const ownerOpt = state.get_property_owner_index(property)) {
				auto const owner = *ownerOpt;
				auto const ownedGroupCount = state.get_properties_owned_in_group_by_player(owner, PropertyGroup::Railroad);
				// Rent is doubled
				state.force_transfer_funds(playerIndex, owner, 2 * rent_price_of_railroad(ownedGroupCount));
			}
			else {
				state.force_property_offer(playerIndex, property);
			}
		};
		ret[Card::Chance_AdvanceToNearestUtility] = [](GameState& state, int playerIndex) {
			auto const pos = state.get_player(playerIndex).position;
			auto const dest = nearest_space(pos, { Space::Utility_1, Space::Utility_2 });
			state.force_advance_to_without_landing(playerIndex, dest);
			auto const property = space_to_property(dest);
			if (auto const ownerOpt = state.get_property_owner_index(property)) {
				auto const owner = *ownerOpt;
				// Pay 10 times a random roll
				auto const roll = state.random_dice_roll();
				auto const sum = roll.first + roll.second;
				state.force_transfer_funds(playerIndex, owner, 10 * sum);
			}
			else {
				state.force_property_offer(playerIndex, property);
			}
		};
		ret[Card::Chance_AdvanceToRailroad1] = [](GameState& state, int playerIndex) {
			state.force_advance_to(playerIndex, Space::Railroad_1);
		};
		ret[Card::Chance_AdvanceToRed3] = [](GameState& state, int playerIndex) {
			state.force_advance_to(playerIndex, Space::Red_3);
		};
		ret[Card::Chance_Gain150] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 150);
		};
		ret[Card::Chance_Gain50] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 50);
		};
		ret[Card::Chance_GetOutOfJailFree] = [](GameState& state, int playerIndex) {
			state.force_give_get_out_of_jail_free_card(playerIndex, DeckType::Chance);
		};
		ret[Card::Chance_GoBack3Spaces] = [](GameState& state, int playerIndex) {
			state.force_advance(playerIndex, -3);
		};
		ret[Card::Chance_GoToJail] = [](GameState& state, int playerIndex) {
			state.force_go_to_jail(playerIndex);
		};
		ret[Card::Chance_Pay15] = [](GameState& state, int playerIndex) {
			state.force_subtract_funds(playerIndex, 15);
		};
		ret[Card::Chance_PayEachPlayer50] = [](GameState& state, int playerIndex) {
			for (auto p = 0; p < state.get_player_count(); ++p) {
				state.force_transfer_funds(playerIndex, p, 50);
			}
		};
		ret[Card::Chance_Repairs] = [](GameState& state, int playerIndex) {
			state.force_subtract_funds (playerIndex, calculate_building_repair_cost(state, playerIndex, 25, 100));
		};
		ret[Card::CommunityChest_AdvanceToGo] = [](GameState& state, int playerIndex) {
			state.force_advance_to(playerIndex, Space::Go);
		};
		ret[Card::CommunityChest_CollectFromEachPlayer50] = [](GameState& state, int playerIndex) {
			for (auto p = 0; p < state.get_player_count(); ++p) {
				state.force_transfer_funds(p, playerIndex, 50);
			}
		};
		ret[Card::CommunityChest_Gain10] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 10);
		};
		ret[Card::CommunityChest_Gain100_A] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 100);
		};
		ret[Card::CommunityChest_Gain100_B] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 100);
		};
		ret[Card::CommunityChest_Gain100_C] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 100);
		};
		ret[Card::CommunityChest_Gain20] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 20);
		};
		ret[Card::CommunityChest_Gain200] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 200);
		};
		ret[Card::CommunityChest_Gain25] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 25);
		};
		ret[Card::CommunityChest_Gain45] = [](GameState& state, int playerIndex) {
			state.force_add_funds(playerIndex, 40);
		};
		ret[Card::CommunityChest_GetOutOfJailFree] = [](GameState& state, int playerIndex) {
			state.force_give_get_out_of_jail_free_card(playerIndex, DeckType::CommunityChest);
		};
		ret[Card::CommunityChest_GoToJail] = [](GameState& state, int playerIndex) {
			state.force_go_to_jail(playerIndex);
		};
		ret[Card::CommunityChest_Pay100] = [](GameState& state, int playerIndex) {
			state.force_subtract_funds(playerIndex, 100);
		};
		ret[Card::CommunityChest_Pay150] = [](GameState& state, int playerIndex) {
			state.force_subtract_funds(playerIndex, 150);
		};
		ret[Card::CommunityChest_Pay50] = [](GameState& state, int playerIndex) {
			state.force_subtract_funds(playerIndex, 50);
		};
		ret[Card::CommunityChest_Repairs] = [](GameState& state, int playerIndex) {
			state.force_subtract_funds (playerIndex, calculate_building_repair_cost(state, playerIndex, 40, 115));
		};
		return ret;
	} ();
	auto const& effect = card_effects.at(card);
	effect(state, playerIndex);
}
