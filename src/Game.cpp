#include"Game.h"
#include"IInterface.h"
using namespace monopoly;

#include<chrono>
#include<iostream>
#include<tuple>
using namespace std::chrono_literals;

Game::~Game()
{
    stop();
}

Game::Game(IInterface* interface)
    : interface(interface)
    , setup(interface->get_setup())
    , state(setup)
    , gameOver(false)
    , currentCycle(0)
{
    start();
}

GameState Game::get_state() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return state;
}

void Game::set_state(GameState newState) {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentCycle = 0;
    std::swap(state, newState);
}

void Game::reset() {
    stop();
    start();
}

void Game::wait_for_processing() {
    std::unique_lock<std::mutex> lock(stateMutex);
    auto cycle = currentCycle;
    waitCondition.wait(lock, [cycle, this] {return cycle != currentCycle;});
}

bool Game::game_over() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return gameOver;
}
void Game::process() {
    while (!gameOver && gameEndFuture.wait_for(33ms) == std::future_status::timeout) { // ~30Hz
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            process_inputs();
            interface->update(state);
            auto const playersRemaining = state.get_players_remaining_count();
            if (playersRemaining < 2)
                gameOver = true;
        }
        waitCondition.notify_all();
        currentCycle++;
    }
}

void Game::process_inputs() {
    auto playerInputQueue = interface->poll();
    int playerIndex;
    Input input;
    while (!playerInputQueue.empty()) {
    std:tie(playerIndex, input) = playerInputQueue.front();
        process_input(playerIndex, input);
        playerInputQueue.pop();
    }
}

void Game::process_input(int playerIndex, Input const& input) {
    if (state.get_controlling_player_index() != playerIndex) {
        return;
    }

    if (auto inputPtr = std::get_if<RollInput>(&input)) {
        process_roll_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<BuyPropertyInput>(&input)) {
        process_buy_property_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<BuyBuildingInput>(&input)) {
        process_buy_building_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<SellBuildingInput>(&input)) {
        process_sell_building_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<SellAllBuildingsInput>(&input)) {
        process_sell_all_buildings_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<UnmortgagePropertiesInput>(&input)) {
        process_unmortgage_properties_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<MortgagePropertiesInput>(&input)) {
        process_mortgage_properties_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<UseGetOutOfJailFreeCardInput>(&input)) {
        process_use_get_out_of_jail_free_card_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<PayBailInput>(&input)) {
        process_pay_bail_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<BidInput>(&input)) {
        process_bid_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<DeclineBidInput>(&input)) {
        process_decline_bid_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<OfferTradeInput>(&input)) {
        process_offer_trade_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<DeclineTradeInput>(&input)) {
        process_decline_trade_input(playerIndex, *inputPtr);
    }
    else if (auto inputPtr = std::get_if<EndTurnInput>(&input)) {
        process_end_turn_input(playerIndex, *inputPtr);
    }
}

void Game::process_roll_input(int playerIndex, RollInput const& input) {
    if (!state.check_if_player_is_allowed_to_roll(playerIndex)) {
        return;
    }

    if (setup.loadedDiceEnabled && input.loadedDiceValues != std::pair<int, int>{0, 0}) {
        auto diceValues = input.loadedDiceValues;
        clamp_die_value(diceValues.first);
        clamp_die_value(diceValues.second);
        state.force_roll(playerIndex, diceValues);
    }
    else {
        state.force_random_roll(playerIndex);
    }
}

void Game::process_buy_property_input(int playerIndex, BuyPropertyInput const& input) {
    if (!state.check_if_player_is_allowed_to_buy_property(playerIndex)) {
        return;
    }

    auto const& player = state.get_player(playerIndex);
    auto const space = player.position;
    assert(space_is_property(space));
    auto const property = space_to_property(space);
    auto const& bank = state.get_bank();
    assert(bank.deeds.count(property));
    if (input.option == BuyPropertyOption::Buy) {
        state.force_property_buy(playerIndex, property);
    }
    else {
        state.force_property_auction(property);
    }
}

void Game::process_unmortgage_properties_input(int playerIndex, UnmortgagePropertiesInput const& input) {
    for (auto property : input.properties) {
        if (!state.check_if_player_is_allowed_to_unmortgage(playerIndex, property)) {
            return;
        }
    }
    for (auto property : input.properties) {
        state.force_unmortgage(property);
    }
}

void Game::process_mortgage_properties_input(int playerIndex, MortgagePropertiesInput const& input) {
    for (auto property : input.properties) {
        if (!state.check_if_player_is_allowed_to_mortgage(playerIndex, property)) {
            return;
        }
    }
    for (auto property : input.properties) {
        state.force_mortgage(property);
    }
}

void Game::process_buy_building_input(int playerIndex, BuyBuildingInput const& input) {
    if (!state.check_if_player_is_allowed_to_buy_building(playerIndex, input.property)) {
        return;
    }

    state.force_buy_building(input.property);
}

void Game::process_sell_building_input(int playerIndex, SellBuildingInput const& input) {
    if (!state.check_if_player_is_allowed_to_sell_building(playerIndex, input.property)) {
        return;
    }

    state.force_sell_building(input.property);
}

void Game::process_sell_all_buildings_input(int playerIndex, SellAllBuildingsInput const& input) {
    if (!state.check_if_player_is_allowed_to_sell_all_buildings(playerIndex, input.group)) {
        return;
    }

    state.force_sell_all_buildings(input.group);
}

void Game::process_use_get_out_of_jail_free_card_input(int playerIndex, UseGetOutOfJailFreeCardInput const& input) {
    if (!state.check_if_player_is_allowed_to_use_get_out_jail_free_card(playerIndex)) {
        return;
    }

    state.force_use_get_out_of_jail_free_card(playerIndex, input.preferredDeckType);
}

void Game::process_pay_bail_input(int playerIndex, PayBailInput const& input) {
    if (!state.check_if_player_is_allowed_to_pay_bail(playerIndex)) {
        return;
    }

    state.force_pay_bail(playerIndex);
}

void Game::process_bid_input(int playerIndex, BidInput const& input) {
    if (!state.check_if_player_is_allowed_to_bid(playerIndex, input.amount)) {
        return;
    }

    state.force_bid(playerIndex, input.amount);
}

void Game::process_decline_bid_input(int playerIndex, DeclineBidInput const& input) {
    if (!state.check_if_player_is_allowed_to_decline_bid(playerIndex)) {
        return;
    }

    state.force_decline_bid(playerIndex);
}

void Game::process_offer_trade_input(int playerIndex, OfferTradeInput const& input) {
    Trade trade;
    trade.offeringPlayer = playerIndex;
    trade.offer = input.offer;
    trade.consideringPlayer = input.consideringPlayer;
    trade.consideration = input.consideration;
    if (!state.check_if_trade_is_valid(trade)) {
        return;
    }

    state.force_offer_trade(trade);
}

void Game::process_decline_trade_input(int playerIndex, DeclineTradeInput const& input) {
    if (!state.check_if_player_is_allowed_to_decline_trade(playerIndex)) {
        return;
    }

    state.force_decline_trade(playerIndex);
}

void Game::process_end_turn_input(int playerIndex, EndTurnInput const& input) {
    if (state.get_turn_phase() != TurnPhase::WaitingForTurnEnd) {
        return;
    }
    if (state.get_active_player_index() != playerIndex) {
        return;
    }
    state.force_turn_start(state.get_next_player_index());
}

void Game::start() {
    std::cout << "Starting a new game" << std::endl;
    std::promise<void> newPromise;
    swap(gameEndPromise, newPromise);
    gameOver = false;
    gameEndFuture = gameEndPromise.get_future();
    gameThread = std::thread(&Game::process, this);

    {
        std::lock_guard<std::mutex> lock(stateMutex);
        GameState newState(interface->get_setup());
        currentCycle = 0;
        std::swap(state, newState);
    }
}

void Game::stop() {
    gameEndPromise.set_value();
    gameThread.join();
    gameOver = true;
    std::cout << "Game over" << std::endl;
}
