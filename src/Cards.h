#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

namespace monopoly
{
    class GameState;

    enum class DeckType
    {
        Chance,
        CommunityChest,
    };

    enum class Card
    {
        Chance_AdvanceToBlue2,
        Chance_AdvanceToGo,
        Chance_AdvanceToMagenta1,
        Chance_AdvanceToNearestRailroad,
        Chance_AdvanceToNearestUtility,
        Chance_AdvanceToRailroad1,
        Chance_AdvanceToRed3,
        Chance_Gain150,
        Chance_Gain50,
        Chance_GetOutOfJailFree,
        Chance_GoBack3Spaces,
        Chance_GoToJail,
        Chance_Pay15,
        Chance_PayEachPlayer50,
        Chance_Repairs,
        CommunityChest_AdvanceToGo,
        CommunityChest_CollectFromEachPlayer50,
        CommunityChest_Gain10,
        CommunityChest_Gain100_A,
        CommunityChest_Gain100_B,
        CommunityChest_Gain100_C,
        CommunityChest_Gain20,
        CommunityChest_Gain200,
        CommunityChest_Gain25,
        CommunityChest_Gain45,
        CommunityChest_GetOutOfJailFree,
        CommunityChest_GoToJail,
        CommunityChest_Pay100,
        CommunityChest_Pay150,
        CommunityChest_Pay50,
        CommunityChest_Repairs,
    };

    using DeckContainer = std::vector<Card>;

    static auto const ChanceCards = DeckContainer{
        Card::Chance_AdvanceToBlue2,
        Card::Chance_AdvanceToGo,
        Card::Chance_AdvanceToMagenta1,
        Card::Chance_AdvanceToNearestRailroad,
        Card::Chance_AdvanceToNearestUtility,
        Card::Chance_AdvanceToRailroad1,
        Card::Chance_AdvanceToRed3,
        Card::Chance_Gain150,
        Card::Chance_Gain50,
        Card::Chance_GetOutOfJailFree,
        Card::Chance_GoBack3Spaces,
        Card::Chance_GoToJail,
        Card::Chance_Pay15,
        Card::Chance_PayEachPlayer50,
        Card::Chance_Repairs,
    };
    static auto const CommunityChestCards = DeckContainer{
        Card::CommunityChest_AdvanceToGo,
        Card::CommunityChest_CollectFromEachPlayer50,
        Card::CommunityChest_Gain10,
        Card::CommunityChest_Gain100_A,
        Card::CommunityChest_Gain100_B,
        Card::CommunityChest_Gain100_C,
        Card::CommunityChest_Gain20,
        Card::CommunityChest_Gain200,
        Card::CommunityChest_Gain25,
        Card::CommunityChest_Gain45,
        Card::CommunityChest_GetOutOfJailFree,
        Card::CommunityChest_GoToJail,
        Card::CommunityChest_Pay100,
        Card::CommunityChest_Pay150,
        Card::CommunityChest_Pay50,
        Card::CommunityChest_Repairs,
    };

    struct CardData
    {
        std::string flavorText;
        std::string effectText;
    };

    using CardEffect = std::function<void(GameState& state, int playerIndex)>;

    std::string to_string(DeckType deckType);
    CardData const& card_data(Card card);
    void apply_card_effect(GameState& state, int playerIndex, Card card);

    inline bool card_is_get_out_of_jail_free(Card card) {
        return (
            card == Card::Chance_GetOutOfJailFree ||
            card == Card::CommunityChest_GetOutOfJailFree
            );
    }

    inline Card get_out_of_jail_free_card(DeckType deckType) {
        switch (deckType) {
        case DeckType::Chance:
            return Card::Chance_GetOutOfJailFree;
        case DeckType::CommunityChest:
            return Card::CommunityChest_GetOutOfJailFree;
        }
        assert(false);
        return Card::Chance_GetOutOfJailFree;
    }

    class Deck
    {
    public:
        explicit Deck() {
        }

        explicit Deck(DeckType type)
        {
            if (type == DeckType::Chance) {
                cards = ChanceCards;
            }
            else {
                cards = CommunityChestCards;
            }
        }
        Card draw()
        {
            auto const card = cards.front();
            cards.erase(cards.begin());
            cards.push_back(card);
            return card;
        }

        void stack_deck(DeckContainer const& sortedCardsToPutOnTop) {
            auto it = begin(cards);
            for (auto desiredCard : sortedCardsToPutOnTop) {
                auto desiredCardIt = std::find(it, cards.end(), desiredCard);
                iter_swap(it++, desiredCardIt);
            }
        }

        void add_card(Card card) {
            cards.push_back(card);
        }

        void remove_card(Card card) {
            auto const removedCardIt = std::find(cards.begin(), cards.end(), card);
            if (removedCardIt != cards.end())
                cards.erase(removedCardIt);
        }

        void shuffle(std::mt19937& rng)
        {
            std::shuffle(cards.begin(), cards.end(), rng);
        }

        bool operator==(Deck const& rhs) const {
            return cards == rhs.cards;
        }
        bool operator!=(Deck const& rhs) const {
            return !operator==(rhs);
        }
    private:
        DeckContainer cards;
    };
}