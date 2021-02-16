#pragma once

#include "Cards.h"
#include "Board.h"

namespace monopoly {
    std::string to_string(Space space);
    std::string to_string(Property property);
    std::string to_string(PropertyGroup group);
    std::string to_string(DeckType deckType);
    std::string to_string(Card card);
    std::string card_flavor_text(Card card);
    std::string card_effect_text(Card card);

    std::string label(std::string const& lookup_key);
    std::string replace_labels(std::string text);
}
