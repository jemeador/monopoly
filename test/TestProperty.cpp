#include "Test.h"
#include "Strings.h"
using namespace monopoly;

#include "catch2/catch.hpp"

SCENARIO("Whenever you land on an unowned property you may buy that property at its printed price. If you don't buy it, it's auctioned.", "[property, auction]") {
    Test test;

    GIVEN("The player has sufficient funds to buy Brown_2") {
        test.set_player_funds(Player::p1, price_of_property(Property::Brown_2));

        WHEN("the player lands on Brown_2") {
            test.land_on_space(Player::p1, Space::Brown_2);

            AND_WHEN("the choice to buy is confirmed") {
                test.buy_property();

                THEN("the player receives the deed for the property") {
                    test.require_has_deed(Player::p1, Property::Brown_2);
                }
            }
            AND_WHEN("the choice to buy is rejected") {
                test.auction_property();

                THEN("the property is auctioned") {
                    test.require_does_not_have_deed(Player::p1, Property::Brown_2);
                    test.require_phase(TurnPhase::WaitingForBids);
                }
            }
        }
    }
    GIVEN("The player has insufficient funds to buy " + to_string (Property::Brown_2) + " but owns " + to_string (Property::Blue_2)) {
        test.set_player_funds(Player::p1, price_of_property(Property::Brown_2) - 1);
        test.give_deed(Player::p1, Property::Blue_2);

        WHEN("the player lands on Brown_2") {
            test.land_on_space(Player::p1, Space::Brown_2);

            AND_WHEN("the choice to buy is confirmed") {
                test.buy_property();

                THEN("nothing happens") {
                    test.require_does_not_have_deed(Player::p1, Property::Brown_2);
                    test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
                }
            }
            AND_WHEN("the player gets a loan to afford the property by mortgaging") {
                test.mortgage_property(Player::p1, Property::Blue_2);

                AND_WHEN("the choice to buy is confirmed") {
                    test.buy_property();

                    THEN("the player receives the deed for the property") {
                        test.require_has_deed(Player::p1, Property::Brown_2);
                    }
                }
            }
        }
    }
}

SCENARIO("When a property is auctioned, players take turns bidding in turn order until all but one player declines to bid", "[property, auction]") {
    Test test;
    int const startingFunds = 500;

    test.set_player_funds(Player::p1, startingFunds);
    test.set_player_funds(Player::p2, startingFunds);
    test.set_player_funds(Player::p3, startingFunds);
    test.set_player_funds(Player::p4, startingFunds);
    test.give_deed(Player::p2, Property::Blue_1);

    auto const property = Property::Blue_2;
    auto const space = property_to_space (Property::Blue_2);
    GIVEN("An auction starts as a result of a player landing on " + to_string(property) + " and declining to buy it") {
        test.land_on_space(Player::p1, space);
        test.auction_property();

        THEN("the highest bid becomes $0 with the property going to Player 1") {
            test.require_highest_bid(Player::p1, 0);
        }
        WHEN("player 2 bids $1") {
            test.bid(Player::p2, 1);

            THEN("the highest bid becomes $1 with the property going to Player 2") {
                test.require_highest_bid(Player::p2, 1);
            }
            AND_WHEN("player 3 bids 0") {
                test.bid(Player::p3, 0);

                THEN("the bid is ignored and the highest bid remains $1 with the property going to Player 2") {
                    test.require_highest_bid(Player::p2, 1);
                }
            }
            AND_WHEN("player 4 bids out of turn") {
                test.bid(Player::p4, 1000000);

                THEN("the bid is ignored and the highest bid remains $1 with the property going to Player 2") {
                    test.require_highest_bid(Player::p2, 1);
                }
            }
            AND_WHEN("player 3 bids $10") {
                test.bid(Player::p3, 10);

                THEN("the highest bid becomes $10 with the property going to Player 3") {
                    test.require_highest_bid(Player::p3, 10);
                }
            }
            AND_WHEN("player 3 and 4 decline to bid") {
                test.decline_bid(Player::p3);
                test.decline_bid(Player::p4);

                THEN("the highest bid remains $1 with the property going to Player 2") {
                    test.require_highest_bid(Player::p2, 1);
                }
                AND_WHEN("player 1 declines to bid") {
                    test.decline_bid(Player::p1);

                    THEN("player 2 is awarded the property") {
                        test.require_funds(Player::p2, startingFunds - 1);
                        test.require_has_deed(Player::p2, property);
                    }
                }
                AND_WHEN("there is a bit more back and forth") {
                    test.bid(Player::p1, 100);
                    test.bid(Player::p2, 200);
                    test.bid(Player::p1, 225);
                    test.bid(Player::p2, 300);
                    test.bid(Player::p1, 301);
                    test.bid(Player::p2, 400);
                    test.bid(Player::p1, 490);
                    test.bid(Player::p2, 499);

                    AND_WHEN("player 1 tries to bid more than their liquid asset value") {
                        test.bid(Player::p1, 501);

                        THEN("nothing happens, the highest bid remains 500 going to player 2") {
                            test.require_highest_bid(Player::p2, 499);
                        }
                    }
                    AND_WHEN("the bid goes over what player 1 can afford ") {
                        test.bid(Player::p1, 500);
                        test.bid(Player::p2, 501);

                        THEN("player 1 is removed from the auction, but player 2 owes a debt before he gets the proprety") {
                            test.require_funds(Player::p2, -1);
                            test.require_does_not_have_deed(Player::p2, property);
                            test.require_phase(TurnPhase::WaitingForDebtSettlement);
                        }
                        AND_WHEN("player 2 mortgages his property") {
                            test.mortgage_property(Player::p2, Property::Blue_1);

                            THEN("player 2 gets the deed") {
                                test.require_funds(Player::p2, startingFunds - 501 + mortgage_value_of_property (Property::Blue_1));
                                test.require_has_deed(Player::p2, property);
                            }
                        }
                    }
                }
            }
        }
        WHEN("player 2, 3, and 4 decline to bid") {
            test.decline_bid(Player::p2);
            test.decline_bid(Player::p3);
            test.decline_bid(Player::p4);

            THEN("player 1 is awarded the property for free") {
                test.require_funds(Player::p1, startingFunds);
                test.require_has_deed(Player::p1, property);
            }
        }
    }
}

SCENARIO("Whenever you land on an owned property you must pay rent to the ownwer of that property.", "[property, rent]") {
    Test test;
    int const startingFunds = 1500;
    test.set_player_funds(Player::p1, startingFunds);
    test.set_player_funds(Player::p2, startingFunds);

    for (auto property : { Property::Brown_2, Property::Yellow_3 }) {
        auto const space = property_to_space(property);
        auto const group = property_group(property);
        auto const propertiesInGroup = properties_in_group(group);
        GIVEN("Player 2 owns " + to_string(property)) {
            test.give_deed(Player::p2, property);

            WHEN("player 1 lands on " + to_string(property)) {
                test.land_on_space(Player::p1, space);

                THEN("player 1 pays player 2 the rent price for the property") {
                    int const rent = rent_price_of_real_estate(property);
                    test.require_funds(Player::p1, startingFunds - rent);
                    test.require_funds(Player::p2, startingFunds + rent);
                }
            }
        }
        GIVEN("Player 2 owns all properties in the " + to_string(group) + " group") {
            test.give_deeds(Player::p2, propertiesInGroup);

            WHEN("player 1 lands on " + to_string(property)) {
                test.land_on_space(Player::p1, space);

                THEN("player 1 pays player 2 double the rent price for the property") {
                    int const rent = 2 * rent_price_of_real_estate(property);
                    test.require_funds(Player::p1, startingFunds - rent);
                    test.require_funds(Player::p2, startingFunds + rent);
                }
            }
        }
    }
    GIVEN("Player 2 owns 3 houses on Brown_2 ") {
        test.give_deeds(Player::p2, { Property::Brown_1, Property::Brown_2 });
        test.set_buildings({
            {Property::Brown_1, 2},
            {Property::Brown_2, 3},
            });

        WHEN("player 1 lands on Brown_2") {
            test.land_on_space(Player::p1, Space::Brown_2);

            THEN("player 1 pays player 2 rent at the 3 house level for the property") {
                int const rent = rent_price_of_improved_real_estate(Property::Brown_2, 3);
                test.require_funds(Player::p1, startingFunds - rent);
                test.require_funds(Player::p2, startingFunds + rent);
            }
        }
    }
    GIVEN("Player 2 owns a hotel on Brown_2 ") {
        test.give_deeds(Player::p2, { Property::Brown_1, Property::Brown_2 });
        test.set_buildings({
            {Property::Brown_1, 4},
            {Property::Brown_2, 5},
            });

        WHEN("player 1 lands on Brown_2") {
            test.land_on_space(Player::p1, Space::Brown_2);

            THEN("player 1 pays player 2 rent at the hotel level for the property") {
                int const rent = rent_price_of_improved_real_estate(Property::Brown_2, 5);
                test.require_funds(Player::p1, startingFunds - rent);
                test.require_funds(Player::p2, startingFunds + rent);
            }
        }
    }
    GIVEN("Player 2 owns Reading Railroad") {
        test.give_deed(Player::p2, Property::Railroad_1);

        WHEN("player 1 lands on Reading Railroad") {
            test.land_on_space(Player::p1, Space::Railroad_1);

            THEN("player 1 pays player 2 the rent price for 2 railroads") {
                int const rent = rent_price_of_railroad(1);
                REQUIRE(rent == 25);
                test.require_funds(Player::p1, startingFunds - rent);
                test.require_funds(Player::p2, startingFunds + rent);
            }
        }
    }
    GIVEN("Player 2 owns Reading Railroad and B. & O. Railroad") {
        test.give_deeds(Player::p2, { Property::Railroad_1, Property::Railroad_3 });

        WHEN("player 1 lands on Reading Railroad") {
            test.land_on_space(Player::p1, Space::Railroad_1);

            THEN("player 1 pays player 2 the rent price for 2 railroads") {
                int const rent = rent_price_of_railroad(2);
                REQUIRE(rent == 50);
                test.require_funds(Player::p1, startingFunds - rent);
                test.require_funds(Player::p2, startingFunds + rent);
            }
        }
    }
    GIVEN("Player 2 owns both utilities") {
        test.give_deeds(Player::p2, { Property::Utility_1, Property::Utility_2 });

        WHEN("player 1 rolls a 6,6 and lands on Electric Company") {
            test.roll(6, 6);

            THEN("player 1 pays player 2 the rent price for 2 utilities; 10 * the dice roll") {
                int const rent = rent_price_of_utility(2, { 6, 6 });
                REQUIRE(rent == 120);
                test.require_funds(Player::p1, startingFunds - rent);
                test.require_funds(Player::p2, startingFunds + rent);
            }
        }
    }
    GIVEN("Player 1 owns St. Charles Place and starts on Reading Railroad") {
        test.move_player(Player::p1, Space::Railroad_1);
        test.give_deeds(Player::p1, { Property::Magenta_1 });

        WHEN("player 1 rolls a 2,4 and lands on St. Charles Place") {
            test.roll(2, 4);

            THEN("player 1 pays nothing because he owns it, and his turn ends") {
                test.require_funds(Player::p1, startingFunds);
                test.require_phase(TurnPhase::WaitingForTurnEnd);
            }
        }
    }
}

SCENARIO("Players can receive loans from the bank by mortgaging properties in an unimproved group.", "[property]") {
    Test test;
    auto const startingFunds = 1500;
    auto const p1Group = PropertyGroup::Orange;
    auto const p1Property = Property::Orange_2;
    auto const p1PropertyWithImprovements = Property::Orange_1;
    auto const p2Group = PropertyGroup::Blue;
    auto const p2Property = Property::Blue_2;

    GIVEN("Player 1 owns all of the " + to_string(p1Group) + " properties and player 2 owns all the " + to_string(p2Group) + " properties") {
        test.give_deeds(Player::p1, { properties_in_group(p1Group) });
        test.give_deeds(Player::p2, { properties_in_group(p2Group) });
        test.set_player_funds(Player::p1, startingFunds);
        test.set_player_funds(Player::p2, startingFunds);

        WHEN("player 1 mortgages " + to_string(p1Property)) {
            test.set_active_player(Player::p1);
            test.mortgage_property(Player::p1, p1Property);

            THEN("player 1 gets the mortgage value of the property from the bank") {
                test.require_funds(Player::p1, startingFunds + mortgage_value_of_property(p1Property));
                test.require_is_mortgaged(p1Property, true);
            }
        }
        WHEN("player 1 tries to mortgage " + to_string(p2Property)) {
            test.set_active_player(Player::p1);
            test.mortgage_property(Player::p1, p2Property);

            THEN("nothing happens") {
                test.require_funds(Player::p1, startingFunds);
                test.require_is_mortgaged(p2Property, false);
            }
        }

        AND_GIVEN(to_string(p1PropertyWithImprovements) + " has 1 house") {
            test.set_buildings({ {p1PropertyWithImprovements, 1} });

            WHEN("player 1 tries to mortgage " + to_string(p1Property)) {
                test.set_active_player(Player::p1);
                test.mortgage_property(Player::p1, p1Property);

                THEN("nothing happens") {
                    test.require_funds(Player::p1, startingFunds);
                    test.require_is_mortgaged(p2Property, false);
                }
            }
        }
    }
    GIVEN("Player 1 owns all of the " + to_string(p1Group) + " properties, but they are mortgaged") {
        test.give_deeds(Player::p1, { properties_in_group(p1Group) });
        for (auto property : properties_in_group(p1Group)) {
            test.mortgage_property(Player::p1, property);
        }
        test.set_player_funds(Player::p1, startingFunds);

        WHEN("player 1 tries to mortgage an already mortgaged " + to_string(p1Property)) {
            test.mortgage_property(Player::p1, p1Property);

            THEN("nothing happens") {
                test.require_funds(Player::p1, startingFunds);
                test.require_is_mortgaged(p1Property, true);
            }
        }
    }
}

SCENARIO("Players must pay off mortgaged properties before they can improve properties in the group or collect rent on the property.", "[property]") {
    Test test;

    auto const p1Group = PropertyGroup::Orange;
    auto const startingFunds = 1500;
    auto const p1Property = Property::Orange_2;

    GIVEN("Player 1 owns all of the " + to_string(p1Group) + " properties, but they are mortgaged") {
        test.give_deeds(Player::p1, { properties_in_group(p1Group) });
        for (auto proprety : properties_in_group(p1Group))
            test.mortgage_property(Player::p1, proprety);
        test.set_player_funds(Player::p1, startingFunds);

        WHEN("player 1 unmortgages " + to_string(p1Property)) {
            test.unmortgage_property(p1Property);

            THEN("player 1 pays the mortgage value of the property, plus interest, to the bank") {
                test.require_funds(Player::p1, startingFunds - mortgage_value_of_property(p1Property) * (1 + MortgageInterestRate));
                test.require_is_mortgaged(p1Property, false);
            }
        }

        AND_GIVEN("player 1 doesn't have enough money to cover the mortgage") {
            test.set_player_funds(Player::p1, 0);

            WHEN("player 1 tries to unmortgage " + to_string(p1Property)) {
                test.unmortgage_property(p1Property);

                THEN("nothing happens") {
                    test.require_funds(Player::p1, 0);
                    test.require_is_mortgaged(p1Property, true);
                }
            }
        }
    }
    GIVEN("Player 1 owns all of the " + to_string(p1Group) + " properties, and they are unmortgaged") {
        test.give_deeds(Player::p1, { properties_in_group(p1Group) });
        test.set_player_funds(Player::p1, startingFunds);

        auto const p1Property = Property::Orange_2;
        WHEN("player 1 tries to unmortgage " + to_string(p1Property)) {
            test.unmortgage_property(p1Property);

            THEN("nothing happens") {
                test.require_funds(Player::p1, startingFunds);
                test.require_is_mortgaged(p1Property, false);
            }
        }
    }
}

SCENARIO("Players may buy/sell buildings (evenly) on real estate properties if they own the entire color group and none of them are mortgaged", "[property, building]") {
    Test test;

    auto const p1Group = PropertyGroup::Orange;
    auto const startingFunds = 1500;
    test.set_player_funds(Player::p1, startingFunds);

    GIVEN("Player 1 owns all of the " + to_string(p1Group) + " properties") {
        test.give_deeds(Player::p1, { properties_in_group(p1Group) });

        auto const p1Property = Property::Orange_2;
        WHEN("player 1 tries to buy a house on " + to_string(p1Property)) {
            test.buy_building(p1Property);

            THEN("player 1 pays the price of a house for that property and a building is placed") {
                test.require_funds(Player::p1, startingFunds - price_per_house_on_property(p1Property));
                test.require_building_level(p1Property, 1);
            }
        }
        WHEN("player 1 tries to sell a house on " + to_string(p1Property) + " (without any houses)") {
            test.sell_building(p1Property);

            THEN("nothing happens") {
                test.require_funds(Player::p1, startingFunds);
                test.require_building_level(p1Property, 0);
            }
        }
        AND_GIVEN("player 1 already owns one house on " + to_string(p1Property) + " and none on the others") {
            test.set_buildings({ {p1Property, 1} });

            WHEN("player 1 tries to buy a house on " + to_string(p1Property)) {
                test.buy_building(p1Property);

                THEN("nothing happens") {
                    test.require_funds(Player::p1, startingFunds);
                    test.require_building_level(p1Property, 1);
                }
            }
            WHEN("player 1 tries to sell a house on " + to_string(p1Property)) {
                test.sell_building(p1Property);

                THEN("player 1 collects the price of a house for that property and a building is removed") {
                    test.require_funds(Player::p1, startingFunds + price_per_house_on_property(p1Property) / 2);
                    test.require_building_level(p1Property, 0);
                }
            }
        }
        AND_GIVEN("player 1 already owns 2 houses on " + to_string(p1Property) + " and 3 on the others") {
            test.set_buildings({
                {Property::Orange_1, 3},
                {p1Property, 2},
                {Property::Orange_3, 3}
                });

            WHEN("player 1 tries to buy a house on " + to_string(p1Property)) {
                test.buy_building(p1Property);

                THEN("player 1 pays the price of a house for that property and a building is placed") {
                    test.require_funds(Player::p1, startingFunds - price_per_house_on_property(p1Property));
                    test.require_building_level(p1Property, 3);
                }
            }
            WHEN("player 1 tries to sell a house on " + to_string(p1Property)) {
                test.sell_building(p1Property);

                THEN("nothing happens") {
                    test.require_funds(Player::p1, startingFunds);
                    test.require_building_level(p1Property, 2);
                }
            }
        }
        AND_GIVEN("player 1 already owns 3 hotels on " + to_string(p1Group) + " properties") {
            test.set_buildings({
                {Property::Orange_1, HotelLevel},
                {Property::Orange_2, HotelLevel},
                {Property::Orange_3, HotelLevel}
                });

            WHEN("player 1 tries to buy a house on " + to_string(p1Property)) {
                test.buy_building(p1Property);

                THEN("nothing happens") {
                    test.require_funds(Player::p1, startingFunds);
                    test.require_building_level(p1Property, 5);
                }
            }
            WHEN("player 1 tries to sell a house on " + to_string(p1Property)) {
                test.sell_building(p1Property);

                THEN("player 1 collects the price of a house for that property and a building is removed") {
                    test.require_funds(Player::p1, startingFunds + price_per_house_on_property(p1Property) / 2);
                    test.require_building_level(p1Property, 4);
                }
            }
        }
    }
    GIVEN("Player 1 owns all of the " + to_string(p1Group) + " properties, execept for " + to_string(Property::Orange_3)) {
        test.give_deeds(Player::p1, { Property::Orange_1, Property::Orange_2 });

        auto const p1Property = Property::Orange_2;
        WHEN("player 1 tries to buy a house on " + to_string(p1Property)) {
            test.buy_building(p1Property);

            THEN("nothing happens") {
                test.require_funds(Player::p1, startingFunds);
                test.require_building_level(p1Property, 0);
            }
        }
    }

    GIVEN("Player 1 owns all of the " + to_string(p1Group) + " properties, but " + to_string(Property::Orange_1) + " is mortgaged") {
        test.give_deeds(Player::p1, { properties_in_group(p1Group) });
        test.mortgage_property(Player::p1, Property::Orange_1);
        test.set_player_funds(Player::p1, startingFunds);

        auto const p1Property = Property::Orange_2;
        WHEN("player 1 tries to buy a house on " + to_string(p1Property)) {
            test.buy_building(p1Property);

            THEN("nothing happens") {
                test.require_funds(Player::p1, startingFunds);
                test.require_building_level(p1Property, 0);
            }
        }
    }
}
