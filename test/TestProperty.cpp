#include "Test.h"
#include "Strings.h"
using namespace monopoly;

#include "catch2/catch.hpp"

SCENARIO("Whenever you land on an unowned property you may buy that property at its printed price. If you don't buy it, it's auctioned.", "[property]") {
	Test test;

    GIVEN ("The player has sufficient funds to buy Brown_2") {
		test.set_player_funds(Player::p1, price_of_property(Property::Brown_2));

		WHEN("the player lands on Brown_2") {
			test.roll(1, 2);

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
				}
			}
		}
    }
    GIVEN ("The player has insufficient funds to buy Brown_2") {
		test.set_player_funds(Player::p1, price_of_property(Property::Brown_2) - 1);

		WHEN("the player lands on Brown_2") {
			test.roll(1, 2);

			THEN("the property is auctioned") {
				test.require_does_not_have_deed(Player::p1, Property::Brown_2);
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

			WHEN("player 1 lands on " + to_string (property)) {
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

			WHEN("player 1 lands on " + to_string (property)) {
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
    GIVEN ("Player 2 owns Reading Railroad and B. & O. Railroad") {
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
    GIVEN ("Player 2 owns both utilities") {
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
    GIVEN ("Player 1 owns St. Charles Place and starts on Reading Railroad") {
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

	GIVEN("Player 1 owns all of the " + to_string (p1Group) + " properties and player 2 owns all the " + to_string (p2Group) + " properties") {
		test.give_deeds(Player::p1, { properties_in_group(p1Group) });
		test.give_deeds(Player::p2, { properties_in_group(p2Group) });
        test.set_player_funds(Player::p1, startingFunds);
        test.set_player_funds(Player::p2, startingFunds);

		WHEN("player 1 mortgages " + to_string(p1Property)) {
			test.set_active_player(Player::p1);
			test.mortgage_property(p1Property);

			THEN("player 1 gets the mortgage value of the property from the bank") {
				test.require_funds(Player::p1, startingFunds + mortgage_value_of_property (p1Property));
				test.require_is_mortgaged(p1Property, true);
			}
		}
		WHEN("player 1 tries to mortgage " + to_string(p2Property)) {
			test.set_active_player(Player::p1);
			test.mortgage_property(p2Property);

			THEN("nothing happens") {
				test.require_funds(Player::p1, startingFunds);
				test.require_is_mortgaged(p2Property, false);
			}
		}

		AND_GIVEN(to_string (p1PropertyWithImprovements) + " has 1 house") {
			test.set_buildings({ {p1PropertyWithImprovements, 1} });

            WHEN("player 1 tries to mortgage " + to_string(p1Property)) {
                test.set_active_player(Player::p1);
                test.mortgage_property(p1Property);

                THEN("nothing happens") {
                    test.require_funds(Player::p1, startingFunds);
                    test.require_is_mortgaged(p2Property, false);
                }
            }
		}
	}
	GIVEN("Player 1 owns all of the " + to_string (p1Group) + " properties, but they are mortgaged") {
		test.give_deeds(Player::p1, { properties_in_group(p1Group) });
		test.mortgage_properties(properties_in_group(p1Group));
        test.set_player_funds(Player::p1, startingFunds);

		WHEN("player 1 tries to mortgage an already mortgaged " + to_string(p1Property)) {
			test.mortgage_property(p1Property);

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

	GIVEN("Player 1 owns all of the " + to_string (p1Group) + " properties, but they are mortgaged") {
		test.give_deeds(Player::p1, { properties_in_group(p1Group) });
		test.mortgage_properties(properties_in_group(p1Group));
        test.set_player_funds(Player::p1, startingFunds);

        auto const p1Property = Property::Orange_2;
		WHEN("player 1 unmortgages " + to_string(p1Property)) {
			test.unmortgage_property(p1Property);

			THEN("player 1 pays the mortgage value of the property, plus interest, to the bank") {
				test.require_funds(Player::p1, startingFunds - mortgage_value_of_property (p1Property) * (1 + MortgageInterestRate));
				test.require_is_mortgaged(p1Property, false);
			}
		}
	}
	GIVEN("Player 1 owns all of the " + to_string (p1Group) + " properties") {
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
