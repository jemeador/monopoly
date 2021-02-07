#include "Interface.h"
#include "Game.h"
using namespace monopoly;

#include <iostream>
#include <vector>
using namespace std;

void init_game()
{
	auto interface = make_interface("terminal");
	auto game = Game(interface.get ());
}

int main(int argc, char** argv)
{
	init_game();
}

