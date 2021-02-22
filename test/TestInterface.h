#pragma once

#include "IInterface.h"

#include <queue>
#include <mutex>

namespace monopoly
{
    class TestInterface final :
        public IInterface
    {
    public:
        TestInterface()
            : IInterface()
            , inputMutex()
            , inputBuffer() {
        }

        void roll_dice(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, RollInput {} });
        }

        void roll_loaded_dice(int playerIndex, std::pair<int, int> diceValues) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, RollInput {diceValues} });
        }

        void buy_property(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BuyPropertyInput { BuyPropertyOption::Buy } });
        }

        void auction_property(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BuyPropertyInput {  BuyPropertyOption::Auction } });
        }

        void mortgage_property(int playerIndex, Property property) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, MortgagePropertiesInput{ {property} } });
        }

        void unmortgage_property(int playerIndex, Property property) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UnmortgagePropertiesInput{ {property} } });
        }

        void buy_building(int playerIndex, Property property) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BuyBuildingInput{ property } });
        }

        void sell_building(int playerIndex, Property property) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, SellBuildingInput{ property } });
        }

        void sell_all_buildings(int playerIndex, PropertyGroup group) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, SellAllBuildingsInput{ group } });
        }

        void use_get_out_of_jail_free_card(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UseGetOutOfJailFreeCardInput {} });
        }

        void pay_bail(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, PayBailInput {} });
        }

        void bid(int playerIndex, int bid) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BidInput {bid} });
        }

        void decline_bid(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, DeclineBidInput {} });
        }

        void propose_trade(Trade trade) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ trade.offeringPlayer, OfferTradeInput {trade.offer, trade.consideringPlayer, trade.consideration} });
        }

        void decline_trade(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, DeclineTradeInput {} });
        }

        void resign(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, ResignInput {} });
        }

        bool has_input() {
            std::lock_guard<std::mutex> lock(inputMutex);
            return !inputBuffer.empty();
        }

        GameSetup get_setup() final {
            GameSetup setup;
            setup.loadedDiceEnabled = true;
            return setup;
        }

        std::queue<PlayerIndexInputPair> poll() final {
            std::lock_guard<std::mutex> lock(inputMutex);
            std::queue<PlayerIndexInputPair> ret;
            swap(ret, inputBuffer);
            return ret;
        }

        void update(GameState state) final {
        }

    private:
        std::mutex inputMutex;
        std::queue<PlayerIndexInputPair> inputBuffer;
    };
}
