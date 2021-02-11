#include "Test.h"
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