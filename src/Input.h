#include "Board.h"

#include <map>
#include <set>
#include <variant>

namespace monopoly {

	struct RollInput {
		std::pair<int, int> loadedDiceValues;
	};

	enum class BuyPropertyOption {
		Auction,
		Buy,
	};
	struct BuyPropertyInput {
		BuyPropertyOption option;
	};

	struct BuyHousesInput {
		std::map<Property, int> properties;
	};

	struct SellHousesInput {
		std::map<Property, int> properties;
	};

	struct UnmortgagePropertiesInput {
		std::set<Property> properties;
	};

	struct MortgagePropertiesInput {
		std::set<Property> properties;
	};

	struct UseGetOutOfJailFreeCardInput {
		DeckType preferredDeckType = DeckType::Chance; // If players have both cards, the one from this deck will be turned in
	};

	struct PayBailInput {
	};

	struct BidInput {
		int funds;
	};

	struct OfferTradeAssets {
		int funds;
		std::set<Property> deeds;
		std::set<DeckType> getOutOfJailFreeCards;
	};
	struct OfferTradeInput {
		OfferTradeAssets offering;
		int tradingPartnerIndex;
		OfferTradeAssets tradingPartnerAssets;
	};

	using Input = std::variant<
		RollInput,
		BuyPropertyInput,
		BuyHousesInput,
		SellHousesInput,
		UnmortgagePropertiesInput,
		MortgagePropertiesInput,
		UseGetOutOfJailFreeCardInput,
		PayBailInput,
		BidInput,
		OfferTradeInput
	>;
	using PlayerIndexInputPair = std::pair<int, Input>;
}
