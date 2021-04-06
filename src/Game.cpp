#include"Game.h"
#include"IInterface.h"
using namespace monopoly;

#include<chrono>
#include<iostream>
using namespace std::chrono_literals;


namespace
{

template<typename INPUT>
void process_input(GameState& state, int playerIndex, INPUT const& input);

template<>
void process_input(GameState &state, int playerIndex, RollInput const& input) {
    if (state.check_if_player_is_allowed_to_roll(playerIndex)) {
        state.player_action_roll(playerIndex);
    }
}

template<>
void process_input(GameState &state, int playerIndex, BuyPropertyInput const& input) {
    if (state.check_if_player_is_allowed_to_buy_property(playerIndex)) {
        state.player_action_buy_property(playerIndex);
    }
}

template<>
void process_input(GameState &state, int playerIndex, AuctionPropertyInput const& input) {
    if (state.check_if_player_is_allowed_to_auction_property(playerIndex)) {
        state.player_action_auction_property(playerIndex);
    }
}

template<>
void process_input(GameState &state, int playerIndex, UnmortgagePropertiesInput const& input) {
    for (auto property : input.properties) {
        if (!state.check_if_player_is_allowed_to_unmortgage(playerIndex, property)) {
            return;
        }
    }
    for (auto property : input.properties) {
        state.player_action_unmortgage(playerIndex, property);
    }
}

template<>
void process_input(GameState &state, int playerIndex, MortgagePropertiesInput const& input) {
    for (auto property : input.properties) {
        if (!state.check_if_player_is_allowed_to_mortgage(playerIndex, property)) {
            return;
        }
    }
    for (auto property : input.properties) {
        state.player_action_mortgage(playerIndex, property);
    }
}

template<>
void process_input(GameState &state, int playerIndex, BuyBuildingInput const& input) {
    if (state.check_if_player_is_allowed_to_buy_building(playerIndex, input.property)) {
        state.player_action_buy_building(playerIndex, input.property);
    }
}

template<>
void process_input(GameState &state, int playerIndex, SellBuildingInput const& input) {
    if (state.check_if_player_is_allowed_to_sell_building(playerIndex, input.property)) {
        state.player_action_sell_building(playerIndex, input.property);
    }
}

template<>
void process_input(GameState &state, int playerIndex, SellAllBuildingsInput const& input) {
    if (state.check_if_player_is_allowed_to_sell_all_buildings(playerIndex, input.group)) {
        state.player_action_sell_all_buildings(playerIndex, input.group);
    }
}

template<>
void process_input(GameState &state, int playerIndex, UseGetOutOfJailFreeCardInput const& input) {
    if (state.check_if_player_is_allowed_to_use_get_out_jail_free_card(playerIndex)) {
        state.player_action_use_get_out_of_jail_free_card(playerIndex, input.preferredDeckType);
    }
}

template<>
void process_input(GameState &state, int playerIndex, PayBailInput const& input) {
    if (state.check_if_player_is_allowed_to_pay_bail(playerIndex)) {
        state.player_action_pay_bail(playerIndex);
    }
}

template<>
void process_input(GameState &state, int playerIndex, BidInput const& input) {
    if (state.check_if_player_is_allowed_to_bid(playerIndex, input.amount)) {
        state.player_action_bid(playerIndex, input.amount);
    }
}

template<>
void process_input(GameState &state, int playerIndex, DeclineBidInput const& input) {
    if (state.check_if_player_is_allowed_to_decline_bid(playerIndex)) {
        state.player_action_decline_bid(playerIndex);
    }
}

template<>
void process_input(GameState &state, int playerIndex, OfferTradeInput const& input) {
    auto const trade = trade_from_offer_input(playerIndex, input);
    if (state.check_if_trade_is_valid(trade)) {
        state.player_action_offer_trade(trade);
    }
}

template<>
void process_input(GameState &state, int playerIndex, DeclineTradeInput const& input) {
    if (state.check_if_player_is_allowed_to_decline_trade(playerIndex)) {
        state.player_action_decline_trade(playerIndex);
    }
}

template<>
void process_input(GameState &state, int playerIndex, EndTurnInput const& input) {
    if (state.check_if_player_is_allowed_to_end_turn(playerIndex)) {
        state.player_action_end_turn(playerIndex);
    }
}

template<>
void process_input(GameState &state, int playerIndex, ResignInput const& input) {
    if (state.check_if_player_is_allowed_to_resign(playerIndex)) {
        state.player_action_resign(playerIndex);
    }
}

}


Game::~Game()
{
    stop();
}

Game::Game(IInterface* interface)
    : interface(interface)
    , setup(interface->get_setup())
    , currentCycle(0)
    , state(setup)
{
    start();
}

GameState Game::get_state() const {
    return state;
}

void Game::set_state(GameState newState) {
    currentCycle = 0;
    std::swap(state, newState);
}

void Game::reset() {
    stop();
    start();
}

void Game::process() {
    process_inputs();
    interface->update(state);
    currentCycle++;
}

void Game::process_inputs() {
    auto playerInputQueue = interface->poll();
    int playerIndex;
    Input input;
    while (!playerInputQueue.empty()) {
        tie(playerIndex, input) = playerInputQueue.front();
        process_input(playerIndex, input);
        playerInputQueue.pop();
    }
}

void Game::process_input(int playerIndex, Input const& input) {
    std::visit([=](auto&& i) { ::process_input(state, playerIndex, i); }, input);
}

void Game::start() {
    std::cout << "Starting a new game" << std::endl;
    GameState newState(interface->get_setup());
    currentCycle = 0;
    std::swap(state, newState);
}

void Game::stop() {
    std::cout << "Game over" << std::endl;
}
