#if __EMSCRIPTEN__
#include <emscripten/bind.h>
using namespace emscripten;

#include "Board.h"
using namespace monopoly;

EMSCRIPTEN_BINDINGS(Monopoly) {
    // Board.h
    constant ("NumberOfSpaces", NumberOfSpaces);
    constant ("MaxJailTurns", MaxJailTurns);
    constant ("HotelLevel", HotelLevel);
    constant ("BailCost", BailCost);
    constant ("GoSalary", GoSalary);
    constant ("MortgageInterestRate", MortgageInterestRate);
    constant ("DeedTable", DeedTable);
    constant ("DeedTableColumns", DeedTableColumns);

    enum_<DeedField>("DeedField")
        .value("Price", DeedField::Price)
        .value("PricePerHouse", DeedField::PricePerHouse)
        .value("Rent", DeedField::Rent)
        .value("Rent1", DeedField::Rent1)
        .value("Rent2", DeedField::Rent2)
        .value("Rent3", DeedField::Rent3)
        .value("Rent4", DeedField::Rent4)
        .value("RentHotel", DeedField::RentHotel)
        .value("Mortgage", DeedField::Mortgage)
    ;
    enum_<PropertyGroup>("PropertyGroup")
        .value("Brown", PropertyGroup::Brown)
        .value("LightBlue", PropertyGroup::LightBlue)
        .value("Magenta", PropertyGroup::Magenta)
        .value("Orange", PropertyGroup::Orange)
        .value("Red", PropertyGroup::Red)
        .value("Yellow", PropertyGroup::Yellow)
        .value("Green", PropertyGroup::Green)
        .value("Blue", PropertyGroup::Blue)
        .value("Utility", PropertyGroup::Utility)
        .value("Railroad", PropertyGroup::Railroad)
        ;
    enum_<Property>("Property")
        .value("Brown_1", Property::Brown_1)
        .value("Brown_2", Property::Brown_2)
        .value("LightBlue_1", Property::LightBlue_1)
        .value("LightBlue_2", Property::LightBlue_2)
        .value("LightBlue_3", Property::LightBlue_3)
        .value("Magenta_1", Property::Magenta_1)
        .value("Magenta_2", Property::Magenta_2)
        .value("Magenta_3", Property::Magenta_3)
        .value("Orange_1", Property::Orange_1)
        .value("Orange_2", Property::Orange_2)
        .value("Orange_3", Property::Orange_3)
        .value("Red_1", Property::Red_1)
        .value("Red_2", Property::Red_2)
        .value("Red_3", Property::Red_3)
        .value("Yellow_1", Property::Yellow_1)
        .value("Yellow_2", Property::Yellow_2)
        .value("Yellow_3", Property::Yellow_3)
        .value("Green_1", Property::Green_1)
        .value("Green_2", Property::Green_2)
        .value("Green_3", Property::Green_3)
        .value("Blue_1", Property::Blue_1)
        .value("Blue_2", Property::Blue_2)
        .value("Utility_1", Property::Utility_1)
        .value("Utility_2", Property::Utility_2)
        .value("Railroad_1", Property::Railroad_1)
        .value("Railroad_2", Property::Railroad_2)
        .value("Railroad_3", Property::Railroad_3)
        .value("Railroad_4", Property::Railroad_4)
        .value("Invalid ", Property::Invalid)
        ;
    enum_<Space>("Space")
        .value("Go", Space::Go)
        .value("Brown_1", Space::Brown_1)
        .value("CommunityChest_1", Space::CommunityChest_1)
        .value("Brown_2", Space::Brown_2)
        .value("IncomeTax", Space::IncomeTax)
        .value("Railroad_1", Space::Railroad_1)
        .value("LightBlue_1", Space::LightBlue_1)
        .value("Chance_1", Space::Chance_1)
        .value("LightBlue_2", Space::LightBlue_2)
        .value("LightBlue_3", Space::LightBlue_3)

        .value("Jail", Space::Jail)
        .value("Magenta_1", Space::Magenta_1)
        .value("Utility_1", Space::Utility_1)
        .value("Magenta_2", Space::Magenta_2)
        .value("Magenta_3", Space::Magenta_3)
        .value("Railroad_2", Space::Railroad_2)
        .value("Orange_1", Space::Orange_1)
        .value("CommunityChest_2", Space::CommunityChest_2)
        .value("Orange_2", Space::Orange_2)
        .value("Orange_3", Space::Orange_3)

        .value("FreeParking", Space::FreeParking)
        .value("Red_1", Space::Red_1)
        .value("Chance_2", Space::Chance_2)
        .value("Red_2", Space::Red_2)
        .value("Red_3", Space::Red_3)
        .value("Railroad_3", Space::Railroad_3)
        .value("Yellow_1", Space::Yellow_1)
        .value("Yellow_2", Space::Yellow_2)
        .value("Utility_2", Space::Utility_2)
        .value("Yellow_3", Space::Yellow_3)

        .value("GoToJail", Space::GoToJail)
        .value("Green_1", Space::Green_1)
        .value("Green_2", Space::Green_2)
        .value("CommunityChest_3", Space::CommunityChest_3)
        .value("Green_3", Space::Green_3)
        .value("Railroad_4", Space::Railroad_4)
        .value("Chance_3", Space::Chance_3)
        .value("Blue_1", Space::Blue_1)
        .value("LuxuryTax", Space::LuxuryTax)
        .value("Blue_2", Space::Blue_2)
    ;

    function("price_of_property", &price_of_property);
    function("property_group", &property_group);
    function("properties_in_group", &properties_in_group);
    function("property_is_in_group", &property_is_in_group);
    function("property_is_in_group", &property_is_in_group);
    function("all_properties", &all_properties);
    function("real_estate_table_lookup", &real_estate_table_lookup);
    function("price_of_property", price_of_property);
    function("price_per_house_on_property", price_per_house_on_property);
    function("sell_price_per_house_on_property", sell_price_per_house_on_property);
    function("rent_price_of_real_estate", rent_price_of_real_estate);
    function("rent_price_of_improved_real_estate", rent_price_of_improved_real_estate);
    function("rent_price_of_railroad", rent_price_of_railroad);
    function("rent_price_of_utility", rent_price_of_utility);
    function("mortgage_value_of_property", mortgage_value_of_property);
    function("unmortgage_price_of_property", unmortgage_price_of_property);
    function("space_to_property", space_to_property);
    function("property_to_space", property_to_space);
    function("space_is_property", space_is_property);
    function("space_to_index", space_to_index);
    function("index_to_space", index_to_space);
    function("advancing_will_pass_go", advancing_will_pass_go);
    function("distance", distance);
    function("add_distance", add_distance);
    function("nearest_space", nearest_space);
}

#endif

