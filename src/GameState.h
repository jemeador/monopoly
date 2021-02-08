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
		TurnStart,
		WaitingForBuyPropertyInput,
		Auction,
	};

	inline char const* to_string(TurnPhase turnPhase) {
		switch (turnPhase) {
		case TurnPhase::TurnStart: return "TurnStart";
		case TurnPhase::WaitingForBuyPropertyInput: return "WaitingForBuyPropertyInput";
		case TurnPhase::Auction: return "Auction";
		}
		return "N/A";
	}

	struct GameSetup
	{
		std::string seed;
		int playerCount = 4;
		int startingFunds = 1500;
		bool loadedDiceEnabled = false;
	};

	struct Bank
	{
		std::set<Property> deeds = all_properties();
		std::set<Deck::Type> getOutOfJailFreeCards = { Deck::Type::Chance, Deck::Type::CommunityChest };
	};

	class GameState
	{
	public:
		explicit GameState(GameSetup setup = {});
		Bank get_bank() const;
		Player get_player(int playerIndex) const;
		int get_active_player_index() const;
		int get_next_player_index() const;
		int get_net_worth(int playerIndex) const;
		TurnPhase get_turn_phase() const;

		void force_turn_start(int playerIndex);
		void force_turn_end();

		void force_funds(int playerIndex, int funds);
		void force_add_funds(int playerIndex, int funds);
		void force_subtract_funds(int playerIndex, int funds);

		void force_go_to_jail(int playerIndex);
		void force_leave_jail(int playerIndex);

		void force_random_roll(int playerIndex);
		void force_roll(int playerIndex, std::pair<int, int> roll);
		void force_advance(int playerIndex, int spaceCount);
		void force_advance_to(int playerIndex, Space space);
		void force_land(int playerIndex, Space space);
		void force_position(int playerIndex, Space space);

		void force_property_offer(int playerIndex, Property space);
		void force_property_offer_prompt(int playerIndex, Property property);

		void force_property_buy(int playerIndex, Property property);
		void force_property_auction(int decliningPlayer);

		void force_income_tax(int playerIndex);
		void force_luxury_tax(int playerIndex);

		void force_deed_transfer(std::set<Property>& from, std::set<Property>& to, Property deed);
		void force_deed_transfers(std::set<Property>& from, std::set<Property>& to, std::set<Property> deeds);

		void force_get_out_of_jail_free_card_keep(int playerIndex, Deck::Type deckType);
		void force_get_out_of_jail_free_card_transfer(std::set<Deck::Type>& from, std::set<Deck::Type>& to, Deck::Type deckType);
		void force_get_out_of_jail_free_card_use(int playerIndex, Deck::Type preferredDeckType);

	private:
		Player init_player(GameSetup const& setup);
		std::vector<Player> init_players(GameSetup const& setup);
		Deck init_deck(GameSetup const& setup, Deck::Type deck_type);

		std::pair<int, int> random_dice_roll();

		std::mt19937 rng;

		TurnPhase phase;
		Bank bank;
		Deck chance;
		Deck communityChest;

		std::vector<Player> players;
		int activePlayerIndex;
		int doublesStreak;

		std::set<Property> mortgagedPropreties;
		std::map<Property, int> buildingLevel;
	};
}
