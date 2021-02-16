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

        void mortgage_properties(int playerIndex, std::set<Property> properties) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, MortgagePropertiesInput{ properties } });
        }

        void unmortgage_property(int playerIndex, Property property) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UnmortgagePropertiesInput{ {property} } });
        }

        void unmortgage_properties(int playerIndex, std::set<Property> properties) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UnmortgagePropertiesInput{ properties } });
        }

        void buy_building(int playerIndex, Property property) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BuyBuildingInput{ property } });
        }

        void sell_building(int playerIndex, Property property) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, SellBuildingInput{ property } });
        }

        void use_get_out_of_jail_free_card(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UseGetOutOfJailFreeCardInput {} });
        }

        void pay_bail(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, PayBailInput {} });
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
