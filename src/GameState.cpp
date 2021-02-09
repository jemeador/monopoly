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
	, decks(init_decks (setup))
	, players (init_players(setup))
	, activePlayerIndex (0)
	, doublesStreak (0)
{
	decks[DeckType::CommunityChest].shuffle(rng);
	decks[DeckType::Chance].shuffle(rng);
}

Bank GameState::get_bank() const {
	return bank;
}

int GameState::get_player_count() const {
	return players.size();
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

void GameState::force_turn_continue() {
	if (doublesStreak > 0)
		force_turn_start(activePlayerIndex);
	else
		force_turn_end();
}

void GameState::force_turn_end() {
	doublesStreak = 0;
	phase = TurnPhase::TurnEnd;
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

void GameState::force_transfer_funds(int fromPlayerIndex, int toPlayerIndex, int funds) {
	players[fromPlayerIndex].funds -= funds;
	players[toPlayerIndex].funds += funds;

	if (players[fromPlayerIndex].funds < 0)
		force_liquidate_prompt(fromPlayerIndex, toPlayerIndex);
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

	while (newPosIndex < 0) {
		newPosIndex += NumberOfSpaces;
		// There are no in-game effects that can cause going in reverse behind GO
	}
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
	case Space::CommunityChest_1:
	case Space::CommunityChest_2:
	case Space::CommunityChest_3:
		force_draw_community_chest_card(playerIndex);
		break;
	case Space::Chance_1:
	case Space::Chance_2:
	case Space::Chance_3:
		force_draw_chance_card(playerIndex);
		break;
	case Space::GoToJail:
		force_go_to_jail(playerIndex);
		break;
	}

	force_turn_continue();
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
			{
				force_property_auction(property);
				force_turn_continue();
			}
		}
}

void GameState::force_property_offer_prompt(int playerIndex, Property property) {
	activePlayerIndex = playerIndex;
	phase = TurnPhase::WaitingForBuyPropertyInput;
	std::cout << player_name (playerIndex) << ": Do you want to buy " << to_string(property) << "?" << std::endl;
}

void GameState::force_stack_deck(DeckType deckType, DeckContainer const& cards) {
	decks[deckType].stack_deck(cards);
}

void GameState::force_draw_chance_card(int playerIndex) {
	force_draw_card(playerIndex, DeckType::Chance);
}

void GameState::force_draw_community_chest_card(int playerIndex) {
	force_draw_card(playerIndex, DeckType::CommunityChest);
}

void GameState::force_draw_card(int playerIndex, DeckType deckType) {
	auto& deck = decks[deckType];
	std::cout << player_name(playerIndex) << " draws a " + to_string(deckType) + " card " << std::endl;
	auto const card = deck.draw();
	std::cout << "\t" << card_data(card).effectText << std::endl;
	apply_card_effect(*this, playerIndex, card);
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
	force_transfer_deed(bank.deeds, players[playerIndex].deeds, property);
}

void GameState::force_property_auction(Property property) {
	std::cout << "Auctioning property" << std::endl;
}

void GameState::force_transfer_deed(std::set<Property>& from, std::set<Property>& to, Property deed) {
	auto const countErased = from.erase(deed);
	assert(countErased == 1);
	auto const insertRet = to.insert(deed);
	assert(insertRet.second);
}

void GameState::force_transfer_deeds(std::set<Property>& from, std::set<Property>& to, std::set<Property> deeds) {
	for (auto deed : deeds)
		force_transfer_deed(from, to, deed);
}

void GameState::force_keep_get_out_of_jail_free_card(int playerIndex, DeckType deckType) {
	auto& deck = decks[deckType];
	deck.remove_card(get_out_of_jail_free_card (deckType));
	players[playerIndex].getOutOfJailFreeCards.insert(deckType);
}

void GameState::force_transfer_get_out_of_jail_free_card(int fromPlayerIndex, int toPlayerIndex, DeckType deckType) {
	auto& fromCards = players[fromPlayerIndex].getOutOfJailFreeCards;
	auto& toCards = players[toPlayerIndex].getOutOfJailFreeCards;
	auto const countErased = fromCards.erase(deckType);
	assert(countErased == 1);
	auto const insertRet = toCards.insert(deckType);
	assert(insertRet.second);
}

void GameState::force_use_get_out_of_jail_free_card(int playerIndex, DeckType preferredDeckType) {
	auto& player = players[playerIndex];
	assert(player.turnsRemainingInJail > 0);
	assert(player.getOutOfJailFreeCards.size () > 0);
	// If player has preferred card, we will spend that one; othewise, just use whatever they have (which must be 1 card)
	if (!player.getOutOfJailFreeCards.erase(preferredDeckType)) {
		player.getOutOfJailFreeCards.erase(std::begin(player.getOutOfJailFreeCards));
	}
	force_leave_jail(playerIndex);
}

void GameState::force_liquidate_prompt(int debtorPlayerIndex) {
	/// \todo give debtor a chance to trade or liquidate assets 
	force_bankrupt(debtorPlayerIndex);
}

void GameState::force_liquidate_prompt(int debtorPlayerIndex, int creditorPlayerIndex) {
	/// \todo give debtor a chance to trade or liquidate assets 
	force_bankrupt(debtorPlayerIndex, creditorPlayerIndex);
}

void GameState::force_bankrupt(int debtorPlayerIndex) {
	auto &debtor = players[debtorPlayerIndex];
	std::set<Property> deedsToAuction;
	swap(deedsToAuction, debtor.deeds);

	for (auto property : debtor.deeds) {
		force_property_auction(property);
	}
}

void GameState::force_bankrupt(int debtorPlayerIndex, int creditorPlayerIndex) {
	auto& debtor = players[debtorPlayerIndex];
	auto& creditor = players[creditorPlayerIndex];
	// sell buildings
	// sell houses
	force_transfer_funds(debtorPlayerIndex, creditorPlayerIndex, debtor.funds); // likely negative
	force_transfer_deeds(debtor.deeds, creditor.deeds, debtor.deeds);
	creditor.getOutOfJailFreeCards.insert(begin(debtor.getOutOfJailFreeCards), end(debtor.getOutOfJailFreeCards));
	// retire player
}

Player GameState::init_player(GameSetup const &setup) {
	Player p;
	p.funds = setup.startingFunds;
	return p;
}

std::vector<Player> GameState::init_players(GameSetup const &setup) {
	return std::vector<Player>(setup.playerCount, init_player(setup));
}

Deck GameState::init_deck(GameSetup const& setup, DeckType deck_type) {
	return Deck(deck_type);
}

std::map<DeckType, Deck> GameState::init_decks(GameSetup const& setup) {
	return {
		{ DeckType::Chance, init_deck(setup, DeckType::Chance) },
		{ DeckType::CommunityChest, init_deck(setup, DeckType::CommunityChest) },
	};
}

std::pair<int, int> GameState::random_dice_roll() {
	static const std::uniform_int_distribution<int> rollDie(0, 6);
	return {
		rollDie(rng),
		rollDie(rng)
	};
}
