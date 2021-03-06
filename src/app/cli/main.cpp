#include "CommandLineInterface.h"
#include "Game.h"
using namespace monopoly;

#include <iostream>
#include <vector>
using namespace std;

int main(int argc, char** argv)
{
    auto interface = CommandLineInterface();
    auto game = Game(&interface);

    while (!game.get_state().is_game_over ()) {
        game.process();
    }
}

