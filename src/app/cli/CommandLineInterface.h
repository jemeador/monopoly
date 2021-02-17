#pragma once

#include "IInterface.h"
#include "Strings.h"

#include <mutex>
#include <queue>
#include <iostream>

namespace monopoly
{
    class CommandLineInterface final :
        public IInterface
    {
    public:
        CommandLineInterface(bool automate = true)
            : IInterface()
            , inputMutex()
            , inputBuffer()
            , automate(automate) {
        }

        GameSetup get_setup() final {
            GameSetup setup;
            setup.playerCount = 2;
            return setup;
        }

        std::queue<PlayerIndexInputPair> poll() final {
            std::lock_guard<std::mutex> lock(inputMutex);
            std::queue<PlayerIndexInputPair> ret;
            swap(ret, inputBuffer);
            return ret;
        }

        void update(GameState newState) final {
            state = newState;
            update_prompt();
            std::swap(prevState, state);
        }

    private:
        void update_prompt() {
            if (prompt_change_required()) {
                switch (state.get_turn_phase()) {
                case TurnPhase::WaitingForRoll:
                    show_waiting_for_roll_prompt();
                    break;
                case TurnPhase::WaitingForBuyPropertyInput:
                    show_waiting_for_buy_property_prompt();
                    break;
                case TurnPhase::WaitingForTurnEnd:
                    show_waiting_for_turn_end_prompt();
                    break;
                }
                initialized = true;
            }
        }

        bool prompt_change_required() const {
            return state != prevState;
        }

        void show_waiting_for_roll_prompt() {
            auto const activePlayerIndex = state.get_active_player_index();
            auto const activePlayer = state.get_player(activePlayerIndex);
            auto options = defaultOptions + rollOption;
            if (activePlayer.turnsRemainingInJail > 0) {
                if (!activePlayer.getOutOfJailFreeCards.empty()) {
                    options.push_back(gojfOption);
                }
                options.push_back(bailOption);
            }
            get_input(activePlayerIndex, options);
        }

        void show_waiting_for_buy_property_prompt() {
            auto const activePlayerIndex = state.get_active_player_index();
            auto options = defaultOptions + buyOption + auctionOption;
            get_input(activePlayerIndex, options);
        }

        void show_waiting_for_turn_end_prompt() {
            auto const activePlayerIndex = state.get_active_player_index();
            auto options = defaultOptions + endTurnOption;
            get_input(activePlayerIndex, options);
        }


        static const auto rollOption = 'r';
        static const auto manageOption = 'm';
        static const auto tradeOption = 't';
        static const auto bailOption = 'p';
        static const auto gojfOption = 'g';

        static const auto buyOption = 'b';
        static const auto auctionOption = 'a';
        static const auto endTurnOption = 'e';

        static const auto quitOption = 'q';
        static const auto showBoardOption = 's';

        std::string const defaultOptions = "sq";

        inline static const auto optionText = std::map<char, char const*>{
            {rollOption, "[r]oll" },
            {manageOption, "[m]anage properties" },
            {tradeOption, "[t]rade" },
            {bailOption, "[p]ay bail ($50)" },
            {gojfOption, "[g]et out of jail free" },
            {buyOption, "[b]uy" },
            {auctionOption, "[a]uction" },
            {endTurnOption, "[e]nd turn" },
            {quitOption, "[q]uit" },
            {showBoardOption, "[s]how board" },
        };

        void show_prompted_player(int activePlayer) {
            std::cout << player_name(activePlayer) << ":\n";
        }

        void show_options(std::string const& options) {
            for (auto o : options) {
                std::cout << "\t" << optionText.at(o) << "\n";
            }
        }

        void get_input(int playerIndex, std::string const& options) {
            bool validInput = false;
            while (!validInput) {
                if (automate) {
                    show_board();
                }
                show_prompted_player(playerIndex);
                show_options(options);
                char input;
                if (automate) {
                    input = auto_input(options);
                }
                else {
                    std::cin >> input;
                }
                if (options.find(input) != std::string::npos) {
                    send_input(playerIndex, input);
                    validInput = true;
                }
                else {
                    std::cin.ignore('\n');
                    std::cout << '\n';
                }
            }
        }

        char auto_input(std::string const& options) {
            static auto const turnLimit = 1000;
            if (state.get_turn() > turnLimit) {
                std::cout << "Turn limit exceeded (" << turnLimit << ')\n';
                return quitOption;
            }
            std::string const optionPriorities = "rbe";
            for (auto o : optionPriorities) {
                if (options.find(o) != std::string::npos) {
                    std::cout << o << '\n';
                    return o;
                }
            }
        }

        void send_input(int activePlayer, char input) {
            switch (input) {
            case rollOption:
                queue_roll_input(activePlayer);
                break;
            case manageOption:
                break;
            case tradeOption:
                break;
            case bailOption:
                queue_pay_bail_input(activePlayer);
                break;
            case gojfOption:
                queue_use_get_out_of_jail_free_card_input(activePlayer);
                break;
            case buyOption:
                queue_purchase_property_input(activePlayer, BuyPropertyOption::Buy);
                break;
            case auctionOption:
                queue_purchase_property_input(activePlayer, BuyPropertyOption::Auction);
                break;
            case endTurnOption:
                queue_end_turn_input(activePlayer);
                break;
            case showBoardOption:
                show_board();
                update_prompt();
                break;
            case quitOption:
                exit(0);
                break;
            }
        }

        void queue_roll_input(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, RollInput {} });
        }
        void queue_purchase_property_input(int playerIndex, BuyPropertyOption option) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BuyPropertyInput { option } });
        }
        void queue_use_get_out_of_jail_free_card_input(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UseGetOutOfJailFreeCardInput {} });
        }
        void queue_pay_bail_input(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, PayBailInput {} });
        }
        void queue_end_turn_input(int playerIndex) {
            std::lock_guard<std::mutex> lock(inputMutex);
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, EndTurnInput {} });
        }

        void show_board() {

            for (auto p = 1; p <= state.get_player_count(); ++p) {
                std::cout << p;
            }
            std::cout << " Buildings O\n";

            auto bl = state.get_building_levels();
            for (auto s = 0; s < NumberOfSpaces; ++s) {

                auto const space = static_cast<Space> (s);
                for (auto p = 0; p < state.get_player_count(); ++p) {
                    bool const playerIsAtSpace = (state.get_player(p).position == space);
                    std::cout << (playerIsAtSpace ? "." : " ");
                }
                if (space_is_property(space)) {
                    auto const property = space_to_property(space);
                    if (property_is_in_group(property, PropertyGroup::Railroad)) {
                        auto const ownedGroupCount = state.get_properties_owned_in_group(property);
                        for (auto railroadSlot = 0; railroadSlot < 4; ++railroadSlot) {
                            bool const slotFilled = ownedGroupCount > railroadSlot;
                            std::cout << (slotFilled ? "R " : "_ ");
                        }
                        std::cout << "  ";
                    }
                    else if (property_is_in_group(property, PropertyGroup::Utility)) {
                        auto const ownedGroupCount = state.get_properties_owned_in_group(property);
                        for (auto utilitySlot = 0; utilitySlot < 2; ++utilitySlot) {
                            bool const slotFilled = ownedGroupCount > utilitySlot;
                            std::cout << (slotFilled ? "U " : "_ ");
                        }
                        std::cout << "      ";
                    }
                    else {
                        bool const buildingLevel = bl[space_to_property(space)];
                        for (auto houseSlot = 0; houseSlot < 5; ++houseSlot) {
                            bool const houseIsBuilt = (buildingLevel > houseSlot);
                            std::cout << (houseIsBuilt ? "# " : "_ ");
                        }
                    }
                    if (auto const ownerOpt = state.get_property_owner_index(property)) {
                        std::cout << *ownerOpt + 1 << " ";
                    }
                    else {
                        std::cout << "* ";
                    }
                }
                else {
                    std::cout << "            ";
                }
                std::cout << to_string(space) << "\n";
            }
            for (auto p = 0; p < state.get_player_count(); ++p) {
                std::cout << player_name(p) << ": $" << state.get_player(p).funds << "\n";
            }
        }

        bool initialized = false;
        GameState state;
        GameState prevState;
        std::mutex inputMutex;
        std::queue<PlayerIndexInputPair> inputBuffer;
        bool automate;
    };
}