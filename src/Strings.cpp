#include "Strings.h"
using namespace monopoly;

using json = nlohmann::json;

#include <fstream>
#include <iostream>

namespace {
	char const *lookup_key(Property property) {
		switch (property) {
		case Property::Brown_1: return "Brown_1";
		case Property::Brown_2: return "Brown_2";
		case Property::LightBlue_1: return "LightBlue_1";
		case Property::LightBlue_2: return "LightBlue_2";
		case Property::LightBlue_3: return "LightBlue_3";
		case Property::Magenta_1: return "Magenta_1";
		case Property::Magenta_2: return "Magenta_2";
		case Property::Magenta_3: return "Magenta_3";
		case Property::Orange_1: return "Orange_1";
		case Property::Orange_2: return "Orange_2";
		case Property::Orange_3: return "Orange_3";
		case Property::Red_1: return "Red_1";
		case Property::Red_2: return "Red_2";
		case Property::Red_3: return "Red_3";
		case Property::Yellow_1: return "Yellow_1";
		case Property::Yellow_2: return "Yellow_2";
		case Property::Yellow_3: return "Yellow_3";
		case Property::Green_1: return "Green_1";
		case Property::Green_2: return "Green_2";
		case Property::Green_3: return "Green_3";
		case Property::Blue_1: return "Blue_1";
		case Property::Blue_2: return "Blue_2";
		case Property::Utility_1: return "Utility_1";
		case Property::Utility_2: return "Utility_2";
		case Property::Railroad_1: return "Railroad_1";
		case Property::Railroad_2: return "Railroad_2";
		case Property::Railroad_3: return "Railroad_3";
		case Property::Railroad_4: return "Railroad_4";
		}
		return "";
	}

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

}

json const& monopoly::string_data() {
	static json const strings = []() {
		try {
			std::string const path(DATA_INSTALL_PREFIX);
			std::ifstream i(path + "strings.json");
			json j;
			i >> j;
			return j;
		}
		catch (std::exception const& e) {
			std::cerr << e.what() << std::endl;
			return json{};
		}
	} ();
	return strings;
}

std::string monopoly::to_string(Property property) {
	return label(lookup_key(property));
}

std::string monopoly::to_string(DeckType deckType) {
	return label(lookup_key(deckType));
}

std::string monopoly::to_string(Card card) {
	return lookup_key(card);
}

std::string monopoly::card_flavor_text(Card card) {
	auto const& card_strings = string_data().at("cards");
	return replace_labels(card_strings.at(lookup_key (card)).at("flavorText").get<std::string>());
}

std::string monopoly::card_effect_text(Card card) {
	auto const& card_strings = string_data().at("cards");
	return replace_labels(card_strings.at(lookup_key (card)).at("effectText").get<std::string>());
}

std::string monopoly::label(std::string const &lookupKey) {
	return string_data().at("labels").at(lookupKey).get<std::string>();
}

std::string monopoly::replace_labels(std::string text) {
	auto pos = 0;
	static std::string const escapeToken = "$${";
	while ((pos = text.find(escapeToken, pos)) != -1) {
		auto const replaceBegin = pos;
		auto const replaceLen = text.find("}", pos) - replaceBegin + 1;
		auto const keyBegin = replaceBegin + escapeToken.size ();
		auto const keyLen = replaceLen - (escapeToken.size() + 1);
		auto const lookupKey = text.substr(keyBegin, keyLen);
		auto const labelStr = label(lookupKey);
		text.replace(replaceBegin, replaceLen, labelStr);
		pos = replaceLen + replaceBegin;
	}
	return text;
}