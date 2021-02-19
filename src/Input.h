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

    enum class BuyPropertyOption {
        Auction,
        Buy,
    };
    // Buy the unowned property the active player is on at its listed price. Only the active player can buy, and only if they have sufficient funds.
    struct BuyPropertyInput {
        BuyPropertyOption option;
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

    // Ends your turn
    struct DeclineTradeInput {
    };

    // Ends your turn
    struct EndTurnInput {
    };

    using Input = std::variant<
        RollInput,
        BuyPropertyInput,
        BuyBuildingInput,
        SellBuildingInput,
        UnmortgagePropertiesInput,
        MortgagePropertiesInput,
        UseGetOutOfJailFreeCardInput,
        PayBailInput,
        BidInput,
        DeclineBidInput,
        OfferTradeInput,
        DeclineTradeInput,
        EndTurnInput
    >;
    using PlayerIndexInputPair = std::pair<int, Input>;
}
