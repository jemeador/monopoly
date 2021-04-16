#pragma once

#include"Board.h"

#include<set>
#include<string>

namespace monopoly
{
    class Player
    {
    public:
        static int const None = -1;
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

    inline std::string player_name(int playerIndex) {
        return "Player " + std::to_string(playerIndex + 1);
    }

}
