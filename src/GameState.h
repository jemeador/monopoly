#pragma once

#include"Cards.h"
#include"Board.h"
#include"Player.h"

#include<map>
#include<optional>
#include<queue>
#include<set>
#include<vector>

namespace monopoly
{
    enum class TurnPhase {
        WaitingForTradeOfferResponse,
        WaitingForDebtSettlement,
        WaitingForBids,
        WaitingForAcquisitionManagement,
        WaitingForBuyPropertyInput,
        WaitingForRoll,
        WaitingForTurnEnd,
    };

    struct GameSetup
    {
        std::string seed;
        int playerCount = 4;
        int startingFunds = 1500;

        // Enable forced die rolls from the interface
        bool loadedDiceEnabled = false;
        // Put these cards on top after shuffling
        std::vector<Card> stackCommunityChest = {};
        std::vector<Card> stackChance = {};
    };

    struct Bank
    {
        std::set<Property> deeds = all_properties();

        inline bool operator==(Bank const& rhs) const {
            return deeds == rhs.deeds;
        }
        inline bool operator!=(Bank const& rhs) const {
            return !operator==(rhs);
        }
    };

    // Auction minigame
    //
    // The rules for auctions are not well-defined by the official rules of
    // monopoly, but this is how it is implemented here:
    // - Starting bid is $0 for the player who either (depending on the
    // circumstances leading to the auction)
    //   - landed on the unowned property being auctioned
    //   - is next in player order from the bankrupted player who is having
    //   their deeds auctioned off
    // - Bids continue in player order, players can either:
    //   - Bid a dollar amount greater than the previous bid
    //   - Decline to bid. By declining you remove yourself from the auction
    // - Bids may be for any dollar amount over the previous bid, not exceeding the player's liquid assets sum value plus closing costs
    // - After all but one player has declined to bid, the remaining player
    // must pay the bank the bid amount; liquidating assets as necesary
    // - After the auction is completed
    //   - If by winning the auction a player goes bankrupt (or they resign), all of their
    //   property is auctioned off, the property causing the
    //   bankruptcy remains in the pool of assets to auction
    //  - If the winning bidder pays the bid price
    //    - They receive the property
    //    - If the received property is mortgaged, they must either immediately
    //    unmortgage it or pay a 10% penalty (which can bankrupt the player!)

    struct Auction
    {
        Property property;
        int highestBid;
        std::queue<int> biddingOrder; // front is the next bidder's player index, back is the recent (highest) bidder

        bool operator== (Auction const& rhs) const {
            return
                highestBid == rhs.highestBid &&
                biddingOrder == rhs.biddingOrder;
        }
        bool operator!= (Auction const& rhs) const { return operator!= (rhs); }
    };

    struct Debt {
        int debtor;
        std::optional<int> creditor;
        int amount;

        bool operator== (Debt const& rhs) const {
            return
                debtor == rhs.debtor &&
                creditor == rhs.creditor &&
                amount == rhs.amount;
        }
        bool operator!= (Debt const& rhs) const { return operator!= (rhs); }
    };

    struct Promise {
        int cash;
        std::set<Property> deeds;
        std::set<DeckType> getOutOfJailFreeCards;

        bool operator== (Promise const& rhs) const {
            return
                cash == rhs.cash &&
                deeds == rhs.deeds &&
                getOutOfJailFreeCards == rhs.getOutOfJailFreeCards;
        }
        bool operator!= (Promise const& rhs) const { return operator!= (rhs); }
    };

    struct Trade {
        int offeringPlayer;
        int consideringPlayer;
        Promise offer;
        Promise consideration;

        bool operator== (Trade const& rhs) const {
            return
                offeringPlayer == rhs.offeringPlayer &&
                consideringPlayer == rhs.consideringPlayer &&
                offer == rhs.offer &&
                consideration == rhs.consideration;
        }
        bool operator!= (Trade const& rhs) const { return operator!= (rhs); }
    };
    
    inline Trade reciprocal_trade(Trade t) {
        return {
            t.consideringPlayer,
            t.offeringPlayer,
            t.consideration,
            t.offer,
        };
    }

    inline bool trades_are_reciprocal(Trade t1, Trade t2) {
        return t1 == reciprocal_trade(t2);
    }

    struct Acquisition {
        int recipient;
        std::set<Property> deeds;

        bool operator== (Acquisition const& rhs) const {
            return
                recipient == rhs.recipient &&
                deeds == rhs.deeds;
        }
        bool operator!= (Acquisition const& rhs) const { return operator!= (rhs); }
    };

    class GameState
    {
    public:
        explicit GameState(GameSetup setup = {});
        int get_turn() const;
        Bank get_bank() const;
        int get_player_count() const;
        int get_players_remaining_count() const;
        Player get_player(int playerIndex) const;
        int get_active_player_index() const; // it's the active player's turn
        int get_controlling_player_index() const; // the game is waiting on input from the controlling player
        int get_next_player_index(int playerIndex = -1) const; // if -1, use activePlayerIndex
        int get_net_worth(int playerIndex) const;
        std::optional<int> get_property_owner_index(Property property) const;
        bool get_property_is_mortgaged(Property property) const;
        int get_properties_owned_in_group(Property property) const;
        int get_properties_owned_in_group_by_player(int playerIndex, PropertyGroup group) const;
        TurnPhase get_turn_phase() const;
        int get_building_level(Property property) const;
        std::map<Property, int> const& get_building_levels() const;
        std::optional<Auction> get_current_auction() const;
        int calculate_rent(Property property) const;
        int calculate_closing_costs_on_sale(Property property) const;
        int calculate_liquid_assets_value(int playerIndex) const;
        int calculate_liquid_value_of_deeds(std::set<Property> deeds) const;
        int calculate_liquid_value_of_buildings(std::set<Property> deeds) const;
        int calculate_liquid_value_of_promise(Promise promise) const;

        bool waiting_on_player_actions() const;

        std::pair<int, int> random_dice_roll();
        std::pair<int, int> get_last_dice_roll() const;

        bool check_if_player_is_allowed_to_roll(int actorIndex) const;
        bool check_if_player_is_allowed_to_use_get_out_jail_free_card(int actorIndex) const;
        bool check_if_player_is_allowed_to_pay_bail(int actorIndex) const;
        bool check_if_player_is_allowed_to_buy_property(int actorIndex) const;
        bool check_if_player_is_allowed_to_mortgage(int actorIndex, Property property) const;
        bool check_if_player_is_allowed_to_unmortgage(int actorIndex, Property property) const;
        bool check_if_player_is_allowed_to_buy_building(int actorIndex, Property property) const;
        bool check_if_player_is_allowed_to_sell_building(int actorIndex, Property property) const;
        bool check_if_player_is_allowed_to_bid(int actorIndex, int amount) const;
        bool check_if_player_is_allowed_to_decline_bid(int actorIndex) const;
        bool check_if_player_is_allowed_to_decline_trade(int actorIndex) const;
        bool check_if_trade_is_valid(Trade trade) const;
        bool check_if_player_can_fulfill_promise(int playerIndex, Promise promise) const;
        bool check_if_player_can_pay_closing_costs(int playerIndex, Promise lostAssets, Promise gainedAssets) const;

        void force_turn_start(int playerIndex);
        void force_turn_end();

        void force_funds(int playerIndex, int funds);
        void force_add_funds(int playerIndex, int funds);
        void force_subtract_funds(int playerIndex, int funds);
        void force_transfer_funds(int fromPlayerIndex, int toPlayerIndex, int funds);

        void force_go_to_jail(int playerIndex);
        void force_leave_jail(int playerIndex);

        void force_random_roll(int playerIndex);
        void force_roll(int playerIndex, std::pair<int, int> roll);
        void force_advance(int playerIndex, int dist);
        void force_advance_without_landing(int playerIndex, int dist);
        void force_advance_to(int playerIndex, Space space);
        void force_advance_to_without_landing(int playerIndex, Space space);
        void force_land(int playerIndex, Space space);
        void force_position(int playerIndex, Space space);

        void force_property_offer(int playerIndex, Property space);

        void force_property_buy(int playerIndex, Property property);
        void force_property_auction(Property property);

        void force_stack_deck(DeckType deckType, DeckContainer const& cards);
        void force_draw_chance_card(int playerIndex);
        void force_draw_community_chest_card(int playerIndex);
        void force_draw_card(int playerIndex, DeckType deckType);

        void force_income_tax(int playerIndex);
        void force_luxury_tax(int playerIndex);

        void force_give_deed(int playerIndex, Property property);
        void force_give_deeds(int playerIndex, std::set<Property> properties);
        void force_transfer_deed(int fromPlayerIndex, int toPlayerIndex, Property deed);
        void force_transfer_deeds(int fromPlayerIndex, int toPlayerIndex, std::set<Property> deeds);

        void force_mortgage(Property property);
        void force_unmortgage(Property property);

        void force_buy_building(Property property);
        void force_sell_building(Property property);

        void force_set_mortgaged(Property property, bool mortgaged);
        void force_set_building_levels(std::map<Property, int> newBuildingLevels);

        void force_give_get_out_of_jail_free_card(int playerIndex, DeckType deckType);
        void force_transfer_get_out_of_jail_free_card(int fromPlayerIndex, int toPlayerIndex, DeckType deckType);
        void force_transfer_get_out_of_jail_free_cards(int fromPlayerIndex, int toPlayerIndex, std::set<DeckType> deckTypes = {DeckType::Chance, DeckType::CommunityChest});
        void force_use_get_out_of_jail_free_card(int playerIndex, DeckType preferredDeckType);
        void force_return_get_out_of_jail_free_card(int playerIndex, DeckType deckType);
        void force_return_get_out_of_jail_free_cards(int playerIndex);

        void force_pay_bail(int playerIndex);

        void force_bankrupt(int debtorPlayerIndex);
        void force_bankrupt(int debtorPlayerIndex, int creditorPlayerIndex);

        void force_property_offer_prompt(int playerIndex, Property property);
        void force_liquidate_prompt(int debtorPlayerIndex, int amount);
        void force_liquidate_prompt(int debtorPlayerIndex, int creditorPlayerIndex, int amount);

        void force_bid(int playerIndex, int amount);
        void force_decline_bid(int playerIndex);

        void force_offer_trade(Trade trade);
        void force_decline_trade(int playerIndex);
        void force_trade(Trade trade);
        void force_transfer_promise(int fromPlayerIndex, int toPlayerIndex, Promise promise);

        inline bool operator==(GameState const& rhs) const {
            return rng == rhs.rng &&
                turn == rhs.turn &&
                phase == rhs.phase &&
                bank == rhs.bank &&
                decks == rhs.decks &&
                players == rhs.players &&
                doublesStreak == rhs.doublesStreak &&
                lastDiceRoll == rhs.lastDiceRoll &&
                mortgagedProperties == rhs.mortgagedProperties &&
                buildingLevels == rhs.buildingLevels &&
                pendingTradeAgreement == rhs.pendingTradeAgreement &&
                pendingDebtSettlements == rhs.pendingDebtSettlements &&
                currentAuction == rhs.currentAuction &&
                pendingAuctionSale == rhs.pendingAuctionSale &&
                propertiesPendingAuction == rhs.propertiesPendingAuction &&
                pendingAcquisitions == rhs.pendingAcquisitions &&
                pendingPurchaseDecision == rhs.pendingPurchaseDecision &&
                pendingRoll == rhs.pendingRoll &&
                activePlayerIndex == rhs.activePlayerIndex;
        }
        inline bool operator!=(GameState const& rhs) const {
            return !operator==(rhs);
        }

    private:
        static Player init_player(GameSetup const& setup);
        static std::vector<Player> init_players(GameSetup const& setup);
        static Deck init_deck(GameSetup const& setup, DeckType deck_type);
        static std::map<DeckType, Deck> init_decks(GameSetup const& setup);


        std::mt19937 rng;

        int turn = 0;
        TurnPhase phase;
        Bank bank;
        std::map<DeckType, Deck> decks;

        std::vector<Player> players;
        int doublesStreak;
        std::pair<int, int> lastDiceRoll;

        std::set<Property> mortgagedProperties;
        std::map<Property, int> buildingLevels;

        // Resolution order
        //
        // Most game states require some user interaction in order to resolve. Some
        // pending events take priority over others (for example a player must roll
        // the dice before they can end their turn).
        void resolve_game_state();
        void resolve_debt_settlements();
        void resolve_auction();
        void resolve_auction_sale();
        void queue_auction(Property property);
        // The game is constantly trying to reach the "end state" which is a game
        // over. Pending events are resolved in the following order:

        // Handle trade offers
        std::optional<Trade> pendingTradeAgreement;
        // Handle debt settlements
        std::queue<Debt> pendingDebtSettlements;
        // Handle current auction
        std::optional<Auction> currentAuction;
        // Handle closing auction
        std::optional<std::pair<int, Property>> pendingAuctionSale;
        // Handle pending auctions
        std::queue<Property> propertiesPendingAuction;
        // Handle acquisition management (if a player received a property on another player's turn they are given this opportunity)
        std::queue<Acquisition> pendingAcquisitions;
        // Handle purchase decision
        bool pendingPurchaseDecision;
        // Handle roll input
        bool pendingRoll;
        // Handle end turn input
        int activePlayerIndex;
        // Game over (next player is active player)

        // Through user action, pending events can be resolved and cleared, or
        // higher priority events can be raised, until the end of the game. A
        // pending event _can never_ depend on the resolution of an event lower in
        // priority (for example you cannot trigger an auction to settle a debt).
        // This rule allows game events to be resolved in a way that avoids
        // undesirable loops.
    };
}
