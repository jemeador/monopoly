#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

SCENARIO("Whenever you land on an unowned property you may buy that property at its printed price. If you don't buy it, it's auctioned.", "[property]") {
	TestInterface interface;
	Game game (&interface);

    GIVEN ("The player has sufficient funds to buy Brown_2") {
		GameState state;
		state.force_funds(Player::p1, 60);
		REQUIRE(state.get_player (Player::p1).funds >= list_price_of_property(Property::Brown_2));

		game.set_state(state);

		WHEN("the player lands on Brown_2") {
			interface.roll_loaded_dice(Player::p1, { 1, 2 });
			game.wait_for_processing();

			AND_WHEN("the choice to buy is confirmed") {
				interface.buy_property(Player::p1);
				game.wait_for_processing();

				THEN("the player receives the deed for the property") {
					auto d = game.get_state ().get_player(Player::p1).deeds;
					REQUIRE(std::any_of(d.begin(), d.end(),
						[](Property p) { return p == Property::Brown_2; }));
				}
			}
			AND_WHEN("the choice to buy is rejected") {
				interface.auction_property(Player::p1);
				game.wait_for_processing();

				THEN("the property is auctioned") {
					auto d = game.get_state ().get_player(Player::p1).deeds;
					/// \todo test that an auction happens?
					REQUIRE(std::none_of(d.begin(), d.end(),
						[](Property p) { return p == Property::Brown_2; }));
				}
			}
		}
    }
    GIVEN ("The player has insufficient funds to buy Brown_2") {
		GameState state;
		state.force_funds(Player::p1, 45);
		REQUIRE(state.get_player (Player::p1).funds < list_price_of_property(Property::Brown_2));

		game.set_state(state);

		WHEN("the player lands on Brown_2") {
			interface.roll_loaded_dice(Player::p1, { 1, 2 });
			game.wait_for_processing();

			THEN("the property is auctioned") {
				/// \todo test that an auction happens?
				auto const& d = game.get_state ().get_player(Player::p1).deeds;
				REQUIRE(std::none_of(d.begin(), d.end(),
					[](Property p) { return p == Property::Brown_2; }));
			}
		}
    }
}