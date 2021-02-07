#pragma once

#include "IInterface.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

namespace monopoly
{
    class TestInterface :
        public IInterface
    {
    public:
        TestInterface()
            : IInterface()
            , inputMutex()
            , inputBuffer() {
        }

        void roll_loaded_dice(int playerIndex, std::pair<int, int> diceValues) {
            queue_roll_input(playerIndex, diceValues);
        }

        void buy_property(int playerIndex) {
            queue_purchase_property_input(playerIndex, BuyPropertyOption::Buy);
        }

        void auction_property(int playerIndex) {
            queue_purchase_property_input(playerIndex, BuyPropertyOption::Auction);
        }

        void use_get_out_of_jail_free_card(int playerIndex) {
            queue_use_get_out_of_jail_free_card_input(playerIndex);
        }

        void pay_bail(int playerIndex) {
            queue_pay_bail_input(playerIndex);
        }

        bool has_input () {
            std::lock_guard<std::mutex> lock(inputMutex);
            return ! inputBuffer.empty();
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

		void prompt_buy_at_list_price(int playerIndex) final {
        }

    private:
        void queue_roll_input(int playerIndex, std::pair<int, int> diceValues) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, RollInput { diceValues }});
        }
        void queue_purchase_property_input(int playerIndex, BuyPropertyOption option) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BuyPropertyInput { option }});
        }
        void queue_use_get_out_of_jail_free_card_input(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UseGetOutOfJailFreeCardInput {}});
        }
        void queue_pay_bail_input(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, PayBailInput {}});
        }

        std::mutex inputMutex;
		std::queue<PlayerIndexInputPair> inputBuffer;
    };
}
