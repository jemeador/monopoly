#pragma once

#include <algorithm>
#include <functional>
#include <random>
#include <string>
#include <vector>

namespace monopoly
{
	class Game;

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

	static auto const ChanceCards = std::vector<Card>{
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
	static auto const CommunityChestCards = std::vector<Card> {
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
		std::string title;
		std::string description;
		std::function<void(Game& game)> effect;
	};


	inline bool card_is_get_out_of_jail_free (Card card) {
		return (
			card == Card::Chance_GetOutOfJailFree ||
			card == Card::CommunityChest_GetOutOfJailFree
			);
	}

	class Deck
	{
	public:
		enum class Type
		{
			Chance,
			CommunityChest,
		};
		explicit Deck(Type type)
		{
			if (type == Deck::Type::Chance) {
				cards = ChanceCards;
			} 
			else {
				cards = CommunityChestCards;
			}
			top = 0;
		}
		Card draw()
		{
			auto const card = cards[top++];

			if (card == Card::Chance_GetOutOfJailFree ||
				card == Card::CommunityChest_GetOutOfJailFree)
				hasGetOutOfJailFree = false;

			return card;
		}
		void return_get_out_of_jail_free() {
			hasGetOutOfJailFree = true;
		}

		void shuffle(std::mt19937 &rng)
		{
			auto begin = cards.begin();
			auto const end = cards.end();
			// We can exclude the get out of jail card without changing deck size by putting it at the top and incrementing the top
			if (!hasGetOutOfJailFree) {
				auto removedCardIt = std::find_if(begin, end, card_is_get_out_of_jail_free);
				iter_swap(removedCardIt, begin);
				++begin;
			}
			std::shuffle(begin, end, rng);
			top = begin - cards.begin ();
		}
	private:
		std::vector<Card> cards;
		bool hasGetOutOfJailFree = true;
		int top = 0;
	};
}