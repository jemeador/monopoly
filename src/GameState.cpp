#include"GameState.h"
#include"Board.h"
#include"DisplayStrings.h"
using namespace monopoly;

#include<algorithm>
#include<iostream>
#include<numeric>

GameState::GameState(GameSetup setup)
    : rng()
    , phase(TurnPhase::WaitingForRoll)
    , bank()
    , decks(init_decks(setup))
    , players(init_players(setup))
    , doublesStreak(0)
    , lastDiceRoll(0, 0)
    , pendingTradeAgreement()
    , pendingDebtSettlements()
    , currentAuction()
    , propertiesPendingAuction()
    , pendingAcquisitions()
    , pendingPurchaseDecision(false)
    , pendingRoll(true)
    , activePlayerIndex(Player::p1)
{
    std::seed_seq seedSeq(setup.seed.begin(), setup.seed.end());
    rng = std::mt19937(seedSeq);
    decks[DeckType::CommunityChest].shuffle(rng);
    decks[DeckType::Chance].shuffle(rng);
}

bool GameState::is_game_over() const {
    return phase == TurnPhase::GameOver;
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
    return std::count_if(players.begin(), players.end(), [](Player const& player) { return !player.eliminated; });
}

bool GameState::get_player_eliminated(int playerIndex) const
{
    return players[playerIndex].eliminated;
}

int GameState::get_player_funds(int playerIndex) const
{
    return players[playerIndex].funds;
}

Space GameState::get_player_position(int playerIndex) const
{
    return players[playerIndex].position;
}

int GameState::get_player_turns_remaining_in_jail(int playerIndex) const
{
    return players[playerIndex].turnsRemainingInJail;
}

int GameState::get_active_player_index() const {
    return activePlayerIndex;
}

std::set<Property> GameState::get_player_deeds(int playerIndex) const {
    return players[playerIndex].deeds;
}

std::set<DeckType> GameState::get_player_get_out_of_jail_free_cards(int playerIndex) const {
    return players[playerIndex].getOutOfJailFreeCards;
}

int GameState::get_controlling_player_index() const {
    switch (phase) {
        case TurnPhase::WaitingForTradeOfferResponse:
            return pendingTradeAgreement->consideringPlayer;
        case TurnPhase::WaitingForDebtSettlement:
            return pendingDebtSettlements.front ().debtor;
        case TurnPhase::WaitingForBids:
            return currentAuction.biddingOrder.front ();
        case TurnPhase::WaitingForAcquisitionManagement:
            return pendingAcquisitions.front().recipient;
        case TurnPhase::WaitingForBuyPropertyInput:
            return activePlayerIndex;
        case TurnPhase::WaitingForRoll:
            return activePlayerIndex;
        case TurnPhase::WaitingForTurnEnd:
            return activePlayerIndex;
        case TurnPhase::GameOver:
            return activePlayerIndex;
    }
    return activePlayerIndex;
}

int GameState::get_next_player_index(int playerIndex) const {
    if (playerIndex == -1) {
        playerIndex = activePlayerIndex;
    }
    int nextIndex = playerIndex;
    do {
        nextIndex = (nextIndex + 1) % static_cast<int> (players.size());
    } while (players[nextIndex].eliminated && nextIndex != playerIndex);
    return nextIndex;
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
        [this](Property p) -> int { return get_building_level(p) * price_per_house_on_property(p); });
    netWorth = std::accumulate(begin(values), end(values), netWorth);
    return netWorth;
}

int GameState::get_property_owner_index(Property property) const {
    if (property == Property::Invalid) {
        return -1;
    }
    for (auto p = 0; p < players.size(); ++p) {
        if (players[p].deeds.count(property))
            return p;
    }
    return Player::None;
}

bool GameState::get_property_is_mortgaged(Property property) const {
    return mortgagedProperties.count(property) > 0;
}

int GameState::get_properties_owned_in_group(Property property) const {
    auto const owner = get_property_owner_index(property);
    auto const ownedGroupCount = owner
        ? get_properties_owned_in_group_by_player(owner, property_group(property))
        : 0;
    return ownedGroupCount;
}

int GameState::get_properties_owned_in_group_by_player(int playerIndex, PropertyGroup group) const {
    if (playerIndex == Player::None)
        return 0;
    auto const& deeds = players[playerIndex].deeds;
    return std::count_if(deeds.begin(), deeds.end(), [group](Property property) { return property_is_in_group(property, group); });
}

TurnPhase GameState::get_turn_phase() const {
    return phase;
}

int GameState::get_building_level(Property property) const {
    auto const buildingLevelIt = buildingLevels.find(property);
    if (buildingLevelIt != buildingLevels.end()) {
        return buildingLevelIt->second;
    }
    return 0;
}

int GameState::get_min_building_level_in_group(PropertyGroup group) const {
    int min = HotelLevel;
    for (auto property : properties_in_group(group)) {
        min = std::min(get_building_level(property), min);
    }
    return min;
}

int GameState::get_max_building_level_in_group(PropertyGroup group) const {
    int max = 0;
    for (auto property : properties_in_group(group)) {
        max = std::max(get_building_level(property), max);
    }
    return max;
}

std::map<Property, int> const& GameState::get_building_levels() const {
    return buildingLevels;
}

Auction GameState::get_current_auction() const {
    return currentAuction;
}

int GameState::calculate_rent(Property property) const {
    auto const ownerIndex = get_property_owner_index(property);
    if (ownerIndex == Player::None) {
        return 0;
    }
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
        if (buildingLevelIt != buildingLevels.end() && buildingLevelIt->second > 0) {
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

int GameState::calculate_closing_costs_on_sale(Property property) const {
    if (mortgagedProperties.count(property)) {
        return mortgage_value_of_property(property) * MortgageInterestRate;
    }
    else {
        return 0;
    }
}

int GameState::calculate_liquid_assets_value(int playerIndex) const {
    auto const& p = players[playerIndex];
    return p.funds + calculate_liquid_value_of_deeds (p.deeds) + calculate_liquid_value_of_buildings (p.deeds);
}

int GameState::calculate_liquid_value_of_deeds(std::set<Property> deeds) const {
    std::vector<int> values;
    std::transform(begin(deeds), end(deeds), std::back_inserter(values),
        [this](Property p) -> int { return mortgagedProperties.count(p) ? 0 : mortgage_value_of_property(p); });
    return std::accumulate(begin(values), end(values), 0);
}

int GameState::calculate_liquid_value_of_buildings(std::set<Property> properties) const {
    std::vector<int> values;
    std::transform(begin(properties), end(properties), std::back_inserter(values),
        [this](Property p) -> int { return get_building_level(p) * sell_price_per_house_on_property(p); });
    return std::accumulate(begin(values), end(values), 0);
}

int GameState::calculate_liquid_value_of_promise(Promise promise) const {
    return promise.cash + calculate_liquid_value_of_deeds(promise.deeds);
}

std::pair<int, int> GameState::random_dice_roll() {
    std::uniform_int_distribution<int> rollDie(1, 6);
    lastDiceRoll = std::pair<int, int>{ rollDie(rng), rollDie(rng) };
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
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_use_get_out_jail_free_card(int actorIndex) const {
    if (phase != TurnPhase::WaitingForRoll) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    auto const& player = players.at(actorIndex);

    if (player.turnsRemainingInJail == 0) {
        return false;
    }
    if (player.getOutOfJailFreeCards.empty()) {
        return false;
    }

    return true;
}
bool GameState::check_if_player_is_allowed_to_pay_bail(int actorIndex) const {
    if (phase != TurnPhase::WaitingForRoll) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    auto const& player = players.at(actorIndex);

    if (player.turnsRemainingInJail == 0) {
        return false;
    }
    if (player.funds < BailCost) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_buy_property(int actorIndex) const {
    if (!check_if_player_is_allowed_to_auction_property(actorIndex)) {
        return false;
    }
    auto const landedOnProperty = space_to_property(players[actorIndex].position);
    if (players[actorIndex].funds < price_of_property(landedOnProperty)) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_auction_property(int actorIndex) const {
    if (phase != TurnPhase::WaitingForBuyPropertyInput) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    auto const landedOnProperty = space_to_property(players[actorIndex].position);
    assert(landedOnProperty != Property::Invalid); // shouldn't be waiting for buy on non-property
    if (landedOnProperty == Property::Invalid) {
        return false;
    }
    auto ownerIndex = get_property_owner_index(landedOnProperty);
    assert(ownerIndex == Player::None); // shouldn't be waiting for buy if player owned
    if (ownerIndex != Player::None) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_mortgage(int actorIndex, Property property) const {
    if (phase == TurnPhase::WaitingForBids || phase == TurnPhase::GameOver) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    if (get_property_owner_index(property) != actorIndex) {
        return false;
    }
    if (get_property_is_mortgaged(property)) {
        return false;
    }
    if (get_max_building_level_in_group(property_group(property)) > 0) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_unmortgage(int actorIndex, Property property) const {
    if (phase == TurnPhase::WaitingForBids ||
        phase == TurnPhase::WaitingForDebtSettlement ||
        phase == TurnPhase::GameOver) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    if (get_property_owner_index(property) != actorIndex) {
        return false;
    }
    if (!get_property_is_mortgaged(property)) {
        return false;
    }
    if (players[actorIndex].funds < unmortgage_price_of_property(property)) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_buy_building(int actorIndex, Property property) const {
    if (property == Property::Invalid) {
        return false;
    }
    if (phase == TurnPhase::WaitingForBids ||
        phase == TurnPhase::WaitingForDebtSettlement ||
        phase == TurnPhase::GameOver) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    if (players[actorIndex].funds < price_per_house_on_property(property)) {
        return false;
    }

    auto const currentBuildingLevel = get_building_level(property);
    if (currentBuildingLevel == HotelLevel) {
        return false;
    }
    for (auto p : properties_in_group(property_group(property))) {
        if (get_property_owner_index(p) != actorIndex) {
            return false;
        }
        if (currentBuildingLevel > get_building_level(p)) {
            return false;
        }
        if (get_property_is_mortgaged(p)) {
            return false;
        }
    }

    if (currentBuildingLevel + 1 < HotelLevel && bank.houses == 0) {
        return false;
    }
    if (currentBuildingLevel + 1 == HotelLevel && bank.hotels == 0) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_sell_building(int actorIndex, Property property) const {
    if (property == Property::Invalid) {
        return false;
    }
    if (phase == TurnPhase::WaitingForBids ||
        phase == TurnPhase::GameOver) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }

    auto const currentBuildingLevel = get_building_level(property);
    if (currentBuildingLevel == 0) {
        return false;
    }
    for (auto p : properties_in_group(property_group(property))) {
        if (get_property_owner_index(p) != actorIndex) {
            return false;
        }
        if (currentBuildingLevel < get_building_level(p)) {
            return false;
        }
        if (get_property_is_mortgaged(p)) {
            return false;
        }
    }

    if (currentBuildingLevel == HotelLevel && bank.houses < HotelLevel - 1) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_sell_all_buildings(int actorIndex, PropertyGroup group) const {
    if (phase == TurnPhase::WaitingForBids ||
        phase == TurnPhase::GameOver) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    if (get_min_building_level_in_group(group) == 0) {
        return false;
    }
    for (auto p : properties_in_group(group)) {
        if (get_property_owner_index(p) != actorIndex) {
            return false;
        }
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_bid(int actorIndex, int amount) const {
    // If a player can't decline a bid, they can't bid; but we still have to check the amount bid
    if (! check_if_player_is_allowed_to_decline_bid (actorIndex)) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    if (amount <= currentAuction.highestBid) {
        return false;
    }
    // Avoid letting players involuntarily bankrupt themselves by bidding too much
    if (amount + calculate_closing_costs_on_sale(currentAuction.property) > calculate_liquid_assets_value(actorIndex)) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_decline_bid(int actorIndex) const {
    if (phase != TurnPhase::WaitingForBids) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_decline_trade(int actorIndex) const {
    if (phase != TurnPhase::WaitingForTradeOfferResponse) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_end_turn(int actorIndex) const {
    if (phase != TurnPhase::WaitingForTurnEnd) {
        return false;
    }
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_is_allowed_to_resign(int actorIndex) const {
    if (get_controlling_player_index () != actorIndex) {
        return false;
    }
    if (phase == TurnPhase::GameOver) {
        return false;
    }
    return true;
}

bool GameState::check_if_trade_is_valid(Trade trade) const {
    if (phase == TurnPhase::WaitingForBids) {
        return false;
    }
    if (get_controlling_player_index() != trade.offeringPlayer) {
        return false;
    }
    if (trade.offeringPlayer == trade.consideringPlayer) {
        return false;
    }
    if (trade.offer.cash > 0 && trade.consideration.cash > 0) { // don't trade cash for cash
        return false;
    }
    auto check_validity_in_one_direction = [this](Trade t) {
        if (t.offer == Promise{}) { // can't offer nothing
            return false;
        }
        if (t.offer.cash < 0) { // can't trade debt, use a positive value
            return false;
        }
        if (!check_if_player_can_fulfill_promise(t.offeringPlayer, t.offer)) {
            return false;
        }
        if (!check_if_player_can_pay_closing_costs(t.offeringPlayer, t.offer, t.consideration)) {
            return false;
        }
        return true;
    };
    if (!check_validity_in_one_direction(trade)) {
        return false;
    }
    if (!check_validity_in_one_direction(reciprocal_trade(trade))) {
        return false;
    }
    return true;
}

bool GameState::check_if_player_can_fulfill_promise(int playerIndex, Promise promise) const {
    auto const& promisingPlayer = players[playerIndex];
    if (promise.cash > 0 && promise.cash > promisingPlayer.funds) {
        return false;
    }
    for (auto const deed : promise.deeds) {
        if (promisingPlayer.deeds.count(deed) == 0) {

            return false;
        }
        if (get_max_building_level_in_group(property_group(deed)) > 0) {
            return false;
        }
    }
    for (auto const gojfc : promise.getOutOfJailFreeCards) {
        if (promisingPlayer.getOutOfJailFreeCards.count(gojfc) == 0) {
            return false;
        }
    }
    return true;
}

bool GameState::check_if_player_can_pay_closing_costs(int playerIndex, Promise lostAssets, Promise gainedAssets) const {
    auto liquidValue = calculate_liquid_assets_value(playerIndex);
    liquidValue -= calculate_liquid_value_of_promise(lostAssets);
    liquidValue += calculate_liquid_value_of_promise(gainedAssets);
    return liquidValue >= 0;
}

void GameState::player_action_roll(int playerIndex) {
    force_roll(playerIndex, random_dice_roll());
    resolve_game_state();
}

void GameState::player_action_use_get_out_of_jail_free_card(int playerIndex, DeckType preferredDeckType) {
    assert(check_if_player_is_allowed_to_use_get_out_jail_free_card(playerIndex));
    auto& player = players[playerIndex];
    if (player.turnsRemainingInJail == 0)
        return;
    if (player.getOutOfJailFreeCards.size() == 0)
        return;
    auto const usedDeckType = (player.getOutOfJailFreeCards.count(preferredDeckType))
        ? preferredDeckType
        : *player.getOutOfJailFreeCards.begin();

    std::cout << player_name(playerIndex) << " used a " << to_string(usedDeckType) << " Get Out of Jail Free card" << std::endl;
    force_return_get_out_of_jail_free_card(playerIndex, usedDeckType);
    force_leave_jail(playerIndex);
    resolve_game_state();
}

void GameState::player_action_pay_bail(int playerIndex) {
    assert(check_if_player_is_allowed_to_pay_bail(playerIndex));
    force_pay_bail(playerIndex);
    resolve_game_state();
}

void GameState::player_action_buy_property(int playerIndex) {
    assert(check_if_player_is_allowed_to_buy_property(playerIndex));
    auto const landedOnProperty = space_to_property(players[playerIndex].position);
    force_subtract_funds(playerIndex, price_of_property(landedOnProperty));
    force_give_deed(playerIndex, landedOnProperty);
    pendingPurchaseDecision = false;
    resolve_game_state();
}

void GameState::player_action_auction_property(int playerIndex) {
    assert(check_if_player_is_allowed_to_auction_property(playerIndex));
    auto const landedOnProperty = space_to_property(players[playerIndex].position);
    propertiesPendingAuction.push(landedOnProperty);
    pendingPurchaseDecision = false;
    resolve_game_state();
}

void GameState::player_action_mortgage(int playerIndex, Property property) {
    assert(check_if_player_is_allowed_to_mortgage(playerIndex, property));
    force_set_mortgaged(property, true);
    force_add_funds(playerIndex, mortgage_value_of_property(property));
    resolve_game_state();
}

void GameState::player_action_unmortgage(int playerIndex, Property property) {
    assert(check_if_player_is_allowed_to_unmortgage(playerIndex, property));
    force_set_mortgaged(property, false);
    force_subtract_funds(playerIndex, unmortgage_price_of_property(property));
    resolve_game_state();
}

void GameState::player_action_buy_building(int playerIndex, Property property) {
    assert(check_if_player_is_allowed_to_buy_building(playerIndex, property));
    force_add_building(property);
    force_subtract_funds(playerIndex, price_per_house_on_property(property));
    resolve_game_state();
}

void GameState::player_action_sell_building(int playerIndex, Property property) {
    assert(check_if_player_is_allowed_to_sell_building(playerIndex, property));
    force_remove_building(property);
    force_add_funds(playerIndex, sell_price_per_house_on_property(property));
    resolve_game_state();
}

void GameState::player_action_sell_all_buildings(int playerIndex, PropertyGroup group) { 
    assert(check_if_player_is_allowed_to_sell_all_buildings(playerIndex, group));
    force_sell_all_buildings(playerIndex, group);
    resolve_game_state();
}

void GameState::player_action_bid(int playerIndex, int amount) {
    assert(check_if_player_is_allowed_to_bid (playerIndex, amount));
    currentAuction.highestBid = amount;
    currentAuction.biddingOrder.erase(currentAuction.biddingOrder.begin ());
    currentAuction.biddingOrder.push_back(playerIndex);
    resolve_game_state();
}

void GameState::player_action_decline_bid(int playerIndex) {
    assert(check_if_player_is_allowed_to_decline_bid (playerIndex));
    currentAuction.biddingOrder.erase(currentAuction.biddingOrder.begin ());
    resolve_game_state();
}

void GameState::player_action_offer_trade(Trade trade) {
    assert(check_if_trade_is_valid (trade));

    if (pendingTradeAgreement && trades_are_reciprocal(trade, *pendingTradeAgreement)) {
        pendingTradeAgreement = {};
        force_trade(trade);
    }
    else {
        pendingTradeAgreement = trade;
    }
    resolve_game_state();
}

void GameState::player_action_decline_trade(int playerIndex) {
    assert(check_if_player_is_allowed_to_decline_trade(playerIndex));
    pendingTradeAgreement = {};
    resolve_game_state();
}

void GameState::player_action_end_turn(int playerIndex) {
    assert(check_if_player_is_allowed_to_end_turn(playerIndex));
    force_start_turn(get_next_player_index());
    resolve_game_state();
}

void GameState::player_action_resign(int resigneeIndex) {
    assert(check_if_player_is_allowed_to_resign(resigneeIndex));
    std::optional<int> creditor;
    for (auto debtIt = pendingDebtSettlements.begin(); debtIt != pendingDebtSettlements.end();) {
        if (debtIt->debtor == resigneeIndex) {
            if (debtIt->creditor) {
                creditor = debtIt->creditor;
                pendingDebtSettlements.erase(debtIt++);
                continue;
            }
        }
        ++debtIt;
    }
    if (creditor) {
        force_bankrupt_by_player(resigneeIndex, *creditor);
    }
    else {
		force_bankrupt_by_bank(resigneeIndex);
    }
    if (check_if_player_is_allowed_to_auction_property (resigneeIndex)) {
        player_action_auction_property(resigneeIndex);
    }
    if (check_if_player_is_allowed_to_decline_trade (resigneeIndex)) {
        player_action_decline_trade(resigneeIndex);
    }
    if (check_if_player_is_allowed_to_decline_bid(resigneeIndex)) {
        player_action_decline_bid(resigneeIndex);
    }
    if (activePlayerIndex == resigneeIndex) {
        force_finish_turn();
        force_start_turn(get_next_player_index());
    }
    players[resigneeIndex].eliminated = true;
	resolve_game_state();
}

void GameState::force_start_turn(int playerIndex) {
    ++turn;
    pendingRoll = true;
    activePlayerIndex = playerIndex;
    resolve_game_state();
}

void GameState::force_finish_turn() {
    doublesStreak = 0;
    pendingRoll = false;
    resolve_game_state();
}

void GameState::force_funds(int playerIndex, int funds) {
    players[playerIndex].funds = funds;
    std::cout << player_name(playerIndex) << " has $" << funds << std::endl;
}

void GameState::force_add_funds(int playerIndex, int funds) {
    players[playerIndex].funds += funds;
    std::cout << player_name(playerIndex) << " collects $" << funds << std::endl;
}

void GameState::force_subtract_funds(int playerIndex, int funds) {
    players[playerIndex].funds -= funds;
    std::cout << player_name(playerIndex) << " pays $" << funds << std::endl;

    if (players[playerIndex].funds < 0)
        force_liquidate_to_pay_bank_prompt(playerIndex, -players[playerIndex].funds);
}

void GameState::force_transfer_funds(int fromPlayerIndex, int toPlayerIndex, int funds) {
    if (fromPlayerIndex == toPlayerIndex)
        return;

    players[fromPlayerIndex].funds -= funds;
    players[toPlayerIndex].funds += funds;

    std::cout << player_name(fromPlayerIndex) << " -($" << funds << ")-> " << player_name(toPlayerIndex) << std::endl;

    if (players[fromPlayerIndex].funds < 0)
        force_liquidate_to_pay_player_prompt(fromPlayerIndex, toPlayerIndex, -players[fromPlayerIndex].funds);
}

void GameState::force_go_to_jail(int playerIndex) {
    force_position(playerIndex, Space::Jail);
    if (players[playerIndex].turnsRemainingInJail != MaxJailTurns)
        std::cout << player_name(playerIndex) << " went to jail!" << std::endl;
    players[playerIndex].turnsRemainingInJail = MaxJailTurns;
    force_finish_turn();
}

void GameState::force_pay_bail(int playerIndex) {
    force_subtract_funds(playerIndex, BailCost);
    force_leave_jail(playerIndex);
}

void GameState::force_leave_jail(int playerIndex) {
    players[playerIndex].turnsRemainingInJail = 0;
}

void GameState::force_roll(int playerIndex, std::pair<int, int> roll) {
    assert_valid_die_value(roll.first);
    assert_valid_die_value(roll.second);
    activePlayerIndex = playerIndex;
    lastDiceRoll = roll;

    std::cout << "Rolled " << roll.first << "," << roll.second << std::endl;

    if (roll.first == roll.second) {
        doublesStreak += 1;
    }
    else {
        doublesStreak = 0;
    }

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
                force_finish_turn();
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

    pendingRoll = doublesStreak > 0;
    int const sum = roll.first + roll.second;
    force_advance(playerIndex, sum);
}

void GameState::force_advance(int playerIndex, int dist) {
    auto const currentPos = players[playerIndex].position;

    if (advancing_will_pass_go(currentPos, dist))
        force_add_funds(playerIndex, GoSalary);

    force_land(playerIndex, add_distance(players[playerIndex].position, dist));
}

void GameState::force_advance_without_landing(int playerIndex, int dist) {
    auto const currentPos = players[playerIndex].position;

    if (advancing_will_pass_go(currentPos, dist))
        force_add_funds(playerIndex, GoSalary);

    force_position(playerIndex, add_distance(players[playerIndex].position, dist));
}

void GameState::force_advance_to(int playerIndex, Space space) {
    auto const currentPos = players[playerIndex].position;
    force_advance(playerIndex, distance(currentPos, space));
}

void GameState::force_advance_to_without_landing(int playerIndex, Space space) {
    auto const currentPos = players[playerIndex].position;
    force_advance_without_landing(playerIndex, distance(currentPos, space));
}

void GameState::force_land(int playerIndex, Space space) {
    activePlayerIndex = playerIndex;
    force_leave_jail(playerIndex);
    force_position(playerIndex, space);
    if (space_is_property(space))
    {
        auto const property = space_to_property(space);
        auto const ownerIndex = get_property_owner_index(property);
        if (ownerIndex == Player::None) {
            force_property_offer(playerIndex, property);
        }
        else if (ownerIndex != playerIndex) {
            auto const rent = calculate_rent(property);
            force_transfer_funds(playerIndex, ownerIndex, rent);
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
        default:
            // Nothing happens when landing on Go, or Jail, or FreeParking
            break;
        }
    }

    resolve_game_state();
}

void GameState::force_position(int playerIndex, Space space) {
    players[playerIndex].position = space;
}

void GameState::force_property_offer(int playerIndex, Property property) {
    if (bank.deeds.count(property)) {
        force_property_offer_prompt(playerIndex, property);
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
    std::cout << player_name(playerIndex) << " pays income tax (" << tax << ")" << std::endl;
    force_subtract_funds(playerIndex, tax);
}

void GameState::force_luxury_tax(int playerIndex) {
    auto const tax = 100;
    std::cout << player_name(playerIndex) << " pays luxury tax (" << tax << ")" << std::endl;
    force_subtract_funds(playerIndex, tax);
}

void GameState::force_give_deed(int playerIndex, Property deed) {
    auto const countErased = bank.deeds.erase(deed);
    assert(countErased == 1);
    auto const insertRet = players[playerIndex].deeds.insert(deed);
    assert(insertRet.second);
}

void GameState::force_give_deeds(int playerIndex, std::set<Property> deeds) {
    for (auto deed : deeds)
        force_give_deed(playerIndex, deed);
}

void GameState::force_transfer_deed(int fromPlayerIndex, int toPlayerIndex, Property deed) {
    assert(get_building_level(deed) == 0);
    auto const countErased = players[fromPlayerIndex].deeds.erase(deed);
    assert(countErased == 1);
    auto const insertRet = players[toPlayerIndex].deeds.insert(deed);
    assert(insertRet.second);
    if (get_property_is_mortgaged(deed)) {
        force_subtract_funds(toPlayerIndex, calculate_closing_costs_on_sale (deed));
    }
}

void GameState::force_transfer_deeds(int fromPlayerIndex, int toPlayerIndex, std::set<Property> deeds) {
    for (auto deed : deeds)
        force_transfer_deed(fromPlayerIndex, toPlayerIndex, deed);
}

void GameState::force_set_mortgaged(Property property, bool mortgaged) {
    if (mortgaged) {
        mortgagedProperties.insert(property);
    }
    else {
        mortgagedProperties.erase(property);
    }
}

void GameState::force_sell_all_buildings(int playerIndex, PropertyGroup group) {
    auto const properties = properties_in_group(group);
    for (auto property : properties) {
        auto &buildingLevel = buildingLevels[property];
        auto const ownerIndex = get_property_owner_index(property);
        assert(ownerIndex != Player::None);
        if (ownerIndex == Player::None)
            continue;
        force_add_funds(ownerIndex, buildingLevel * sell_price_per_house_on_property(property));
        if (buildingLevel == HotelLevel) {
            bank.hotels += 1;
        }
        if (buildingLevel < HotelLevel) {
            bank.houses += buildingLevel;
        }
        buildingLevel = 0;
    }
    resolve_game_state();
}

void GameState::force_add_building(Property property) {
    auto& buildingLevel = buildingLevels[property];
    ++buildingLevel;
    if (buildingLevel < HotelLevel) {
        assert(bank.houses > 0);
        bank.houses -= 1;
    }
    if (buildingLevel == HotelLevel) {
        assert(bank.hotels > 0);
        bank.hotels -= 1;
        bank.houses += HotelLevel - 1;
    }
    assert(buildingLevel <= HotelLevel);
}

void GameState::force_remove_building(Property property) {
    auto& buildingLevel = buildingLevels[property];
    if (buildingLevel < HotelLevel) {
        bank.houses += 1;
    }
    if (buildingLevel == HotelLevel) {
        assert(bank.houses > 0);
        bank.hotels += 1;
        bank.houses -= HotelLevel - 1;
    }
    --buildingLevel;
    assert(buildingLevel >= 0);
}

void GameState::force_set_building_levels(std::map<Property, int> newBuildingLevels) {
    Property property;
    int desiredBuildingLevel;
    for (auto const &pair : newBuildingLevels) {
        std::tie(property, desiredBuildingLevel) = pair;
        while (get_building_level(property) > desiredBuildingLevel) {
            force_remove_building(property);
        }
        while (get_building_level(property) < desiredBuildingLevel) {
            force_add_building(property);
        }
    }
}

void GameState::force_give_get_out_of_jail_free_card(int playerIndex, DeckType deckType) {
    auto& deck = decks[deckType];
    deck.remove_card(get_out_of_jail_free_card(deckType));
    players[playerIndex].getOutOfJailFreeCards.insert(deckType);
}

void GameState::force_transfer_get_out_of_jail_free_card(int fromPlayerIndex, int toPlayerIndex, DeckType deckType) {
    auto& fromCards = players[fromPlayerIndex].getOutOfJailFreeCards;
    auto& toCards = players[toPlayerIndex].getOutOfJailFreeCards;
    if (fromCards.erase(deckType))
        toCards.insert(deckType);
}

void GameState::force_transfer_get_out_of_jail_free_cards(int fromPlayerIndex, int toPlayerIndex, std::set<DeckType> deckTypes) {
    for (auto deckType : deckTypes) {
        force_transfer_get_out_of_jail_free_card(fromPlayerIndex, toPlayerIndex, deckType);
    }
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

void GameState::force_bankrupt_by_bank(int debtorPlayerIndex) {
    auto& debtor = players[debtorPlayerIndex];
    for (auto deed : debtor.deeds) {
        if (get_building_level(deed) > 0) {
            force_sell_all_buildings(debtorPlayerIndex, property_group(deed)); // selling all at once guarantees tearing down evenly
        }
    }
    for (auto property : debtor.deeds) {
        propertiesPendingAuction.push(property);
    }
    debtor.funds = 0;
    bank.deeds.insert(debtor.deeds.begin(), debtor.deeds.end());
    debtor.deeds.clear();
    force_return_get_out_of_jail_free_cards(debtorPlayerIndex);
}

void GameState::force_bankrupt_by_player(int debtorPlayerIndex, int creditorPlayerIndex) {
    auto& debtor = players[debtorPlayerIndex];
    for (auto deed : debtor.deeds) {
        if (get_building_level(deed) > 0) {
            force_sell_all_buildings(debtorPlayerIndex, property_group(deed)); // selling all at once guarantees tearing down evenly
        }
    }
    force_transfer_funds(debtorPlayerIndex, creditorPlayerIndex, debtor.funds); // likely negative
    force_transfer_deeds(debtorPlayerIndex, creditorPlayerIndex, debtor.deeds);
    force_transfer_get_out_of_jail_free_cards(debtorPlayerIndex, creditorPlayerIndex);
}

void GameState::force_property_offer_prompt(int playerIndex, Property property) {
    std::cout << "Offering unowned property " << to_string(property) << std::endl;
    activePlayerIndex = playerIndex;
    pendingPurchaseDecision = true;
    resolve_game_state();
}

void GameState::force_liquidate_to_pay_bank_prompt(int debtorPlayerIndex, int amount) {
    Debt debt;
    debt.debtor = debtorPlayerIndex;
    debt.amount = amount;
    pendingDebtSettlements.push_back(debt);
}

void GameState::force_liquidate_to_pay_player_prompt(int debtorPlayerIndex, int creditorPlayerIndex, int amount) {
    Debt debt;
    debt.debtor = debtorPlayerIndex;
    debt.amount = amount;
    debt.creditor = creditorPlayerIndex;
    pendingDebtSettlements.push_back(debt);
}

void GameState::force_trade(Trade trade) {
    force_transfer_promise(trade.offeringPlayer, trade.consideringPlayer, trade.offer);
    force_transfer_promise(trade.consideringPlayer, trade.offeringPlayer, trade.consideration); // Seems a bit silly that you can trade cash for cash
}

void GameState::force_transfer_promise(int fromPlayerIndex, int toPlayerIndex, Promise promise) {
    force_transfer_funds(fromPlayerIndex, toPlayerIndex, promise.cash);
    force_transfer_deeds(fromPlayerIndex, toPlayerIndex, promise.deeds);
    force_transfer_get_out_of_jail_free_cards(fromPlayerIndex, toPlayerIndex, promise.getOutOfJailFreeCards);
}

Player GameState::init_player(GameSetup const& setup) {
    Player p;
    p.funds = setup.startingFunds;
    return p;
}

std::vector<Player> GameState::init_players(GameSetup const& setup) {
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

void GameState::resolve_game_state() {
    if (get_players_remaining_count () < 2) {
        phase = TurnPhase::GameOver;
    }
    else if (pendingTradeAgreement.has_value ()) {
        phase = TurnPhase::WaitingForTradeOfferResponse;
    }
    else if (!pendingDebtSettlements.empty()) {
        resolve_debt_settlements();
    }
    else if (currentAuction) {
        resolve_auction();
    }
    else if (pendingAuctionSale.has_value()) {
        resolve_auction_sale();
    }
    else if (!propertiesPendingAuction.empty()) {
        resolve_queued_auction(propertiesPendingAuction.front());
    }
    else if (!pendingAcquisitions.empty()) {
        phase = TurnPhase::WaitingForAcquisitionManagement;
    }
    else if (pendingPurchaseDecision) {
        phase = TurnPhase::WaitingForBuyPropertyInput;
    }
    else if (pendingRoll) {
        phase = TurnPhase::WaitingForRoll;
    }
    else if (activePlayerIndex != get_next_player_index ()) {
        phase = TurnPhase::WaitingForTurnEnd;
    }
}

void GameState::resolve_debt_settlements() {
    assert(! pendingDebtSettlements.empty ());
    auto const debt = pendingDebtSettlements.front();

    if (players[debt.debtor].funds >= 0) {
        pendingDebtSettlements.pop_front();
        resolve_game_state();
    }
    else {
        phase = TurnPhase::WaitingForDebtSettlement;
    }
}

void GameState::resolve_auction() {
    auto const highestBidderIndex = currentAuction.biddingOrder.back();
    auto const nextBidderIndex = currentAuction.biddingOrder.front();
    std::cout << to_string (currentAuction.property) << " going to " <<
        player_name(highestBidderIndex) << " for $" << currentAuction.highestBid << std::endl;
    phase = TurnPhase::WaitingForBids;
    if (currentAuction.biddingOrder.size() > 1) {
        if (check_if_player_is_allowed_to_bid(nextBidderIndex, currentAuction.highestBid + 1)) {
            std::cout << player_name(nextBidderIndex) << ": Bid or decline?" << std::endl;
        }
        else {
            std::cout << player_name(nextBidderIndex) << " can't afford to bid more than " << currentAuction.highestBid << std::endl;
            player_action_decline_bid(nextBidderIndex);
            resolve_game_state();
        }
    }
    else {
        auto const payment = currentAuction.highestBid + calculate_closing_costs_on_sale(currentAuction.property);
        pendingAuctionSale = { highestBidderIndex, currentAuction.property };
        currentAuction = {};
        force_subtract_funds(highestBidderIndex, payment);
        resolve_game_state();
    }
}

void GameState::resolve_auction_sale() {
    assert(pendingAuctionSale.has_value());
    auto const highestBidderIndex = pendingAuctionSale->first;
    auto const property = pendingAuctionSale->second;

    assert(pendingDebtSettlements.empty());
    if (! players[highestBidderIndex].eliminated) {
        force_give_deed(highestBidderIndex, property);
    }
    else {
        propertiesPendingAuction.push(property);
    }
    pendingAuctionSale = {};
    resolve_game_state();
}

void GameState::resolve_queued_auction(Property property) {
    std::cout << "Auctioning property" << to_string (property) << std::endl;
    currentAuction = Auction{};
    // Determine order of auction
    for (int i = get_next_player_index(activePlayerIndex); i != activePlayerIndex; i = get_next_player_index(i)) {
        currentAuction.biddingOrder.push_back(i);
    }
    // Active player starts with a bid of 0, so they go last
    currentAuction.biddingOrder.push_back(activePlayerIndex);
    currentAuction.highestBid = 0;
    currentAuction.property = property;
    propertiesPendingAuction.pop();
    resolve_game_state();
}
