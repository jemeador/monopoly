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

	inline char const *to_string(Property property) {
		switch (property) {
		case Property::Brown_1: return "Brown_1";
		case Property::Brown_2: return "Brown_2";
		case Property::LightBlue_1: return "LightBlue_1";
		case Property::LightBlue_2: return "LightBlue_2";
		case Property::LightBlue_3: return "LightBlue_3";
		case Property::Magenta_1: return "Magenta_1";
		case Property::Magenta_2: return "Magenta_2";
		case Property::Magenta_3: return "Magenta_3";
		case Property::Orange_1: return "Orange_1";
		case Property::Orange_2: return "Orange_2";
		case Property::Orange_3: return "Orange_3";
		case Property::Red_1: return "Red_1";
		case Property::Red_2: return "Red_2";
		case Property::Red_3: return "Red_3";
		case Property::Yellow_1: return "Yellow_1";
		case Property::Yellow_2: return "Yellow_2";
		case Property::Yellow_3: return "Yellow_3";
		case Property::Green_1: return "Green_1";
		case Property::Green_2: return "Green_2";
		case Property::Green_3: return "Green_3";
		case Property::Blue_1: return "Blue_1";
		case Property::Blue_2: return "Blue_2";
		case Property::Utility_1: return "Utility_1";
		case Property::Utility_2: return "Utility_2";
		case Property::Railroad_1: return "Railroad_1";
		case Property::Railroad_2: return "Railroad_2";
		case Property::Railroad_3: return "Railroad_3";
		case Property::Railroad_4: return "Railroad_4";
		}
		return "N/A";
	}

	inline int face_value_of_property(Property p) {
		switch (p) {
		case Property::Brown_1:
		case Property::Brown_2:
			return 60;
		case Property::LightBlue_1:
		case Property::LightBlue_2:
			return 100;
		case Property::LightBlue_3:
			return 120;
		case Property::Magenta_1:
		case Property::Magenta_2:
			return 140;
		case Property::Magenta_3:
			return 160;
		case Property::Orange_1:
		case Property::Orange_2:
			return 180;
		case Property::Orange_3:
			return 200;
		case Property::Red_1:
		case Property::Red_2:
			return 220;
		case Property::Red_3:
			return 240;
		case Property::Yellow_1:
		case Property::Yellow_2:
			return 260;
		case Property::Yellow_3:
			return 280;
		case Property::Green_1:
		case Property::Green_2:
			return 300;
		case Property::Green_3:
			return 320;
		case Property::Blue_1:
			return 350;
		case Property::Blue_2:
			return 400;
		case Property::Utility_1:
		case Property::Utility_2:
			return 150;
		case Property::Railroad_1:
		case Property::Railroad_2:
		case Property::Railroad_3:
		case Property::Railroad_4:
			return 200;
		}
		return 0;
	}

	inline int mortgage_value_of_property(Property p) {
		return face_value_of_property (p) / 2;
	}

	inline int building_value(Property p) {
		switch (p) {
		case Property::Brown_1:
		case Property::Brown_2:
		case Property::LightBlue_1:
		case Property::LightBlue_2:
		case Property::LightBlue_3:
			return 50;
		case Property::Magenta_1:
		case Property::Magenta_2:
		case Property::Magenta_3:
		case Property::Orange_1:
		case Property::Orange_2:
		case Property::Orange_3:
			return 100;
		case Property::Red_1:
		case Property::Red_2:
		case Property::Red_3:
		case Property::Yellow_1:
		case Property::Yellow_2: 
		case Property::Yellow_3:
			return 150;
		case Property::Green_1:
		case Property::Green_2:
		case Property::Green_3:
		case Property::Blue_1:
		case Property::Blue_2:
			return 200;
		case Property::Utility_1:
		case Property::Utility_2:
		case Property::Railroad_1:
		case Property::Railroad_2:
		case Property::Railroad_3:
		case Property::Railroad_4:
			break;
		}
		return 0;
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

	inline bool space_is_property (Space s) {
		return space_to_property (s) != Property::Invalid;
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
