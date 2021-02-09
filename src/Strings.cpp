#include "Strings.h"
using namespace monopoly;

using json = nlohmann::json;

#include <fstream>
#include <iostream>

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

std::string monopoly::card_flavor_text(std::string const &lookupKey) {
	auto const& card_strings = string_data().at("cards");
	return replace_labels(card_strings.at(lookupKey).at("flavorText").get<std::string>());
}

std::string monopoly::card_effect_text(std::string const &lookupKey) {
	auto const& card_strings = string_data().at("cards");
	return replace_labels(card_strings.at(lookupKey).at("effectText").get<std::string>());
}
