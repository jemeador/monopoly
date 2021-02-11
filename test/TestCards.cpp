#include "Test.h"
using namespace monopoly;

#include "catch2/catch.hpp"

#include <cassert>

SCENARIO("A player draws a card that advances them to a specific location", "[cards]") {
	Test test;

	int const startingFunds = 1500;
	test.set_player_funds(Player::p1, startingFunds);

	GIVEN("the Community Chest deck is stacked with Advance to GO") {
		test.stack_deck(DeckType::CommunityChest, { Card::CommunityChest_AdvanceToGo });

		WHEN("player 1 lands on Community Chest") {
			test.land_on_space(Player::p1, Space::CommunityChest_1);

			THEN("player 1 draws a Community Chest card (Advance to GO), advances to GO, and collects GO salary") {
				test.require_position(Player::p1, Space::Go);
				test.require_funds(Player::p1, startingFunds + GoSalary);
			}
		}
	}
}

SCENARIO("A player draws the Go to Jail card", "[cards]") {
	Test test;

	int const startingFunds = 1500;
	test.set_player_funds(Player::p1, startingFunds);

	GIVEN("the Chance deck is stacked with Go To Jail") {
		test.stack_deck(DeckType::Chance, { Card::Chance_GoToJail });

		WHEN("player 1 rolls (2,2) and lands on Chance") {
			test.land_on_space(Player::p1, Space::Chance_1);

			THEN("player 1 goes directly to jail, does not pass go, does not collect $200") {
				test.require_position(Player::p1, Space::Jail);
				test.require_jailed(Player::p1, true);
				test.require_funds(Player::p1, startingFunds);
			}
		}
	}
}

SCENARIO("A player draws Advance to the Nearest Railroad", "[cards]") {
	Test test;

	int const startingFunds = 1500;
	test.set_player_funds(Player::p1, startingFunds);
	test.set_player_funds(Player::p2, startingFunds);

	GIVEN("The Chance deck is stacked with Advance to Nearest Railroad") {
		test.stack_deck(DeckType::Chance, { Card::Chance_AdvanceToNearestRailroad });

		WHEN("player 1 lands on Chance 1") {
			test.land_on_space(Player::p1, Space::Chance_1);

			THEN("player 1 advances to Pennsylvania Railroad") {
				test.require_position(Player::p1, Space::Railroad_2);
				test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
			}
		}
		WHEN("player 1 lands on Chance 2") {
			test.land_on_space(Player::p1, Space::Chance_2);

			THEN("player 1 advances to B. & O. Railroad") {
				test.require_position(Player::p1, Space::Railroad_3);
				test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
			}
		}
		WHEN("player 1 lands on Chance 3") {
			test.land_on_space(Player::p1, Space::Chance_3);

			THEN("player 1 advances to Reading Railroad") {
				test.require_position(Player::p1, Space::Railroad_1);
				test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
			}
		}
		WHEN("player 2 owns B. & O. Railroad") {
			test.give_deed(Player::p2, Property::Railroad_3);
			AND_WHEN("player 1 lands on Chance 3") {
				test.land_on_space(Player::p1, Space::Chance_2);

				THEN("player 1 advances to B. & 0. Railroad and pays double rent") {
					auto const modifiedRent = 2 * rent_price_of_railroad(1);
					test.require_position(Player::p1, Space::Railroad_3);
					test.require_funds(Player::p1, startingFunds - modifiedRent);
					test.require_funds(Player::p2, startingFunds + modifiedRent);
				}
			}
		}
	}
}

SCENARIO("A player draws Advance to the Nearest Utility", "[cards]") {
	Test test;

	int const startingFunds = 1500;
	test.set_player_funds(Player::p1, startingFunds);
	test.set_player_funds(Player::p2, startingFunds);

	GIVEN("The Chance deck is stacked with Advance to Nearest Railroad") {
		test.stack_deck(DeckType::Chance, { Card::Chance_AdvanceToNearestUtility });

		WHEN("player 1 lands on Chance 1") {
			test.land_on_space(Player::p1, Space::Chance_1);

			THEN("player 1 advances to Electric Company") {
				test.require_position(Player::p1, Space::Utility_1);
				test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
			}
		}
		WHEN("player 1 lands on Chance 2") {
			test.land_on_space(Player::p1, Space::Chance_2);

			THEN("player 1 advances to Water Works") {
				test.require_position(Player::p1, Space::Utility_2);
				test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
			}
		}
		WHEN("player 1 lands on Chance 3") {
			test.land_on_space(Player::p1, Space::Chance_3);

			THEN("player 1 advances to Electric Company") {
				test.require_position(Player::p1, Space::Utility_1);
				test.require_phase(TurnPhase::WaitingForBuyPropertyInput);
			}
		}
		WHEN("player 2 owns Water Works") {
			test.give_deed(Player::p2, Property::Utility_2);
			AND_WHEN("player 1 lands on Chance 3") {
				test.land_on_space(Player::p1, Space::Chance_2);

				THEN("player 1 advances to Water Works and pays double the value of a random roll") {
					auto const seededRollTimes10 = 100; // This may need to change if we mess with the seed
					// todo Expose more data or allow loaded dice rolls outside of movement to make this more robust
					test.require_position(Player::p1, Space::Utility_2);
					test.require_funds(Player::p1, startingFunds - seededRollTimes10);
					test.require_funds(Player::p2, startingFunds + seededRollTimes10);
				}
			}
		}
	}
}

SCENARIO("A player draws Go Back 3 Spaces", "[cards]") {
	Test test;

	GIVEN("The Chance deck is stacked with Go Back 3 Spaces") {
		test.stack_deck(DeckType::Chance, { Card::Chance_GoBack3Spaces });
		test.stack_deck(DeckType::CommunityChest, { Card::CommunityChest_Gain10 });

		WHEN("player 1 lands on Chance 1") {
			test.land_on_space(Player::p1, Space::Chance_1);

			THEN("player 1 goes back 3 spaces to Income Tax") {
				test.require_position(Player::p1, Space::IncomeTax);
			}
		}
		WHEN("player 1 lands on Chance 2") {
			test.land_on_space(Player::p1, Space::Chance_2);

			THEN("player 1 goes back 3 spaces New York Avenue") {
				test.require_position(Player::p1, Space::Orange_3);
			}
		}
		WHEN("player 1 lands on Chance 3") {
			test.land_on_space(Player::p1, Space::Chance_3);

			THEN("player 1 goes back 3 spaces to Community Chest") {
				test.require_position(Player::p1, Space::CommunityChest_3);
			}
		}
	}
}
