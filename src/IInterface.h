#pragma once

#include "Game.h"
#include "Input.h"

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

    class SimpleInterface : public IInterface {
        std::queue<PlayerIndexInputPair> inputBuffer;
    public:
        void roll_dice(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, RollInput {} });
        }

        void buy_property(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BuyPropertyInput {} });
        }

        void auction_property(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, AuctionPropertyInput {} });
        }

        void mortgage_property(int playerIndex, Property property) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, MortgagePropertiesInput{ {property} } });
        }

        void unmortgage_property(int playerIndex, Property property) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UnmortgagePropertiesInput{ {property} } });
        }

        void buy_building(int playerIndex, Property property) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BuyBuildingInput{ property } });
        }

        void sell_building(int playerIndex, Property property) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, SellBuildingInput{ property } });
        }

        void sell_all_buildings(int playerIndex, PropertyGroup group) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, SellAllBuildingsInput{ group } });
        }

        void use_get_out_of_jail_free_card(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, UseGetOutOfJailFreeCardInput {} });
        }

        void pay_bail(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, PayBailInput {} });
        }

        void bid(int playerIndex, int bid) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, BidInput {bid} });
        }

        void decline_bid(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, DeclineBidInput {} });
        }

        void propose_trade(Trade trade) {
            inputBuffer.push(PlayerIndexInputPair{ trade.offeringPlayer, OfferTradeInput {trade.offer, trade.consideringPlayer, trade.consideration} });
        }

        void decline_trade(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, DeclineTradeInput {} });
        }

        void end_turn(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, EndTurnInput {} });
        }

        void resign(int playerIndex) {
            inputBuffer.push(PlayerIndexInputPair{ playerIndex, ResignInput {} });
        }

        std::queue<PlayerIndexInputPair> poll() final {
            std::queue<PlayerIndexInputPair> ret;
            swap(ret, inputBuffer);
            return ret;
        }
    };

}
