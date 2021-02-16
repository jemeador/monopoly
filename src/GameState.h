#include"Cards.h"
#include"Board.h"
#include"Player.h"

#include<map>
#include<optional>
#include<set>
#include<vector>

namespace monopoly
{
    enum class TurnPhase {
        WaitingForRoll,
        WaitingForBuyPropertyInput,
        Auction,
        WaitingForTurnEnd,
    };

    inline char const* to_string(TurnPhase turnPhase) {
        switch (turnPhase) {
        case TurnPhase::WaitingForRoll: return "WaitingForRoll";
        case TurnPhase::WaitingForBuyPropertyInput: return "WaitingForBuyPropertyInput";
        case TurnPhase::Auction: return "Auction";
        case TurnPhase::WaitingForTurnEnd: return "WaitingForTurnEnd";
        }
        return "N/A";
    }

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

    class GameState
    {
    public:
        explicit GameState(GameSetup setup = {});
        int get_turn() const;
        Bank get_bank() const;
        int get_player_count() const;
        int get_players_remaining_count() const;
        Player get_player(int playerIndex) const;
        int get_active_player_index() const;
        int get_next_player_index() const;
        int get_net_worth(int playerIndex) const;
        std::optional<int> get_property_owner_index(Property property) const;
        bool get_property_is_mortgaged(Property property) const;
        int get_properties_owned_in_group(Property property) const;
        int get_properties_owned_in_group_by_player(int playerIndex, PropertyGroup group) const;
        TurnPhase get_turn_phase() const;
        int get_building_level(Property property) const;
        std::map<Property, int> const& get_building_levels() const;
        int calculate_rent(Property property) const;
        bool waiting_on_prompt() const;

        std::pair<int, int> random_dice_roll();
        std::pair<int, int> get_last_dice_roll() const;

        bool check_if_player_is_allowed_to_roll(int actorIndex) const;
        bool check_if_player_is_allowed_to_buy_property(int actorIndex) const;
        bool check_if_player_is_allowed_to_mortgage(int actorIndex, Property property) const;
        bool check_if_player_is_allowed_to_unmortgage(int actorIndex, Property property) const;
        bool check_if_player_is_allowed_to_buy_building(int actorIndex, Property property) const;
        bool check_if_player_is_allowed_to_sell_building(int actorIndex, Property property) const;

        void force_turn_start(int playerIndex);
        void force_turn_continue();
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
        void force_transfer_deed(std::set<Property>& from, std::set<Property>& to, Property deed);
        void force_transfer_deeds(std::set<Property>& from, std::set<Property>& to, std::set<Property> deeds);

        void force_mortgage(Property property);
        void force_unmortgage(Property property);

        void force_buy_building(Property property);
        void force_sell_building(Property property);

        void force_set_mortgaged(Property property, bool mortgaged);
        void force_set_building_levels(std::map<Property, int> newBuildingLevels);

        void force_give_get_out_of_jail_free_card(int playerIndex, DeckType deckType);
        void force_transfer_get_out_of_jail_free_card(int fromPlayerIndex, int toPlayerIndex, DeckType deckType);
        void force_transfer_get_out_of_jail_free_cards(int fromPlayerIndex, int toPlayerIndex);
        void force_use_get_out_of_jail_free_card(int playerIndex, DeckType preferredDeckType);
        void force_return_get_out_of_jail_free_card(int playerIndex, DeckType deckType);
        void force_return_get_out_of_jail_free_cards(int playerIndex);

        void force_pay_bail(int playerIndex);

        void force_bankrupt(int debtorPlayerIndex);
        void force_bankrupt(int debtorPlayerIndex, int creditorPlayerIndex);

        void force_roll_prompt(int playerIndex);
        void force_property_offer_prompt(int playerIndex, Property property);
        void force_liquidate_prompt(int debtorPlayerIndex);
        void force_liquidate_prompt(int debtorPlayerIndex, int creditorPlayerIndex);

        inline bool operator==(GameState const& rhs) const {
            return rng == rhs.rng &&
                turn == rhs.turn &&
                phase == rhs.phase &&
                bank == rhs.bank &&
                decks == rhs.decks &&
                players == rhs.players &&
                activePlayerIndex == rhs.activePlayerIndex &&
                doublesStreak == rhs.doublesStreak &&
                lastDiceRoll == rhs.lastDiceRoll &&
                mortgagedProperties == rhs.mortgagedProperties &&
                buildingLevels == rhs.buildingLevels;
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
        int activePlayerIndex;
        int doublesStreak;
        std::pair<int, int> lastDiceRoll;

        std::set<Property> mortgagedProperties;
        std::map<Property, int> buildingLevels;
    };
}
