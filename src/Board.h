#pragma once
#include "Cards.h"

#include <cassert>
#include <set>
#include <string>
#include <vector>

namespace monopoly
{
	static constexpr int NumberOfSpaces = 40;
	static constexpr int MaxJailTurns = 3;
	static constexpr int BailCost = 50;
	static constexpr int GoSalary = 200;

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

	// Calculate the proprety ID, used to populate enum
	inline int constexpr PId(PropertyGroup group, int indexInGroup)
	{
		assert(indexInGroup < 4);
		// Max bits to encode index is 2
		return (static_cast<int> (group) << 2) + indexInGroup;
	}

	enum class Property
	{
		Brown_1 = PId(PropertyGroup::Brown, 0),
		Brown_2 = PId(PropertyGroup::Brown, 1),

		LightBlue_1 = PId(PropertyGroup::LightBlue, 0),
		LightBlue_2 = PId(PropertyGroup::LightBlue, 1),
		LightBlue_3 = PId(PropertyGroup::LightBlue, 2),

		Magenta_1 = PId(PropertyGroup::Magenta, 0),
		Magenta_2 = PId(PropertyGroup::Magenta, 1),
		Magenta_3 = PId(PropertyGroup::Magenta, 2),

		Orange_1 = PId(PropertyGroup::Orange, 0),
		Orange_2 = PId(PropertyGroup::Orange, 1),
		Orange_3 = PId(PropertyGroup::Orange, 2),

		Red_1 = PId(PropertyGroup::Red, 0),
		Red_2 = PId(PropertyGroup::Red, 1),
		Red_3 = PId(PropertyGroup::Red, 2),

		Yellow_1 = PId(PropertyGroup::Yellow, 0),
		Yellow_2 = PId(PropertyGroup::Yellow, 1),
		Yellow_3 = PId(PropertyGroup::Yellow, 2),

		Green_1 = PId(PropertyGroup::Green, 0),
		Green_2 = PId(PropertyGroup::Green, 1),
		Green_3 = PId(PropertyGroup::Green, 2),

		Blue_1 = PId(PropertyGroup::Blue, 0),
		Blue_2 = PId(PropertyGroup::Blue, 1),

		Utility_1 = PId(PropertyGroup::Utility, 0),
		Utility_2 = PId(PropertyGroup::Utility, 1),

		Railroad_1 = PId(PropertyGroup::Railroad, 0),
		Railroad_2 = PId(PropertyGroup::Railroad, 1),
		Railroad_3 = PId(PropertyGroup::Railroad, 2),
		Railroad_4 = PId(PropertyGroup::Railroad, 3),

		Invalid = -1,
	};

	inline PropertyGroup property_group(Property p) {
		return static_cast<PropertyGroup> ((static_cast<int> (p) >> 2));
	}
	inline bool property_in_group(Property p, PropertyGroup g) {
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
