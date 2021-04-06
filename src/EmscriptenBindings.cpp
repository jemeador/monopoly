#if __EMSCRIPTEN__
#include <emscripten/bind.h>
using namespace emscripten;

#include "Board.h"
using namespace monopoly;


EMSCRIPTEN_BINDINGS(Monopoly) {
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
    function("price_of_property", &price_of_property);
    function("property_group", &property_group);
    function("property_is_in_group", &property_is_in_group);
}

#endif