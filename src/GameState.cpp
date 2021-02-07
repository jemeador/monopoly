#include"GameState.h"
#include"Board.h"
using namespace monopoly;

#include<iostream>

GameState::GameState(GameSetup setup) 
	: phase (TurnPhase::TurnStart)
	, bank ()
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

void GameState::force_property_buy(int playerIndex, Property property) {
	std::cout << "Purchasing property" << std::endl;
	force_subtract_funds(playerIndex, list_price_of_property(property));
	force_deed_transfer(bank.deeds, players[playerIndex].deeds, property);
	force_turn_end();
}

void GameState::force_property_auction(int decliningPlayer) {
	std::cout << "Auctioning property" << std::endl;
	force_turn_end(); // todo auction
}

void GameState::force_subtract_funds(int playerIndex, int funds) {
	assert (players[playerIndex].funds >= funds);
	players[playerIndex].funds -= funds;
}

void GameState::force_funds(int playerIndex, int funds) {
	players[playerIndex].funds = funds;
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
			force_leave_jail(playerIndex);
		}
		else {
			auto& t = players[playerIndex].turnsRemainingInJail;
			t -= 1;
			if (t > 0) {
				force_turn_end();
				return;
			}
			else {
				force_subtract_funds(playerIndex, 50);
			}
		}
	}
	else if (doublesStreak == 3) {
		doublesStreak = 0;
		force_go_to_jail(playerIndex);
		return;
	}

	int const sum = roll.first + roll.second;
	force_advance(playerIndex, sum);
}

void GameState::force_go_to_jail(int playerIndex) {
	force_position(playerIndex, Space::Jail);
	players[playerIndex].turnsRemainingInJail = 3;
	force_turn_end();
}

void GameState::force_leave_jail(int playerIndex) {
	players[playerIndex].turnsRemainingInJail = 0;
}

void GameState::force_advance(int playerIndex, int spaceCount) {
	auto const currentPos = players[playerIndex].position;
	// Spaces are enumerated such that they are ordered according to their position on the board
	auto const newPosIndex = (static_cast<int> (currentPos) + spaceCount) % NumberOfSpaces;
	force_land(playerIndex, static_cast<Space> (newPosIndex));
}

void GameState::force_land(int playerIndex, Space space) {
	activePlayerIndex = playerIndex;
	force_position(playerIndex, space);
	if (space_is_property (space)) // and is unowned
	{
		std::cout << "Landed on unowned property" << std::endl;
		auto property = space_to_property(space);
		if (bank.deeds.count(property)) {
			if (players[playerIndex].funds >= list_price_of_property (property))
				force_purchase_prompt(playerIndex, property);
			else
				force_property_auction(playerIndex);
		}
	}
}

void GameState::force_position(int playerIndex, Space space) {
	players[playerIndex].position = space;
}

void GameState::force_purchase_prompt(int playerIndex, Property property) {
	activePlayerIndex = playerIndex;
	phase = TurnPhase::WaitingForBuyPropertyInput;
	std::cout << player_name (playerIndex) << ": Do you want to buy " << to_string(property) << "?" << std::endl;
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
