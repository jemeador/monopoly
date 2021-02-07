#pragma once
#include"GameState.h"
#include"Input.h"

#include<atomic>
#include<condition_variable>
#include<future>
#include<memory>
#include<mutex>
#include<queue>
#include<thread>
#include<variant>
#include<vector>

namespace monopoly
{
	class IInterface;
	class Game
	{
	public:
		~Game();
		Game(IInterface* interface);

		GameState get_state() const;
		void set_state(GameState state);

		// Game is started over with the same setup
		void reset();

		// Blocks the calling thread until all input from the interfaces has been processed
		void wait_for_processing();

		// Must be called before forcing things from outside the class. Hold the lock until you are done modifying state
		std::unique_lock<std::mutex> pause_processing();

	private:
		void process();
		void process_input(int playerIndex, Input const &input);
		void process_roll_input(int playerIndex, RollInput const &input);
		void process_buy_property_input(int playerIndex, BuyPropertyInput const &input);
		void process_buy_houses_input(int playerIndex, BuyHousesInput const &input);
		void process_sell_houses_input(int playerIndex, SellHousesInput const &input);
		void process_unmortgage_properties_input(int playerIndex, UnmortgagePropertiesInput const &input);
		void process_mortgage_properties_input(int playerIndex, MortgagePropertiesInput const &input);
		void process_use_get_out_of_jail_free_card_input(int playerIndex, UseGetOutOfJailFreeCardInput const &input);
		void process_bid_input(int playerIndex, BidInput const &input);
		void process_offer_trade_input (int playerIndex, OfferTradeInput const &input);

		void start();
		void stop();

		std::pair<int, int> random_dice_roll();

		IInterface * const interface;
		GameSetup const setup;

		// Handles the game thread
		std::promise<void> gameEndPromise;
		std::future<void> gameEndFuture;
		std::thread gameThread;
		std::queue<std::pair<int, Input>> playerInputQueue;

		// Used to wait for processing on the thread to finish
		std::mutex mutable stateMutex;
		std::condition_variable waitCondition;

		// Randomizers
		std::random_device randomDevice;
		std::mt19937 randomNumberGenerator;
		std::uniform_int_distribution<int> rollDie;

		int currentCycle;
		GameState state;
	};
}
