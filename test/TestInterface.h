#pragma once

#include "IInterface.h"

#include <queue>

namespace monopoly
{
    class TestInterface final :
        public SimpleInterface
    {
    public:
        TestInterface()
            : SimpleInterface() {
        }

        GameSetup get_setup() final {
            GameSetup setup;
            setup.loadedDiceEnabled = true;
            return setup;
        }

        void update(GameState state) final {
        }
    };
}
