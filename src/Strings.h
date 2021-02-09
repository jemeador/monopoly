#pragma once

#include "nlohmann/json.hpp"

namespace monopoly {

	nlohmann::json const& string_data();
	std::string label(std::string const& lookup_key);
	std::string replace_labels(std::string text);
	std::string card_flavor_text(std::string const& lookup_key);
	std::string card_effect_text(std::string const& lookup_key);
}
