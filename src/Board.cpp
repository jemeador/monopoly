#include "Board.h"
using namespace monopoly;

#include <cmath>
#include <vector>

namespace {
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

    enum class DeedField : int {
        Price = 0, PricePerHouse, Rent, Rent1, Rent2, Rent3, Rent4, RentHotel, Mortgage,
    };
    static_assert (HotelLevel == static_cast<int> (DeedField::RentHotel) - static_cast<int> (DeedField::Rent));
    static const int tableColumns = 9;

    inline int real_estate_table_lookup(Property p, DeedField f) {
        int const row = static_cast<int> (p);
        int const col = static_cast<int> (f);
        return real_estate_table[row * tableColumns + col];
    }

}

int monopoly::price_of_property(Property p) {
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
int monopoly::price_per_house_on_property(Property p) {
    return real_estate_table_lookup(p, DeedField::PricePerHouse);
}
int monopoly::sell_price_per_house_on_property(Property p) {
    return price_per_house_on_property(p) / 2;
}
int monopoly::rent_price_of_real_estate(Property p) {
    return real_estate_table_lookup(p, DeedField::Rent);
}
int monopoly::rent_price_of_improved_real_estate(Property p, int buildingLevel) {
    return real_estate_table_lookup(p, static_cast<DeedField> (static_cast<int> (DeedField::Rent) + buildingLevel));
}
int monopoly::rent_price_of_railroad(int ownedRailroads) {
    return 25 * pow(2, ownedRailroads - 1);
}
int monopoly::rent_price_of_utility(int ownedUtilities, std::pair<int, int> roll) {
    auto const sum = roll.first + roll.second;
    if (ownedUtilities == 1)
        return 4 * sum;
    else
        return 10 * sum;
}
int monopoly::mortgage_value_of_property(Property p) {
    return real_estate_table_lookup(p, DeedField::Mortgage);
}
int monopoly::unmortgage_price_of_property(Property p) {
    return real_estate_table_lookup(p, DeedField::Mortgage) * (1 + MortgageInterestRate);
}

