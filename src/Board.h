#pragma once
#include "Cards.h"

#include <array>
#include <cassert>
#include <cmath>
#include <iterator>
#include <set>
#include <string>
#include <vector>

namespace monopoly
{
    static std::vector<int> const real_estate_table = {
        //  PRC,  BLD,  RNT, RNT1, RNT2, RNT3, RNT4, RNTH,  MTG,
             60,   50,    2,   10,   30,   90,  160,  250,   30, // Brown 1
             60,   50,    4,   20,   60,  180,  320,  450,   30, // Brown 2
            100,   50,    6,   30,   90,  270,  400,  550,   50, // Light Blue 1
            100,   50,    6,   30,   90,  270,  400,  550,   50, // Light Blue 2
            120,   50,    8,   40,  100,  300,  450,  600,   60, // Light Blue 3
            140,  100,   10,   50,  150,  450,  625,  750,   70, // Magenta 1
            140,  100,   10,   50,  150,  450,  625,  750,   70, // Magenta 2
            160,  100,   12,   60,  180,  500,  700,  900,   80, // Magenta 3
            180,  100,   14,   70,  200,  550,  750,  950,   90, // Orange 1
            180,  100,   14,   70,  200,  550,  750,  950,   90, // Orange 2
            200,  100,   16,   80,  220,  600,  800, 1000,  100, // Orange 3
            220,  150,   18,   90,  250,  700,  875, 1050,  110, // Red 1
            220,  150,   18,   90,  250,  700,  875, 1050,  110, // Red 2
            240,  150,   20,  100,  300,  750,  925, 1100,  120, // Red 3
            260,  150,   22,  110,  330,  800,  975, 1150,  130, // Yellow 1
            260,  150,   22,  110,  330,  800,  975, 1150,  130, // Yellow 2
            280,  150,   24,  120,  360,  850, 1025, 1200,  140, // Yellow 3
            300,  200,   26,  130,  390,  900, 1100, 1275,  150, // Green 1
            300,  200,   26,  130,  390,  900, 1100, 1275,  150, // Green 2
            320,  200,   28,  150,  450, 1000, 1200, 1400,  160, // Green 3
            350,  200,   35,  175,  500, 1100, 1300, 1500,  175, // Blue 1
            400,  200,   50,  200,  600, 1400, 1700, 2000,  200, // Blue 2
            200,    0,    0,    0,    0,    0,    0,    0,  100, // Railroad 1
            200,    0,    0,    0,    0,    0,    0,    0,  100, // Railroad 2
            200,    0,    0,    0,    0,    0,    0,    0,  100, // Railroad 3
            200,    0,    0,    0,    0,    0,    0,    0,  100, // Railroad 4
            150,    0,    0,    0,    0,    0,    0,    0,  75, // Utility 1
            150,    0,    0,    0,    0,    0,    0,    0,  75, // Utility 2
    };

    static constexpr int NumberOfSpaces = 40;
    static constexpr int MaxJailTurns = 3;
    static constexpr int HotelLevel = 5;
    static constexpr int BailCost = 50;
    static constexpr int GoSalary = 200;
    static constexpr double MortgageInterestRate = 0.10;

    enum class DeedField : int {
        Price = 0, PricePerHouse, Rent, Rent1, Rent2, Rent3, Rent4, RentHotel, Mortgage,
    };
    static_assert (HotelLevel == static_cast<int> (DeedField::RentHotel) - static_cast<int> (DeedField::Rent));
    static const int tableColumns = 9;

    enum class PropertyGroup
    {
        Brown = 0,
        LightBlue,
        Magenta,
        Orange,
        Red,
        Yellow,
        Green,
        Blue,
        Utility,
        Railroad,
    };

    enum class Property
    {
        Brown_1 = 0,
        Brown_2,
        LightBlue_1,
        LightBlue_2,
        LightBlue_3,
        Magenta_1,
        Magenta_2,
        Magenta_3,
        Orange_1,
        Orange_2,
        Orange_3,
        Red_1,
        Red_2,
        Red_3,
        Yellow_1,
        Yellow_2,
        Yellow_3,
        Green_1,
        Green_2,
        Green_3,
        Blue_1,
        Blue_2,
        Utility_1,
        Utility_2,
        Railroad_1,
        Railroad_2,
        Railroad_3,
        Railroad_4,
        Invalid = -1,
    };

    inline PropertyGroup property_group(Property p) {
        switch (p) {
        case Property::Brown_1:
        case Property::Brown_2:
            return PropertyGroup::Brown;
        case Property::LightBlue_1:
        case Property::LightBlue_2:
        case Property::LightBlue_3:
            return PropertyGroup::LightBlue;
        case Property::Magenta_1:
        case Property::Magenta_2:
        case Property::Magenta_3:
            return PropertyGroup::Magenta;
        case Property::Orange_1:
        case Property::Orange_2:
        case Property::Orange_3:
            return PropertyGroup::Orange;
        case Property::Red_1:
        case Property::Red_2:
        case Property::Red_3:
            return PropertyGroup::Red;
        case Property::Yellow_1:
        case Property::Yellow_2:
        case Property::Yellow_3:
            return PropertyGroup::Yellow;
        case Property::Green_1:
        case Property::Green_2:
        case Property::Green_3:
            return PropertyGroup::Green;
        case Property::Blue_1:
        case Property::Blue_2:
            return PropertyGroup::Blue;
        case Property::Utility_1:
        case Property::Utility_2:
            return PropertyGroup::Utility;
        case Property::Railroad_1:
        case Property::Railroad_2:
        case Property::Railroad_3:
        case Property::Railroad_4:
            return PropertyGroup::Railroad;
        case Property::Invalid:
            break;
        }
        assert(false); // unreachable
        return PropertyGroup::Railroad;
    }

    inline std::set<Property> properties_in_group(PropertyGroup group) {
        static const std::vector<std::set<Property>> propertiesByGroup = {
            {
                Property::Brown_1,
                Property::Brown_2,
            },
            {
                Property::LightBlue_1,
                Property::LightBlue_2,
                Property::LightBlue_3,
            },
            {
                Property::Magenta_1,
                Property::Magenta_2,
                Property::Magenta_3,
            },
            {
                Property::Orange_1,
                Property::Orange_2,
                Property::Orange_3,
            },
            {
                Property::Red_1,
                Property::Red_2,
                Property::Red_3,
            },
            {
                Property::Yellow_1,
                Property::Yellow_2,
                Property::Yellow_3,
            },
            {
                Property::Green_1,
                Property::Green_2,
                Property::Green_3,
            },
            {
                Property::Blue_1,
                Property::Blue_2,
            },
            {
                Property::Utility_1,
                Property::Utility_2,
            },
            {
                Property::Railroad_1,
                Property::Railroad_2,
                Property::Railroad_3,
                Property::Railroad_4,
            },
        };
        return propertiesByGroup[static_cast<int> (group)];
    }

    inline bool property_is_in_group(Property p, PropertyGroup g) {
        return property_group(p) == g;
    }

    inline std::set<Property> const all_properties() {
        return {
            Property::Brown_1,
            Property::Brown_2,
            Property::LightBlue_1,
            Property::LightBlue_2,
            Property::LightBlue_3,
            Property::Magenta_1,
            Property::Magenta_2,
            Property::Magenta_3,
            Property::Orange_1,
            Property::Orange_2,
            Property::Orange_3,
            Property::Red_1,
            Property::Red_2,
            Property::Red_3,
            Property::Yellow_1,
            Property::Yellow_2,
            Property::Yellow_3,
            Property::Green_1,
            Property::Green_2,
            Property::Green_3,
            Property::Blue_1,
            Property::Blue_2,
            Property::Utility_1,
            Property::Utility_2,
            Property::Railroad_1,
            Property::Railroad_2,
            Property::Railroad_3,
            Property::Railroad_4,
        };
    }

    inline int real_estate_table_lookup(Property p, DeedField f) {
        int const row = static_cast<int> (p);
        int const col = static_cast<int> (f);
        if (row >= all_properties().size()) {
            return 0;
        }
        if (col >= tableColumns) {
            return 0;
        }
        return real_estate_table[static_cast<size_t> (row * tableColumns + col)];
    }


    inline int price_of_property(Property p) {
        if (property_is_in_group(p, PropertyGroup::Railroad)) {
            return 200;
        }
        else if (property_is_in_group(p, PropertyGroup::Utility)) {
            return 150;
        }
        else {
            return real_estate_table_lookup(p, DeedField::Price);
        }
    }

    inline int price_per_house_on_property(Property p) {
        return real_estate_table_lookup(p, DeedField::PricePerHouse);
    }

    inline int sell_price_per_house_on_property(Property p) {
        return price_per_house_on_property(p) / 2;
    }
    inline int rent_price_of_real_estate(Property p) {
        return real_estate_table_lookup(p, DeedField::Rent);
    }
    inline int rent_price_of_improved_real_estate(Property p, int buildingLevel) {
        return real_estate_table_lookup(p, static_cast<DeedField> (static_cast<int> (DeedField::Rent) + buildingLevel));
    }
    inline int rent_price_of_railroad(int ownedRailroads) {
        return 25 * pow(2, ownedRailroads - 1);
    }
    inline int rent_price_of_utility(int ownedUtilities, std::pair<int, int> roll) {
        auto const sum = roll.first + roll.second;
        if (ownedUtilities == 1)
            return 4 * sum;
        else
            return 10 * sum;
    }
    inline int mortgage_value_of_property(Property p) {
        return real_estate_table_lookup(p, DeedField::Mortgage);
    }
    inline int unmortgage_price_of_property(Property p) {
        return real_estate_table_lookup(p, DeedField::Mortgage) * (1 + MortgageInterestRate);
    }

    enum class Space
    {
        Go = 0,
        Brown_1,
        CommunityChest_1,
        Brown_2,
        IncomeTax,
        Railroad_1,
        LightBlue_1,
        Chance_1,
        LightBlue_2,
        LightBlue_3,

        Jail,
        Magenta_1,
        Utility_1,
        Magenta_2,
        Magenta_3,
        Railroad_2,
        Orange_1,
        CommunityChest_2,
        Orange_2,
        Orange_3,

        FreeParking,
        Red_1,
        Chance_2,
        Red_2,
        Red_3,
        Railroad_3,
        Yellow_1,
        Yellow_2,
        Utility_2,
        Yellow_3,

        GoToJail,
        Green_1,
        Green_2,
        CommunityChest_3,
        Green_3,
        Railroad_4,
        Chance_3,
        Blue_1,
        LuxuryTax,
        Blue_2,
    };

    inline Property space_to_property(Space s) {
        switch (s) {
        case Space::Brown_1: return Property::Brown_1;
        case Space::Brown_2: return Property::Brown_2;
        case Space::LightBlue_1: return Property::LightBlue_1;
        case Space::LightBlue_2: return Property::LightBlue_2;
        case Space::LightBlue_3: return Property::LightBlue_3;
        case Space::Magenta_1: return Property::Magenta_1;
        case Space::Magenta_2: return Property::Magenta_2;
        case Space::Magenta_3: return Property::Magenta_3;
        case Space::Orange_1: return Property::Orange_1;
        case Space::Orange_2: return Property::Orange_2;
        case Space::Orange_3: return Property::Orange_3;
        case Space::Red_1: return Property::Red_1;
        case Space::Red_2: return Property::Red_2;
        case Space::Red_3: return Property::Red_3;
        case Space::Yellow_1: return Property::Yellow_1;
        case Space::Yellow_2: return Property::Yellow_2;
        case Space::Yellow_3: return Property::Yellow_3;
        case Space::Green_1: return Property::Green_1;
        case Space::Green_2: return Property::Green_2;
        case Space::Green_3: return Property::Green_3;
        case Space::Blue_1: return Property::Blue_1;
        case Space::Blue_2: return Property::Blue_2;
        case Space::Utility_1: return Property::Utility_1;
        case Space::Utility_2: return Property::Utility_2;
        case Space::Railroad_1: return Property::Railroad_1;
        case Space::Railroad_2: return Property::Railroad_2;
        case Space::Railroad_3: return Property::Railroad_3;
        case Space::Railroad_4: return Property::Railroad_4;
        default: break;
        }
        return Property::Invalid;
    }

    inline Space property_to_space(Property p) {
        switch (p) {
        case Property::Brown_1: return Space::Brown_1;
        case Property::Brown_2: return Space::Brown_2;
        case Property::LightBlue_1: return Space::LightBlue_1;
        case Property::LightBlue_2: return Space::LightBlue_2;
        case Property::LightBlue_3: return Space::LightBlue_3;
        case Property::Magenta_1: return Space::Magenta_1;
        case Property::Magenta_2: return Space::Magenta_2;
        case Property::Magenta_3: return Space::Magenta_3;
        case Property::Orange_1: return Space::Orange_1;
        case Property::Orange_2: return Space::Orange_2;
        case Property::Orange_3: return Space::Orange_3;
        case Property::Red_1: return Space::Red_1;
        case Property::Red_2: return Space::Red_2;
        case Property::Red_3: return Space::Red_3;
        case Property::Yellow_1: return Space::Yellow_1;
        case Property::Yellow_2: return Space::Yellow_2;
        case Property::Yellow_3: return Space::Yellow_3;
        case Property::Green_1: return Space::Green_1;
        case Property::Green_2: return Space::Green_2;
        case Property::Green_3: return Space::Green_3;
        case Property::Blue_1: return Space::Blue_1;
        case Property::Blue_2: return Space::Blue_2;
        case Property::Utility_1: return Space::Utility_1;
        case Property::Utility_2: return Space::Utility_2;
        case Property::Railroad_1: return Space::Railroad_1;
        case Property::Railroad_2: return Space::Railroad_2;
        case Property::Railroad_3: return Space::Railroad_3;
        case Property::Railroad_4: return Space::Railroad_4;
        case Property::Invalid: break;
        }
        return Space::Go;
    }


    inline bool space_is_property(Space s) {
        return space_to_property(s) != Property::Invalid;
    }

    inline int space_to_index(Space s) {
        return static_cast<int> (s);
    }

    inline Space index_to_space(int i) {
        return static_cast<Space> (i % NumberOfSpaces);
    }

    inline bool advancing_will_pass_go(Space s, int dist) {
        return (space_to_index(s) + dist) >= 40;
    }

    inline int distance(Space from, Space to) {
        auto d = static_cast<int> (to) - static_cast<int> (from);
        if (d < 0)
            d += NumberOfSpaces;
        return d;
    }

    inline Space add_distance(Space from, int dist) {
        return index_to_space(space_to_index(from) + dist);
    }

    inline Space nearest_space(Space pos, std::vector<Space> const& spaces) {
        std::vector<int> distances;
        std::transform(begin(spaces), end(spaces), std::back_inserter(distances), [pos](Space space) { return distance(pos, space);});
        auto const distIt = std::min_element(begin(distances), end(distances));
        auto const spaceIt = begin(spaces) + (distIt - begin(distances));
        return *spaceIt;
    }

    inline void clamp_die_value(int& val) {
        if (val < 1)
            val = 1;
        else if (val > 6)
            val = 6;
    }

    inline void assert_valid_die_value(int val) {
        assert(1 <= val && val <= 6);
    }
}
