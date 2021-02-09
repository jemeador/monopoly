#include "Cards.h"
#include "GameState.h"
#include "Strings.h"
using namespace monopoly;

#include "nlohmann/json.hpp"

#include <algorithm>
#include <iostream>
#include <ranges>


namespace {
	char const *lookup_key(DeckType deckType) {
		switch (deckType) {
		case DeckType::Chance: return "Chance";
		case DeckType::CommunityChest: return "CommunityChest";
		}
		return "";
	}
	char const *lookup_key(Card card) {
		switch (card) {
		case Card::Chance_AdvanceToBlue2: return "Chance_AdvanceToBlue2";
		case Card::Chance_AdvanceToGo: return "Chance_AdvanceToGo";
		case Card::Chance_AdvanceToMagenta1: return "Chance_AdvanceToMagenta1";
		case Card::Chance_AdvanceToNearestRailroad: return "Chance_AdvanceToNearestRailroad";
		case Card::Chance_AdvanceToNearestUtility: return "Chance_AdvanceToNearestUtility";
		case Card::Chance_AdvanceToRailroad1: return "Chance_AdvanceToRailroad1";
		case Card::Chance_AdvanceToRed3: return "Chance_AdvanceToRed3";
		case Card::Chance_Gain150: return "Chance_Gain150";
		case Card::Chance_Gain50: return "Chance_Gain50";
		case Card::Chance_GetOutOfJailFree: return "Chance_GetOutOfJailFree";
		case Card::Chance_GoBack3Spaces: return "Chance_GoBack3Spaces";
		case Card::Chance_GoToJail: return "Chance_GoToJail";
		case Card::Chance_Pay15: return "Chance_Pay15";
		case Card::Chance_PayEachPlayer50: return "Chance_PayEachPlayer50";
		case Card::Chance_Repairs: return "Chance_Repairs";
		case Card::CommunityChest_AdvanceToGo: return "CommunityChest_AdvanceToGo";
		case Card::CommunityChest_CollectFromEachPlayer50: return "CommunityChest_CollectFromEachPlayer50";
		case Card::CommunityChest_Gain10: return "CommunityChest_Gain10";
		case Card::CommunityChest_Gain100_A: return "CommunityChest_Gain100_A";
		case Card::CommunityChest_Gain100_B: return "CommunityChest_Gain100_B";
		case Card::CommunityChest_Gain100_C: return "CommunityChest_Gain100_C";
		case Card::CommunityChest_Gain20: return "CommunityChest_Gain20";
		case Card::CommunityChest_Gain200: return "CommunityChest_Gain200";
		case Card::CommunityChest_Gain25: return "CommunityChest_Gain25";
		case Card::CommunityChest_Gain45: return "CommunityChest_Gain45";
		case Card::CommunityChest_GetOutOfJailFree: return "CommunityChest_GetOutOfJailFree";
		case Card::CommunityChest_GoToJail: return "CommunityChest_GoToJail";
		case Card::CommunityChest_Pay100: return "CommunityChest_Pay100";
		case Card::CommunityChest_Pay150: return "CommunityChest_Pay150";
		case Card::CommunityChest_Pay50: return "CommunityChest_Pay50";
		case Card::CommunityChest_Repairs: return "CommunityChest_Repairs";
		}
		return "";
	}

	int distance(Space from, Space to) {
		auto d = static_cast<int> (to) - static_cast<int> (from);
		if (d < 0)
			d += NumberOfSpaces;
		return d;
	}

	Space nearest_space (Space pos, std::vector<Space> const &spaces) {
		std::vector<int> distances;
		std::transform(begin(spaces), end(spaces), begin(distances), [pos](Space space) { return distance (pos, space);});
		auto const distIt = std::min_element(begin(distances), end(distances));
		auto const spaceIt = begin(spaces) + (distIt - begin(distances));
		return *spaceIt;
	}
}

std::string monopoly::to_string(DeckType deckType) {
	try {
		return string_data().at("labels").at(lookup_key(deckType)).get<std::string>();
	}
	catch (...) {
		return "N/A";
	}
}

CardData const& monopoly::card_data(Card card) {
	static std::unordered_map<Card, CardData> const card_data = []() {
		std::unordered_map<Card, CardData> ret;
		for (auto i = 0; i < ChanceCards.size() + CommunityChestCards.size(); ++i) {
			Card c = static_cast<Card> (i);
			auto& singleCardData = ret[c];
			try {
				singleCardData.flavorText = card_flavor_text(lookup_key(c));
				singleCardData.effectText = card_effect_text(lookup_key(c));
			}
			catch (...) {
				std::cerr << "Bad definition for card \"" << lookup_key(c) << "\"" << std::endl;
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
			state.force_advance_to(playerIndex, dest);
		};
		ret[Card::Chance_AdvanceToNearestUtility] = [](GameState& state, int playerIndex) {
			auto const pos = state.get_player(playerIndex).position;
			auto const dest = nearest_space(pos, { Space::Utility_1, Space::Utility_2 });
			state.force_advance_to(playerIndex, dest);
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
			state.force_keep_get_out_of_jail_free_card(playerIndex, DeckType::Chance);
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
			/// todo
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
			state.force_keep_get_out_of_jail_free_card(playerIndex, DeckType::CommunityChest);
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
			/// todo
		};
		return ret;
	} ();
	auto const& effect = card_effects.at(card);
	effect(state, playerIndex);
}
