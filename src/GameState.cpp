#include"GameState.h"
#include"Board.h"
#include"Strings.h"
using namespace monopoly;

#include<algorithm>
#include<iostream>
#include<numeric>

GameState::GameState(GameSetup setup) 
	: rng ()
	, phase (TurnPhase::WaitingForRoll)
	, bank ()
	, decks(init_decks (setup))
	, players (init_players(setup))
	, activePlayerIndex (0)
	, doublesStreak (0)
	, lastDiceRoll (0, 0)
{
	std::seed_seq seedSeq(setup.seed.begin(), setup.seed.end());
	rng = std::mt19937(seedSeq);
	decks[DeckType::CommunityChest].shuffle(rng);
	decks[DeckType::Chance].shuffle(rng);
}

int GameState::get_turn() const {
	return turn;
}

Bank GameState::get_bank() const {
	return bank;
}

int GameState::get_player_count() const {
	return players.size();
}

int GameState::get_players_remaining_count() const {
	return std::count_if(players.begin(), players.end(), [](Player const &player) { return ! player.eliminated; });
}

Player GameState::get_player(int playerIndex) const {
	return players[playerIndex];
}

int GameState::get_active_player_index() const {
	return activePlayerIndex;
}

int GameState::get_next_player_index() const {
	int index = activePlayerIndex;
    do {
		index = (index + 1) % static_cast<int> (players.size());
	} while (players[index].eliminated && index != activePlayerIndex);
    return index;
}

int GameState::get_net_worth(int playerIndex) const {
	auto const& p = players[playerIndex];
	std::vector<int> values;
	int netWorth = p.funds;
	// Add property values
	std::transform(begin(p.deeds), end(p.deeds), std::back_inserter(values),
		[this](Property p) -> int { return mortgagedProperties.count(p) ? mortgage_value_of_property(p) : price_of_property(p); });
	netWorth = std::accumulate(begin(values), end(values), netWorth);
	values.clear();
	// Add building values
	std::transform(begin(p.deeds), end(p.deeds), std::back_inserter(values),
		[this](Property p) -> int { return get_building_level(p) * price_per_house_on_property (p); });
	netWorth = std::accumulate(begin(values), end(values), netWorth);
	return netWorth;
}

std::optional<int> GameState::get_property_owner_index(Property property) const {
	for (auto p = 0; p < players.size(); ++p) {
		if (players[p].deeds.count (property))
			return p;
	}
	return {};
 }

bool GameState::get_property_is_mortgaged(Property property) const {
	return mortgagedProperties.count (property) > 0;
}

int GameState::get_properties_owned_in_group(Property property) const {
	auto const ownerOpt = get_property_owner_index(property);
    auto const ownedGroupCount = ownerOpt
        ? get_properties_owned_in_group_by_player(*ownerOpt, property_group (property))
        : 0;
	return ownedGroupCount;
}

int GameState::get_properties_owned_in_group_by_player(int playerIndex, PropertyGroup group) const {
	auto const &deeds = players[playerIndex].deeds;
	return std::count_if(deeds.begin(), deeds.end(), [group](Property property) { return property_is_in_group(property, group); });
}

TurnPhase GameState::get_turn_phase() const {
	return phase;
}

int GameState::get_building_level(Property property) const {
    auto const buildingLevelIt = buildingLevels.find(property);
	if (buildingLevelIt != buildingLevels.end ()) {
		return buildingLevelIt->second;
	}
	return 0;
}

std::map<Property, int> const& GameState::get_building_levels() const {
	return buildingLevels;
}

int GameState::calculate_rent(Property property) const {
	auto const ownerOpt = get_property_owner_index(property);
	if (!ownerOpt) {
		return 0;
    }
	auto const ownerIndex = *ownerOpt;
	auto const &ownerDeeds = players[ownerIndex].deeds;
	auto const group = property_group(property);
    auto const ownedDeedsInGroup = get_properties_owned_in_group_by_player(ownerIndex, group);

	if (group == PropertyGroup::Railroad) {
		return rent_price_of_railroad(ownedDeedsInGroup);
	}
	else if (group == PropertyGroup::Utility) {
		return rent_price_of_utility(ownedDeedsInGroup, lastDiceRoll);
	}
	else { // group is real estate
		auto const buildingLevelIt = buildingLevels.find(property);
        if (buildingLevelIt != buildingLevels.end () && buildingLevelIt->second > 0) {
            return rent_price_of_improved_real_estate(property, buildingLevelIt->second);
        }
        else if (properties_in_group(group).size() == ownedDeedsInGroup) {
            return 2 * rent_price_of_real_estate(property);
        }
        else {
            return rent_price_of_real_estate(property);
        }
	}
}

bool GameState::waiting_on_prompt() const {
	return (phase == TurnPhase::WaitingForBuyPropertyInput);
}

std::pair<int, int> GameState::random_dice_roll() {
	std::uniform_int_distribution<int> rollDie(1, 6);
	lastDiceRoll = std::pair<int, int> { rollDie(rng), rollDie(rng) };
	std::cout << "\t[" << lastDiceRoll.first << "] [" << lastDiceRoll.second << "]" << "\n";
	return lastDiceRoll;
}

std::pair<int, int> GameState::get_last_dice_roll() const {
	return lastDiceRoll;
}

bool GameState::check_if_player_is_allowed_to_roll(int actorIndex) const {
	if (phase != TurnPhase::WaitingForRoll) {
		return false;
	}
	if (activePlayerIndex != actorIndex) {
		return false;
	}
	return true;
}

bool GameState::check_if_player_is_allowed_to_buy_property(int actorIndex) const {
	if (phase != TurnPhase::WaitingForBuyPropertyInput) {
		return false;
	}
	if (activePlayerIndex != actorIndex) {
		return false;
	}
	return true;
}

bool GameState::check_if_player_is_allowed_to_mortgage(int actorIndex, Property property) const {
	static auto const allowedPhases = { TurnPhase::WaitingForBuyPropertyInput, TurnPhase::WaitingForRoll, TurnPhase::WaitingForTurnEnd };
	if (std::find(allowedPhases.begin(), allowedPhases.end(), phase) == allowedPhases.end ()) {
		return false;
	}
    if (get_property_owner_index(property) != actorIndex) {
        return false;
    }
    if (get_property_is_mortgaged(property)) {
        return false;
    }
	for (auto property : properties_in_group(property_group(property))) {
		if (get_building_level (property) > 0) {
			return false;
		}
	}
	return true;
}

bool GameState::check_if_player_is_allowed_to_unmortgage(int actorIndex, Property property) const {
	static auto const allowedPhases = { TurnPhase::WaitingForBuyPropertyInput, TurnPhase::WaitingForRoll, TurnPhase::WaitingForTurnEnd };
	if (std::find(allowedPhases.begin(), allowedPhases.end(), phase) == allowedPhases.end ()) {
		return false;
	}
    if (get_property_owner_index(property) != actorIndex) {
        return false;
    }
    if (! get_property_is_mortgaged(property)) {
        return false;
    }
	return true;
}

void GameState::force_turn_start(int playerIndex) {
	++turn;
	force_roll_prompt(playerIndex);
}

void GameState::force_turn_continue() {
	if (doublesStreak > 0)
		force_turn_start(activePlayerIndex);
	else
		force_turn_end();
}

void GameState::force_turn_end() {
	doublesStreak = 0;
	phase = TurnPhase::WaitingForTurnEnd;
}

void GameState::force_funds(int playerIndex, int funds) {
	players[playerIndex].funds = funds;
	std::cout << player_name (playerIndex) << " has $" << funds << std::endl;
}

void GameState::force_add_funds(int playerIndex, int funds) {
	players[playerIndex].funds += funds;
	std::cout << player_name (playerIndex) << " collects $" << funds << std::endl;
}

void GameState::force_subtract_funds(int playerIndex, int funds) {
	players[playerIndex].funds -= funds;
	std::cout << player_name (playerIndex) << " pays $" << funds << std::endl;

	if (players[playerIndex].funds < 0)
		force_liquidate_prompt(playerIndex);
}

void GameState::force_transfer_funds(int fromPlayerIndex, int toPlayerIndex, int funds) {
	if (fromPlayerIndex == toPlayerIndex)
		return;

	players[fromPlayerIndex].funds -= funds;
	players[toPlayerIndex].funds += funds;

	std::cout << player_name(fromPlayerIndex) << " -($" << funds << ")-> " << player_name(toPlayerIndex) << std::endl;

	if (players[fromPlayerIndex].funds < 0)
		force_liquidate_prompt(fromPlayerIndex, toPlayerIndex);
}

void GameState::force_go_to_jail(int playerIndex) {
	force_position(playerIndex, Space::Jail);
	if (players[playerIndex].turnsRemainingInJail != MaxJailTurns)
		std::cout << player_name(playerIndex) << " went to jail!" << std::endl;
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
	lastDiceRoll = roll;

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
				force_pay_bail(playerIndex);
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

void GameState::force_advance(int playerIndex, int dist) {
	auto const currentPos = players[playerIndex].position;

	if (advancing_will_pass_go (currentPos, dist))
		force_add_funds(playerIndex, GoSalary);

	force_land(playerIndex, add_distance(players[playerIndex].position, dist));
}

void GameState::force_advance_without_landing(int playerIndex, int dist) {
	auto const currentPos = players[playerIndex].position;

	if (advancing_will_pass_go (currentPos, dist))
		force_add_funds(playerIndex, GoSalary);

	force_position(playerIndex, add_distance(players[playerIndex].position, dist));
}

void GameState::force_advance_to(int playerIndex, Space space) {
	auto const currentPos = players[playerIndex].position;
	force_advance(playerIndex, distance (currentPos, space));
}

void GameState::force_advance_to_without_landing(int playerIndex, Space space) {
	auto const currentPos = players[playerIndex].position;
	force_advance_without_landing(playerIndex, distance (currentPos, space));
}

void GameState::force_land(int playerIndex, Space space) {
	activePlayerIndex = playerIndex;
	force_leave_jail(playerIndex);
	force_position(playerIndex, space);
	if (space_is_property (space))
	{
        auto const property = space_to_property(space);
		auto const ownerOpt = get_property_owner_index(property);
		if (! ownerOpt) {
            force_property_offer(playerIndex, property);
		}
		else if (ownerOpt != playerIndex) {
            auto const rent = calculate_rent(property);
			force_transfer_funds(playerIndex, *ownerOpt, rent);
		}
		else {
			// Owner pays no rent to stay
		}
	}
	else {
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
	}

	// Some effects can cause a prompt to open
	if (! waiting_on_prompt()) {
		force_turn_continue();
	}
}

void GameState::force_position(int playerIndex, Space space) {
	players[playerIndex].position = space;
}

void GameState::force_property_offer(int playerIndex, Property property) {
    std::cout << "Offering unowned property" << std::endl;
    if (bank.deeds.count(property)) {
        if (players[playerIndex].funds >= price_of_property(property)) {
            force_property_offer_prompt(playerIndex, property);
        }
        else
        {
            force_property_auction(property);
        }
    }
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
	force_subtract_funds(playerIndex, price_of_property(property));
	force_give_deed(playerIndex, property);
	force_turn_continue();
}

void GameState::force_property_auction(Property property) {
	std::cout << "Auctioning property" << std::endl;
	// todo auction
	force_turn_continue();
}

void GameState::force_give_deed(int playerIndex, Property property) {
	force_transfer_deed(bank.deeds, players[playerIndex].deeds, property);
}

void GameState::force_give_deeds(int playerIndex, std::set<Property> properties) {
	for (auto property : properties)
		force_give_deed(playerIndex, property);
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

void GameState::force_mortgage(Property property) {
	auto const ownerOpt = get_property_owner_index(property);
	assert(ownerOpt);
	auto const wasMortgaged = mortgagedProperties.insert(property).second;
	assert(wasMortgaged);
	force_add_funds(*ownerOpt, mortgage_value_of_property (property));
}

void GameState::force_unmortgage(Property property) {
	auto const ownerOpt = get_property_owner_index(property);
	assert(ownerOpt);
	auto const wasMortgaged = mortgagedProperties.erase(property);
	assert(wasMortgaged);
	force_subtract_funds(*ownerOpt, mortgage_value_of_property (property) * (1 + MortgageInterestRate));
}

void GameState::force_set_mortgaged(Property property, bool mortgaged) {
	if (mortgaged) {
		mortgagedProperties.insert(property);
	}
	else {
		mortgagedProperties.erase(property);
	}
}

void GameState::force_set_building_levels(std::map<Property, int> newBuildingLevels) {
	newBuildingLevels.insert(buildingLevels.begin(), buildingLevels.end());
	swap(newBuildingLevels, buildingLevels);
}

void GameState::force_give_get_out_of_jail_free_card(int playerIndex, DeckType deckType) {
	auto& deck = decks[deckType];
	deck.remove_card(get_out_of_jail_free_card (deckType));
	players[playerIndex].getOutOfJailFreeCards.insert(deckType);
}

void GameState::force_transfer_get_out_of_jail_free_card(int fromPlayerIndex, int toPlayerIndex, DeckType deckType) {
	auto& fromCards = players[fromPlayerIndex].getOutOfJailFreeCards;
	auto& toCards = players[toPlayerIndex].getOutOfJailFreeCards;
	if (fromCards.erase(deckType))
        toCards.insert(deckType);
}

void GameState::force_transfer_get_out_of_jail_free_cards(int fromPlayerIndex, int toPlayerIndex) {
	for (auto deckType : { DeckType::CommunityChest, DeckType::Chance }) {
		force_transfer_get_out_of_jail_free_card(fromPlayerIndex, toPlayerIndex, deckType);
	}
}

void GameState::force_use_get_out_of_jail_free_card(int playerIndex, DeckType preferredDeckType) {
	auto& player = players[playerIndex];
	if (player.turnsRemainingInJail == 0)
		return;
	if (player.getOutOfJailFreeCards.size() == 0)
		return;
	auto const usedDeckType = (player.getOutOfJailFreeCards.count(preferredDeckType))
		? preferredDeckType
		: *player.getOutOfJailFreeCards.begin();

	std::cout << player_name(playerIndex) << " used a " << to_string (usedDeckType) << " Get Out of Jail Free card" << std::endl;
	force_return_get_out_of_jail_free_card(playerIndex, usedDeckType);
	force_leave_jail(playerIndex);
}

void GameState::force_return_get_out_of_jail_free_card(int playerIndex, DeckType deckType) {
	auto& player = players[playerIndex];
	player.getOutOfJailFreeCards.erase(deckType);
	decks[deckType].add_card(get_out_of_jail_free_card(deckType));
}

void GameState::force_return_get_out_of_jail_free_cards(int playerIndex) {
	for (auto deckType : { DeckType::CommunityChest, DeckType::Chance }) {
        force_return_get_out_of_jail_free_card(playerIndex, deckType);
	}
}

void GameState::force_pay_bail(int playerIndex) {
    force_subtract_funds(playerIndex, BailCost);
	force_leave_jail(playerIndex);
}

void GameState::force_bankrupt(int debtorPlayerIndex) {
	auto &debtor = players[debtorPlayerIndex];
	// sell buildings
	// sell houses
	std::set<Property> deedsToAuction;
	swap(deedsToAuction, debtor.deeds);
	for (auto property : debtor.deeds) {
		force_property_auction(property);
	}
	force_return_get_out_of_jail_free_cards(debtorPlayerIndex);
	debtor.eliminated = true;
}

void GameState::force_bankrupt(int debtorPlayerIndex, int creditorPlayerIndex) {
	auto& debtor = players[debtorPlayerIndex];
	auto& creditor = players[creditorPlayerIndex];
	// sell buildings
	// sell houses
	force_transfer_funds(debtorPlayerIndex, creditorPlayerIndex, debtor.funds); // likely negative
	force_transfer_deeds(debtor.deeds, creditor.deeds, debtor.deeds);
    force_transfer_get_out_of_jail_free_cards(debtorPlayerIndex, creditorPlayerIndex);
	debtor.eliminated = true;
}

void GameState::force_roll_prompt(int playerIndex) {
	activePlayerIndex = playerIndex;
	phase = TurnPhase::WaitingForRoll;
}

void GameState::force_property_offer_prompt(int playerIndex, Property property) {
	activePlayerIndex = playerIndex;
	phase = TurnPhase::WaitingForBuyPropertyInput;
}

void GameState::force_liquidate_prompt(int debtorPlayerIndex) {
	/// \todo give debtor a chance to trade or liquidate assets 
	force_bankrupt(debtorPlayerIndex);
}

void GameState::force_liquidate_prompt(int debtorPlayerIndex, int creditorPlayerIndex) {
	/// \todo give debtor a chance to trade or liquidate assets 
	force_bankrupt(debtorPlayerIndex, creditorPlayerIndex);
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
