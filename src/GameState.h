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
	};

	class GameState
	{
	public:
		explicit GameState(GameSetup setup = {});
		Bank get_bank() const;
		int get_player_count() const;
		Player get_player(int playerIndex) const;
		int get_active_player_index() const;
		int get_next_player_index() const;
		int get_net_worth(int playerIndex) const;
		std::optional<int> get_property_owner_index(Property property) const;
		int get_properties_owned_in_group(int playerIndex, PropertyGroup group) const;
		TurnPhase get_turn_phase() const;
		std::map<Property, int> const &get_building_levels() const;

		std::pair<int, int> random_dice_roll();

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
		void force_property_offer_prompt(int playerIndex, Property property);

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

		void force_set_building_levels(std::map<Property, int> newBuildingLevels);

		void force_give_get_out_of_jail_free_card(int playerIndex, DeckType deckType);
		void force_transfer_get_out_of_jail_free_card(int fromPlayerIndex, int toPlayerIndex, DeckType deckType);
		void force_use_get_out_of_jail_free_card(int playerIndex, DeckType preferredDeckType);

		void force_liquidate_prompt(int debtorPlayerIndex);
		void force_liquidate_prompt(int debtorPlayerIndex, int creditorPlayerIndex);
		void force_bankrupt(int debtorPlayerIndex);
		void force_bankrupt(int debtorPlayerIndex, int creditorPlayerIndex);

	private:
		static Player init_player(GameSetup const& setup);
		static std::vector<Player> init_players(GameSetup const& setup);
		static Deck init_deck(GameSetup const& setup, DeckType deck_type);
		static std::map<DeckType, Deck> init_decks(GameSetup const& setup);

		std::mt19937 rng;

		TurnPhase phase;
		Bank bank;
		std::map<DeckType, Deck> decks;

		std::vector<Player> players;
		int activePlayerIndex;
		int doublesStreak;

		std::set<Property> mortgagedPropreties;
		std::map<Property, int> buildingLevels;
	};
}
