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

        // Process inputs until further player intervention is required
        void process();

    private:
        void process_inputs();

        void process_input(int playerIndex, Input const& input);

        void start();
        void stop();

        IInterface* const interface;
        GameSetup const setup;

        int currentCycle;
        GameState state;
    };
}
