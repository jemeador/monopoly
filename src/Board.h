#pragma once
#include "Cards.h"

#include <array>
#include <cassert>
#include <set>
#include <string>
#include <vector>

namespace monopoly
{
	static constexpr int NumberOfSpaces = 40;
	static constexpr int MaxJailTurns = 3;
	static constexpr int HotelLevel = 5;
	static constexpr int BailCost = 50;
	static constexpr int GoSalary = 200;
	static constexpr double MortgageInterestRate = 0.10;

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

	inline std::set<Property> const all_properties () {
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

	int price_of_property(Property p);
	int price_per_house_on_property(Property p);
	int rent_price_of_real_estate(Property p);
	int rent_price_of_improved_real_estate(Property p, int buildingLevel);
	int rent_price_of_railroad(int ownedRailroads);
	int rent_price_of_utility(int ownedUtilities, std::pair<int, int> roll);
	int mortgage_value_of_property(Property p);
	int unmortgage_price_of_property(Property p);

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

	inline Property space_to_property (Space s) {
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
		}
		return Property::Invalid;
	}

	inline Space property_to_space (Property p) {
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
		}
		return Space::Go;
	}


	inline bool space_is_property(Space s) {
		return space_to_property (s) != Property::Invalid;
	}

	inline int space_to_index(Space s) {
		return static_cast<int> (s);
	}

	inline Space index_to_space(int i) {
		return static_cast<Space> (i % NumberOfSpaces);
	}

	inline bool advancing_will_pass_go(Space s, int dist) {
		return (space_to_index (s) + dist) >= 40;
	}

	inline int distance(Space from, Space to) {
		auto d = static_cast<int> (to) - static_cast<int> (from);
		if (d < 0)
			d += NumberOfSpaces;
		return d;
	}

	inline Space add_distance(Space from, int dist) {
		return index_to_space (space_to_index (from) + dist);
	}

	inline Space nearest_space (Space pos, std::vector<Space> const &spaces) {
		std::vector<int> distances;
		std::transform(begin(spaces), end(spaces), std::back_inserter(distances), [pos](Space space) { return distance (pos, space);});
		auto const distIt = std::min_element(begin(distances), end(distances));
		auto const spaceIt = begin(spaces) + (distIt - begin(distances));
		return *spaceIt;
	}

	inline void clamp_die_value(int &val) {
		if (val < 1)
			val = 1;
		else if (val > 6)
			val = 6;
	}

	inline void assert_valid_die_value(int val) {
		assert(1 <= val && val <= 6);
	}
}
