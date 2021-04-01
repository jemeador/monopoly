#pragma once

#include "Board.h"
#include "GameState.h"

#include <map>
#include <set>
#include <variant>

namespace monopoly {

    // Roll the dice. If game is setup with loaded dice, loadedDiceValues will be used for the dice roll, otherwise the value are ignored
    struct RollInput {
        std::pair<int, int> loadedDiceValues;
    };

    // Buy the landed on property at its listed price. Only the active player can buy a property immediately after landing on it, and only if it is unowned and they have sufficient funds.
    struct BuyPropertyInput {
    };

    // Put the landed on property up for auction. Only the active player can start an auction immediately after landing on an unowned property.
    struct AuctionPropertyInput {
    };

    // Buy a building on a property. Inputs that do not follow the building placement rules of monopoly are ignored.
    // You may do this at any time on your turn between actions, or when you receive property from another player
    struct BuyBuildingInput {
        Property property;
    };

    // Sell a building on a property. Inputs that do not follow the building placement rules of monopoly are ignored.
    // You may do this at any time on your turn between actions, or when you receive property from another player
    struct SellBuildingInput {
        Property property;
    };

    // Sell ALL buildings in a property group. This may be necessary if there aren't enough houses to replace hotels.
    // You may do this at any time on your turn between actions, or when you receive property from another player
    struct SellAllBuildingsInput {
        PropertyGroup group;
    };

    // Unmortgage real estate properties.
    // You may do this at any time on your turn between actions, or when prompted
    struct UnmortgagePropertiesInput {
        std::set<Property> properties;
    };

    // Mortgage real estate properties.
    // You may do this at any time on your turn between actions, or when prompted
    struct MortgagePropertiesInput {
        std::set<Property> properties;
    };

    // Get out of jail without paying paying bail
    // You may do this at the beginning of your turn.
    struct UseGetOutOfJailFreeCardInput {
        DeckType preferredDeckType = DeckType::Chance; // If players have both cards, the one from this deck will be turned in
    };

    // Get out of jail by paying bail
    // You may do this at the beginning of your turn.
    struct PayBailInput {
    };

    // Bid in an auction
    // You may do this when receiving a bid prompt
    struct BidInput {
        int amount;
    };

    // Decline to bid, you can no longer bid in this auction
    // You may do this when receiving a bid prompt
    struct DeclineBidInput {
    };

    // Offer a trade to another player
    // You may do this at any time on your turn between actions, or when you receive property from another player
    struct OfferTradeInput {
        Promise offer;
        int consideringPlayer;
        Promise consideration;
    };

    // Decline to trade with another player after recieving a trade proposal
    // Only the considering player can decline a trade.
    struct DeclineTradeInput {
    };

    // Ends your turn
    // Can only be used at the end of the active player's turn
    struct EndTurnInput {
    };

    // Resign from the game
    // Can be done at any time, but may be the only viable action when there are unsettled debts that cannot be paid
    struct ResignInput {
    };

    using Input = std::variant<
        RollInput,
        BuyPropertyInput,
        AuctionPropertyInput,
        BuyBuildingInput,
        SellBuildingInput,
        SellAllBuildingsInput,
        UnmortgagePropertiesInput,
        MortgagePropertiesInput,
        UseGetOutOfJailFreeCardInput,
        PayBailInput,
        BidInput,
        DeclineBidInput,
        OfferTradeInput,
        DeclineTradeInput,
        EndTurnInput,
        ResignInput
    >;
    using PlayerIndexInputPair = std::pair<int, Input>;

    inline Trade trade_from_offer_input(int playerIndex, OfferTradeInput offerInput) {
        Trade trade;
        trade.offeringPlayer = playerIndex;
        trade.offer = offerInput.offer;
        trade.consideringPlayer = offerInput.consideringPlayer;
        trade.consideration = offerInput.consideration;
        return trade;
    }
}
