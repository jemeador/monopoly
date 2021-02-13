#pragma once

#include "Game.h"

#include <queue>

namespace monopoly
{
	class IInterface
	{
	public:
		virtual ~IInterface() = default;
		virtual GameSetup get_setup() = 0;
		// Get any queued inputs (player index, Input), buffered inputs should be cleared after polling
		virtual std::queue<PlayerIndexInputPair> poll() = 0;
		// Process changes in game state
		virtual void update(GameState state) = 0;

	};
}
