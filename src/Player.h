#pragma once

#include"Board.h"

#include<set>

namespace monopoly
{
class Player
{
public:
	static int const p1 = 0;
	static int const p2 = 1;
	static int const p3 = 2;
	static int const p4 = 3;

	bool eliminated = false;
	int funds = 1500;
	Space position = Space::Go;
	std::set<Property> deeds;
	int turnsRemainingInJail = 0;
	std::set<DeckType> getOutOfJailFreeCards;

	inline bool operator==(Player const& rhs) const {
		return funds == rhs.funds &&
			position == rhs.position &&
			deeds == rhs.deeds &&
			turnsRemainingInJail == rhs.turnsRemainingInJail &&
			getOutOfJailFreeCards == rhs.getOutOfJailFreeCards;
	}
	inline bool operator!=(Player const& rhs) const {
		return !operator==(rhs);
	}
};

inline char const* player_name(int playerIndex) {
	switch (playerIndex) {
	case Player::p1: return "Player 1";
	case Player::p2: return "Player 2";
	case Player::p3: return "Player 3";
	case Player::p4: return "Player 4";
	}
	return "";
}

}
