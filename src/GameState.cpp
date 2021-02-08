#include"GameState.h"
#include"Board.h"
using namespace monopoly;

#include<algorithm>
#include<iostream>
#include<numeric>

GameState::GameState(GameSetup setup) 
	: rng (std::seed_seq (setup.seed.begin (), setup.seed.end ()))
	, phase (TurnPhase::TurnStart)
	, bank ()
	, chance (Deck::Type::Chance)
	, communityChest (Deck::Type::CommunityChest)
	, players (init_players(setup))
	, activePlayerIndex (0)
	, doublesStreak (0)
{
}

Bank GameState::get_bank() const {
	return bank;
}

Player GameState::get_player(int playerIndex) const {
	return players[playerIndex];
}

int GameState::get_active_player_index() const {
	return activePlayerIndex;
}

int GameState::get_next_player_index() const {
	return (activePlayerIndex + 1) % static_cast<int> (players.size ());
}

int GameState::get_net_worth(int playerIndex) const {
	auto const& p = players[playerIndex];
	std::vector<int> values;
	int netWorth = p.funds;
	// Add property values
	std::transform(begin(p.deeds), end(p.deeds), std::back_inserter(values),
		[this](Property p) -> int { return mortgagedPropreties.count(p) ? mortgage_value_of_property(p) : face_value_of_property(p); });
	netWorth = std::accumulate(begin(values), end(values), netWorth);
	values.clear();
	// Add building values
	std::transform(begin(p.deeds), end(p.deeds), std::back_inserter(values),
		[this](Property p) -> int { return buildingLevel.at (p) * building_value (p); });
	netWorth = std::accumulate(begin(values), end(values), netWorth);
	return netWorth;
}

TurnPhase GameState::get_turn_phase() const {
	return phase;
}

void GameState::force_turn_start(int playerIndex) {
	phase = TurnPhase::TurnStart;
	activePlayerIndex = playerIndex;
}

void GameState::force_turn_end() {
	phase = TurnPhase::TurnStart;
	activePlayerIndex = get_next_player_index ();
}

void GameState::force_funds(int playerIndex, int funds) {
	players[playerIndex].funds = funds;
}

void GameState::force_add_funds(int playerIndex, int funds) {
	players[playerIndex].funds += funds;
}

void GameState::force_subtract_funds(int playerIndex, int funds) {
	players[playerIndex].funds -= funds;
}

void GameState::force_go_to_jail(int playerIndex) {
	force_position(playerIndex, Space::Jail);
	players[playerIndex].turnsRemainingInJail = MaxJailTurns;
	force_turn_end();
}

void GameState::force_leave_jail(int playerIndex) {
	players[playerIndex].turnsRemainingInJail = 0;
}

void GameState::force_random_roll(int playerIndex) {
	force_roll(playerIndex, random_dice_roll ());
}

void GameState::force_roll(int playerIndex, std::pair<int, int> roll) {
	assert_valid_die_value(roll.first);
	assert_valid_die_value(roll.second);
	activePlayerIndex = playerIndex;

	std::cout << "Rolled " << roll.first << "," << roll.second << std::endl;

	if (roll.first == roll.second)
		doublesStreak += 1;
	else
		doublesStreak = 0;

	if (players[playerIndex].turnsRemainingInJail > 0) {
		if (doublesStreak > 0) {
			doublesStreak = 0;
			std::cout << player_name(playerIndex) << " rolled doubles and left jail" << std::endl;
			force_leave_jail(playerIndex);
		}
		else {
			auto& t = players[playerIndex].turnsRemainingInJail;
			t -= 1;
			if (t > 0) {
				std::cout << player_name(playerIndex) << " is stuck in jail" << std::endl;
				force_turn_end();
				return;
			}
			else {
				std::cout << player_name(playerIndex) << " must pay fine" << std::endl;
				force_subtract_funds(playerIndex, BailCost);
			}
		}
	}
	else if (doublesStreak == 3) {
		doublesStreak = 0;
		std::cout << player_name(playerIndex) << " went to jail for rolling 3 doubles in a row" << std::endl;
		force_go_to_jail(playerIndex);
		return;
	}

	int const sum = roll.first + roll.second;
	force_advance(playerIndex, sum);
}

void GameState::force_advance(int playerIndex, int spaceCount) {
	auto const currentPos = players[playerIndex].position;
	// Spaces are enumerated such that they are ordered according to their position on the board
	auto newPosIndex = static_cast<int> (currentPos) + spaceCount;

	while (newPosIndex >= NumberOfSpaces) {
		newPosIndex -= NumberOfSpaces;
		force_add_funds(playerIndex, GoSalary);
	}

	force_land(playerIndex, static_cast<Space> (newPosIndex));
}

void GameState::force_advance_to(int playerIndex, Space space) {
	auto const currentPos = players[playerIndex].position;
	auto distance = (static_cast<int> (space) - static_cast<int> (currentPos));

	if (distance < 0) {
		distance += NumberOfSpaces;
	}

	force_advance(playerIndex, distance);
}

void GameState::force_land(int playerIndex, Space space) {
	activePlayerIndex = playerIndex;
	force_position(playerIndex, space);
	if (space_is_property (space)) // and is unowned
	{
		auto const property = space_to_property(space);
		force_property_offer(playerIndex, property);
		return;
	}

	switch (space) 
	{
	case Space::IncomeTax:
		force_income_tax(playerIndex);
		break;
	case Space::LuxuryTax:
		force_luxury_tax(playerIndex);
		break;
	}

	if (doublesStreak > 0)
		force_turn_start(playerIndex);
	else
		force_turn_end();
}

void GameState::force_position(int playerIndex, Space space) {
	players[playerIndex].position = space;
}

void GameState::force_property_offer(int playerIndex, Property property) {
		std::cout << "Offering unowned property" << std::endl;
		if (bank.deeds.count(property)) {
			if (players[playerIndex].funds >= face_value_of_property (property))
				force_property_offer_prompt(playerIndex, property);
			else
				force_property_auction(playerIndex);
		}
}

void GameState::force_property_offer_prompt(int playerIndex, Property property) {
	activePlayerIndex = playerIndex;
	phase = TurnPhase::WaitingForBuyPropertyInput;
	std::cout << player_name (playerIndex) << ": Do you want to buy " << to_string(property) << "?" << std::endl;
}

void GameState::force_income_tax(int playerIndex) {
	auto const netWorth = get_net_worth(playerIndex);
	assert(netWorth >= 0);
	auto const tax = std::min<int>(200, static_cast<int> (ceil(netWorth * 0.1)));
	std::cout << player_name (playerIndex) << " pays income tax (" << tax << ")" << std::endl;
	force_subtract_funds(playerIndex, tax);
}

void GameState::force_luxury_tax(int playerIndex) {
	auto const tax = 100;
	std::cout << player_name (playerIndex) << " pays luxury tax (" << tax <<  ")" << std::endl;
	force_subtract_funds(playerIndex, tax);
}

void GameState::force_property_buy(int playerIndex, Property property) {
	std::cout << "Purchasing property" << std::endl;
	force_subtract_funds(playerIndex, face_value_of_property(property));
	force_deed_transfer(bank.deeds, players[playerIndex].deeds, property);
	force_turn_end();
}

void GameState::force_property_auction(int decliningPlayer) {
	std::cout << "Auctioning property" << std::endl;
	force_turn_end(); // todo auction
}


void GameState::force_deed_transfer(std::set<Property>& from, std::set<Property>& to, Property deed) {
	auto const countErased = from.erase(deed);
	assert(countErased == 1);
	auto const insertRet = to.insert(deed);
	assert(insertRet.second);
}

void GameState::force_deed_transfers(std::set<Property>& from, std::set<Property>& to, std::set<Property> deeds) {
	for (auto deed : deeds)
		force_deed_transfer(from, to, deed);
}

void GameState::force_get_out_of_jail_free_card_keep(int playerIndex, Deck::Type deckType) {
	force_get_out_of_jail_free_card_transfer(bank.getOutOfJailFreeCards, players[playerIndex].getOutOfJailFreeCards, deckType);
}

void GameState::force_get_out_of_jail_free_card_transfer(std::set<Deck::Type>& from, std::set<Deck::Type>& to, Deck::Type deckType) {
	auto const countErased = from.erase(deckType);
	assert(countErased == 1);
	auto const insertRet = to.insert(deckType);
	assert(insertRet.second);
}

void GameState::force_get_out_of_jail_free_card_use(int playerIndex, Deck::Type preferredDeckType) {
	auto& player = players[playerIndex];
	assert(player.turnsRemainingInJail > 0);
	assert(player.getOutOfJailFreeCards.size () > 0);
	// If player has preferred card, we will spend that one; othewise, just use whatever they have (which must be 1 card)
	if (!player.getOutOfJailFreeCards.erase(preferredDeckType)) {
		auto const spent = player.getOutOfJailFreeCards.erase(std::begin(player.getOutOfJailFreeCards));
	}
	force_leave_jail(playerIndex);
}

Player GameState::init_player(GameSetup const &setup) {
	Player p;
	p.funds = setup.startingFunds;
	return p;
}

std::vector<Player> GameState::init_players(GameSetup const &setup) {
	return std::vector<Player>(setup.playerCount, init_player(setup));
}

Deck GameState::init_deck(GameSetup const& setup, Deck::Type deck_type) {
	Deck deck(deck_type);
	deck.shuffle(rng);
	return deck;
}

std::pair<int, int> GameState::random_dice_roll() {
	static const std::uniform_int_distribution<int> rollDie(0, 6);
	return {
		rollDie(rng),
		rollDie(rng)
	};
}
