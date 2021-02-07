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
	};

	struct CardData
	{
		std::string title;
		std::string description;
		std::function<void(Game& game)> effect;
	};

	class Deck
	{
	public:
		enum class Type
		{
			Chance,
			CommunityChest,
		};
		void load(Type type)
		{
			cards.clear();
			// load
			top = 0;
		}
		Card draw()
		{
			return cards[top++];
		}
		void shuffle()
		{
			std::random_device rd;
			std::mt19937 g(rd ());
			std::shuffle(cards.begin(), cards.end(), g);
			top = 0;
		}
	private:
		std::vector<Card> cards;
		int top = 0;
	};
}