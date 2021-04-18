#if __EMSCRIPTEN__
#include <emscripten/bind.h>
using namespace emscripten;

#include "Cards.h"
#include "Board.h"
#include "Player.h"
#include "GameState.h"
#include "IInterface.h"
#include "Game.h"
#include "Input.h"
#include "DisplayStrings.h"
using namespace monopoly;

// Allow "subclassing" IInterface from Javascript
class SimpleInterfaceWrapper : public wrapper<SimpleInterface> {
public:
    EMSCRIPTEN_WRAPPER(SimpleInterfaceWrapper);
    GameSetup get_setup() {
        return call<GameSetup>("get_setup");
    }
    void update(GameState state) {
        return call<void>("update", state);
    }
};

EMSCRIPTEN_BINDINGS(Monopoly) {
    // IInterface.h
    class_<IInterface>("IInterface")
        .function("get_setup", &IInterface::get_setup, pure_virtual())
        .function("update", &IInterface::update, pure_virtual())
        ;

    class_<SimpleInterface, base<IInterface>>("SimpleInterface")
        .function("roll_dice", &SimpleInterfaceWrapper::roll_dice)
        .function("buy_property", &SimpleInterfaceWrapper::buy_property)
        .function("auction_property", &SimpleInterfaceWrapper::auction_property)
        .function("mortgage_property", &SimpleInterfaceWrapper::mortgage_property)
        .function("unmortgage_property", &SimpleInterfaceWrapper::unmortgage_property)
        .function("buy_building", &SimpleInterfaceWrapper::buy_building)
        .function("sell_building", &SimpleInterfaceWrapper::sell_building)
        .function("sell_all_buildings", &SimpleInterfaceWrapper::sell_all_buildings)
        .function("use_get_out_of_jail_free_card", &SimpleInterfaceWrapper::use_get_out_of_jail_free_card)
        .function("pay_bail", &SimpleInterfaceWrapper::pay_bail)
        .function("bid", &SimpleInterfaceWrapper::bid)
        .function("decline_bid", &SimpleInterfaceWrapper::decline_bid)
        .function("propose_trade", &SimpleInterfaceWrapper::propose_trade)
        .function("end_turn", &SimpleInterfaceWrapper::end_turn)
        .function("resign", &SimpleInterfaceWrapper::resign)
        .allow_subclass<SimpleInterfaceWrapper>("SimpleInterfaceWrapper")
        ;

    // Cards.h
    enum_<DeckType>("DeckType")
        .value("Chance", DeckType::Chance)
        .value("CommunityChest", DeckType::CommunityChest)
        ;

    enum_<Card>("Card")
        .value("Chance_AdvanceToBlue2", Card::Chance_AdvanceToBlue2)
        .value("Chance_AdvanceToGo", Card::Chance_AdvanceToGo)
        .value("Chance_AdvanceToMagenta1", Card::Chance_AdvanceToMagenta1)
        .value("Chance_AdvanceToNearestRailroad", Card::Chance_AdvanceToNearestRailroad)
        .value("Chance_AdvanceToNearestUtility", Card::Chance_AdvanceToNearestUtility)
        .value("Chance_AdvanceToRailroad1", Card::Chance_AdvanceToRailroad1)
        .value("Chance_AdvanceToRed3", Card::Chance_AdvanceToRed3)
        .value("Chance_Gain150", Card::Chance_Gain150)
        .value("Chance_Gain50", Card::Chance_Gain50)
        .value("Chance_GetOutOfJailFree", Card::Chance_GetOutOfJailFree)
        .value("Chance_GoBack3Spaces", Card::Chance_GoBack3Spaces)
        .value("Chance_GoToJail", Card::Chance_GoToJail)
        .value("Chance_Pay15", Card::Chance_Pay15)
        .value("Chance_PayEachPlayer50", Card::Chance_PayEachPlayer50)
        .value("Chance_Repairs", Card::Chance_Repairs)
        .value("CommunityChest_AdvanceToGo", Card::CommunityChest_AdvanceToGo)
        .value("CommunityChest_CollectFromEachPlayer50", Card::CommunityChest_CollectFromEachPlayer50)
        .value("CommunityChest_Gain10", Card::CommunityChest_Gain10)
        .value("CommunityChest_Gain100_A", Card::CommunityChest_Gain100_A)
        .value("CommunityChest_Gain100_B", Card::CommunityChest_Gain100_B)
        .value("CommunityChest_Gain100_C", Card::CommunityChest_Gain100_C)
        .value("CommunityChest_Gain20", Card::CommunityChest_Gain20)
        .value("CommunityChest_Gain200", Card::CommunityChest_Gain200)
        .value("CommunityChest_Gain25", Card::CommunityChest_Gain25)
        .value("CommunityChest_Gain45", Card::CommunityChest_Gain45)
        .value("CommunityChest_GetOutOfJailFree", Card::CommunityChest_GetOutOfJailFree)
        .value("CommunityChest_GoToJail", Card::CommunityChest_GoToJail)
        .value("CommunityChest_Pay100", Card::CommunityChest_Pay100)
        .value("CommunityChest_Pay150", Card::CommunityChest_Pay150)
        .value("CommunityChest_Pay50", Card::CommunityChest_Pay50)
        .value("CommunityChest_Repairs", Card::CommunityChest_Repairs)
        ;

    constant ("ChanceCards", ChanceCards);
    constant ("CommunityChestCards", CommunityChestCards);

    value_object<CardData>("CardData")
        .field("flavorText", &CardData::flavorText)
        .field("effectText", &CardData::effectText)
        ;

    function("deck_type_to_string", select_overload<std::string(DeckType)>(&to_string));
    function("space_to_string", select_overload<std::string(Space)>(&to_string));
    function("proprety_to_string", select_overload<std::string(Property)>(&to_string));
    function("property_group_to_string", select_overload<std::string(PropertyGroup)>(&to_string));
    function("card_to_string", select_overload<std::string(Card)>(&to_string));
    function("to_string", select_overload<std::string(TurnPhase)>(&to_string));
    function("card_data", &card_data);
    function("apply_card_effect", &apply_card_effect);
    function("card_is_get_out_of_jail_free", &card_is_get_out_of_jail_free);
    function("get_out_of_jail_free_card", &get_out_of_jail_free_card);

    class_<Deck>("Deck")
        .constructor<>()
        .constructor<DeckType>()
        .function ("draw", &Deck::draw)
        .function("stack_deck", &Deck::stack_deck)
        .function("add_card", &Deck::add_card)
        .function("remove_card", &Deck::remove_card)
        .function("shuffle", &Deck::shuffle)
        ;

    // Board.h
    constant ("NumberOfSpaces", NumberOfSpaces);
    constant ("MaxJailTurns", MaxJailTurns);
    constant ("HotelLevel", HotelLevel);
    constant ("BailCost", BailCost);
    constant ("GoSalary", GoSalary);
    constant ("MortgageInterestRate", MortgageInterestRate);
    constant ("DeedTable", DeedTable);
    constant ("DeedTableColumns", DeedTableColumns);

    value_object<std::pair<int, int>>("PairOfInt")
        .field("first", &std::pair<int, int>::first)
        .field("second", &std::pair<int, int>::second)
        ;

    enum_<DeedField>("DeedField")
        .value("Price", DeedField::Price)
        .value("PricePerHouse", DeedField::PricePerHouse)
        .value("Rent", DeedField::Rent)
        .value("Rent1", DeedField::Rent1)
        .value("Rent2", DeedField::Rent2)
        .value("Rent3", DeedField::Rent3)
        .value("Rent4", DeedField::Rent4)
        .value("RentHotel", DeedField::RentHotel)
        .value("Mortgage", DeedField::Mortgage)
        ;
    enum_<PropertyGroup>("PropertyGroup")
        .value("Brown", PropertyGroup::Brown)
        .value("LightBlue", PropertyGroup::LightBlue)
        .value("Magenta", PropertyGroup::Magenta)
        .value("Orange", PropertyGroup::Orange)
        .value("Red", PropertyGroup::Red)
        .value("Yellow", PropertyGroup::Yellow)
        .value("Green", PropertyGroup::Green)
        .value("Blue", PropertyGroup::Blue)
        .value("Utility", PropertyGroup::Utility)
        .value("Railroad", PropertyGroup::Railroad)
        ;
    enum_<Property>("Property")
        .value("Brown_1", Property::Brown_1)
        .value("Brown_2", Property::Brown_2)
        .value("LightBlue_1", Property::LightBlue_1)
        .value("LightBlue_2", Property::LightBlue_2)
        .value("LightBlue_3", Property::LightBlue_3)
        .value("Magenta_1", Property::Magenta_1)
        .value("Magenta_2", Property::Magenta_2)
        .value("Magenta_3", Property::Magenta_3)
        .value("Orange_1", Property::Orange_1)
        .value("Orange_2", Property::Orange_2)
        .value("Orange_3", Property::Orange_3)
        .value("Red_1", Property::Red_1)
        .value("Red_2", Property::Red_2)
        .value("Red_3", Property::Red_3)
        .value("Yellow_1", Property::Yellow_1)
        .value("Yellow_2", Property::Yellow_2)
        .value("Yellow_3", Property::Yellow_3)
        .value("Green_1", Property::Green_1)
        .value("Green_2", Property::Green_2)
        .value("Green_3", Property::Green_3)
        .value("Blue_1", Property::Blue_1)
        .value("Blue_2", Property::Blue_2)
        .value("Utility_1", Property::Utility_1)
        .value("Utility_2", Property::Utility_2)
        .value("Railroad_1", Property::Railroad_1)
        .value("Railroad_2", Property::Railroad_2)
        .value("Railroad_3", Property::Railroad_3)
        .value("Railroad_4", Property::Railroad_4)
        .value("Invalid ", Property::Invalid)
        ;
    enum_<Space>("Space")
        .value("Go", Space::Go)
        .value("Brown_1", Space::Brown_1)
        .value("CommunityChest_1", Space::CommunityChest_1)
        .value("Brown_2", Space::Brown_2)
        .value("IncomeTax", Space::IncomeTax)
        .value("Railroad_1", Space::Railroad_1)
        .value("LightBlue_1", Space::LightBlue_1)
        .value("Chance_1", Space::Chance_1)
        .value("LightBlue_2", Space::LightBlue_2)
        .value("LightBlue_3", Space::LightBlue_3)
        .value("Jail", Space::Jail)
        .value("Magenta_1", Space::Magenta_1)
        .value("Utility_1", Space::Utility_1)
        .value("Magenta_2", Space::Magenta_2)
        .value("Magenta_3", Space::Magenta_3)
        .value("Railroad_2", Space::Railroad_2)
        .value("Orange_1", Space::Orange_1)
        .value("CommunityChest_2", Space::CommunityChest_2)
        .value("Orange_2", Space::Orange_2)
        .value("Orange_3", Space::Orange_3)
        .value("FreeParking", Space::FreeParking)
        .value("Red_1", Space::Red_1)
        .value("Chance_2", Space::Chance_2)
        .value("Red_2", Space::Red_2)
        .value("Red_3", Space::Red_3)
        .value("Railroad_3", Space::Railroad_3)
        .value("Yellow_1", Space::Yellow_1)
        .value("Yellow_2", Space::Yellow_2)
        .value("Utility_2", Space::Utility_2)
        .value("Yellow_3", Space::Yellow_3)
        .value("GoToJail", Space::GoToJail)
        .value("Green_1", Space::Green_1)
        .value("Green_2", Space::Green_2)
        .value("CommunityChest_3", Space::CommunityChest_3)
        .value("Green_3", Space::Green_3)
        .value("Railroad_4", Space::Railroad_4)
        .value("Chance_3", Space::Chance_3)
        .value("Blue_1", Space::Blue_1)
        .value("LuxuryTax", Space::LuxuryTax)
        .value("Blue_2", Space::Blue_2)
        ;

    function("price_of_property", &price_of_property);
    function("property_group", &property_group);
    function("properties_in_group", &properties_in_group);
    function("property_is_in_group", &property_is_in_group);
    function("property_is_in_group", &property_is_in_group);
    function("all_properties", &all_properties);
    function("real_estate_table_lookup", &real_estate_table_lookup);
    function("price_of_property", price_of_property);
    function("price_per_house_on_property", price_per_house_on_property);
    function("sell_price_per_house_on_property", sell_price_per_house_on_property);
    function("rent_price_of_real_estate", rent_price_of_real_estate);
    function("rent_price_of_improved_real_estate", rent_price_of_improved_real_estate);
    function("rent_price_of_railroad", rent_price_of_railroad);
    function("rent_price_of_utility", rent_price_of_utility);
    function("mortgage_value_of_property", mortgage_value_of_property);
    function("unmortgage_price_of_property", unmortgage_price_of_property);
    function("space_to_property", space_to_property);
    function("property_to_space", property_to_space);
    function("space_is_property", space_is_property);
    function("space_to_index", space_to_index);
    function("index_to_space", index_to_space);
    function("advancing_will_pass_go", advancing_will_pass_go);
    function("distance", distance);
    function("add_distance", add_distance);
    function("nearest_space", nearest_space);

    // Player.h
    constant ("PlayerNone", Player::None);
    constant ("Player1", Player::p1);
    constant ("Player2", Player::p2);
    constant ("Player3", Player::p3);
    constant ("Player4", Player::p4);
    function("player_name", &player_name);

    // GameState.h
    enum_ <TurnPhase>("TurnPhase")
        .value("WaitingForTradeOfferResponse", TurnPhase::WaitingForTradeOfferResponse)
        .value("WaitingForDebtSettlement", TurnPhase::WaitingForDebtSettlement)
        .value("WaitingForBids", TurnPhase::WaitingForBids)
        .value("WaitingForAcquisitionManagement", TurnPhase::WaitingForAcquisitionManagement)
        .value("WaitingForBuyPropertyInput", TurnPhase::WaitingForBuyPropertyInput)
        .value("WaitingForRoll", TurnPhase::WaitingForRoll)
        .value("WaitingForTurnEnd", TurnPhase::WaitingForTurnEnd)
        ;
    value_object<GameSetup>("GameSetup")
        .field("seed", &GameSetup::seed)
        .field("playerCount", &GameSetup::playerCount)
        .field("startingFunds", &GameSetup::startingFunds)
        ;
    value_object<Bank>("Bank")
        .field("houses", &Bank::houses)
        .field("hotels", &Bank::hotels)
        .field("deeds", &Bank::deeds)
        ;
    value_object<Auction>("Auction")
        .field("property", &Auction::property)
        .field("highestBid", &Auction::highestBid)
        .field("biddingOrder", &Auction::biddingOrder)
    ;
    register_vector<int>("vector<int>");
    value_object<Debt>("Debt")
        .field("debtor", &Debt::debtor)
        .field("creditor", &Debt::creditor)
        .field("amount", &Debt::amount)
    ;
    value_object<Promise>("Promise")
        .field("cash", &Promise::cash)
        .field("deeds", &Promise::deeds)
        .field("getOutOfJailFreeCards", &Promise::getOutOfJailFreeCards)
    ;
    value_object<Trade>("Trade")
        .field("offeringPlayer", &Trade::offeringPlayer)
        .field("consideringPlayer", &Trade::consideringPlayer)
        .field("offer", &Trade::offer)
        .field("consideration", &Trade::consideration)
    ;
    function("reciprocal_trade", &reciprocal_trade);
    function("trades_are_reciprocal", &trades_are_reciprocal);
    value_object<Acquisition>("Acquisition")
        .field("recipient", &Acquisition::recipient)
        .field("deeds", &Acquisition::deeds)
    ;
    class_<GameState>("GameState")
        .constructor<>()
        .constructor<GameSetup>()
        .function("assign", select_overload<GameState &(GameState const &)>(&GameState::operator=))
        .function("is_game_over", &GameState::is_game_over)
        .function("get_turn", &GameState::get_turn)
        .function("get_bank", &GameState::get_bank)
        .function("get_player_count", &GameState::get_player_count)
        .function("get_players_remaining_count", &GameState::get_players_remaining_count)
        .function("get_player_eliminated",&GameState::get_player_eliminated)
        .function("get_player_funds",&GameState::get_player_funds)
        .function("get_player_position",&GameState::get_player_position)
        .function("get_player_turns_remaining_in_jail",&GameState::get_player_turns_remaining_in_jail)
        .function("get_active_player_index", &GameState::get_active_player_index)
        .function("get_controlling_player_index", &GameState::get_controlling_player_index)
        .function("get_next_player_index", &GameState::get_next_player_index)
        .function("get_net_worth", &GameState::get_net_worth)
        .function("get_property_owner_index", &GameState::get_property_owner_index)
        .function("get_property_is_mortgaged", &GameState::get_property_is_mortgaged)
        .function("get_properties_owned_in_group", &GameState::get_properties_owned_in_group)
        .function("get_properties_owned_in_group_by_player", &GameState::get_properties_owned_in_group_by_player)
        .function("get_turn_phase", &GameState::get_turn_phase)
        .function("get_building_level", &GameState::get_building_level)
        .function("get_min_building_level_in_group", &GameState::get_min_building_level_in_group)
        .function("get_max_building_level_in_group", &GameState::get_max_building_level_in_group)
        .function("get_building_levels", &GameState::get_building_levels)
        .function("get_current_auction", &GameState::get_current_auction)
        .function("calculate_rent", &GameState::calculate_rent)
        .function("calculate_closing_costs_on_sale", &GameState::calculate_closing_costs_on_sale)
        .function("calculate_liquid_assets_value", &GameState::calculate_liquid_assets_value)
        .function("calculate_liquid_value_of_deeds", &GameState::calculate_liquid_value_of_deeds)
        .function("calculate_liquid_value_of_buildings", &GameState::calculate_liquid_value_of_buildings)
        .function("calculate_liquid_value_of_promise", &GameState::calculate_liquid_value_of_promise)
        .function("waiting_on_player_actions", &GameState::waiting_on_player_actions)
        .function("random_dice_roll", &GameState::random_dice_roll)
        .function("get_last_dice_roll", &GameState::get_last_dice_roll)
        .function("check_if_player_is_allowed_to_roll", &GameState::check_if_player_is_allowed_to_roll)
        .function("check_if_player_is_allowed_to_use_get_out_jail_free_card", &GameState::check_if_player_is_allowed_to_use_get_out_jail_free_card)
        .function("check_if_player_is_allowed_to_pay_bail", &GameState::check_if_player_is_allowed_to_pay_bail)
        .function("check_if_player_is_allowed_to_buy_property", &GameState::check_if_player_is_allowed_to_buy_property)
        .function("check_if_player_is_allowed_to_auction_property", &GameState::check_if_player_is_allowed_to_auction_property)
        .function("check_if_player_is_allowed_to_mortgage", &GameState::check_if_player_is_allowed_to_mortgage)
        .function("check_if_player_is_allowed_to_unmortgage", &GameState::check_if_player_is_allowed_to_unmortgage)
        .function("check_if_player_is_allowed_to_buy_building", &GameState::check_if_player_is_allowed_to_buy_building)
        .function("check_if_player_is_allowed_to_sell_building", &GameState::check_if_player_is_allowed_to_sell_building)
        .function("check_if_player_is_allowed_to_sell_all_buildings", &GameState::check_if_player_is_allowed_to_sell_all_buildings)
        .function("check_if_player_is_allowed_to_bid", &GameState::check_if_player_is_allowed_to_bid)
        .function("check_if_player_is_allowed_to_decline_bid", &GameState::check_if_player_is_allowed_to_decline_bid)
        .function("check_if_player_is_allowed_to_decline_trade", &GameState::check_if_player_is_allowed_to_decline_trade)
        .function("check_if_player_is_allowed_to_end_turn", &GameState::check_if_player_is_allowed_to_end_turn)
        .function("check_if_player_is_allowed_to_resign", &GameState::check_if_player_is_allowed_to_resign)
        .function("check_if_trade_is_valid", &GameState::check_if_trade_is_valid)
        .function("check_if_player_can_fulfill_promise", &GameState::check_if_player_can_fulfill_promise)
        .function("check_if_player_can_pay_closing_costs", &GameState::check_if_player_can_pay_closing_costs)
        .function("player_action_roll", &GameState::player_action_roll)
        .function("player_action_use_get_out_of_jail_free_card", &GameState::player_action_use_get_out_of_jail_free_card)
        .function("player_action_pay_bail", &GameState::player_action_pay_bail)
        .function("player_action_buy_property", &GameState::player_action_buy_property)
        .function("player_action_auction_property", &GameState::player_action_auction_property)
        .function("player_action_mortgage", &GameState::player_action_mortgage)
        .function("player_action_unmortgage", &GameState::player_action_unmortgage)
        .function("player_action_buy_building", &GameState::player_action_buy_building)
        .function("player_action_sell_building", &GameState::player_action_sell_building)
        .function("player_action_sell_all_buildings", &GameState::player_action_sell_all_buildings)
        .function("player_action_bid", &GameState::player_action_bid)
        .function("player_action_decline_bid", &GameState::player_action_decline_bid)
        .function("player_action_offer_trade", &GameState::player_action_offer_trade)
        .function("player_action_decline_trade", &GameState::player_action_decline_trade)
        .function("player_action_end_turn", &GameState::player_action_end_turn)
        .function("player_action_resign", &GameState::player_action_resign)
        .function("force_start_turn", &GameState::force_start_turn)
        .function("force_finish_turn", &GameState::force_finish_turn)
        .function("force_funds", &GameState::force_funds)
        .function("force_add_funds", &GameState::force_add_funds)
        .function("force_subtract_funds", &GameState::force_subtract_funds)
        .function("force_transfer_funds", &GameState::force_transfer_funds)
        .function("force_go_to_jail", &GameState::force_go_to_jail)
        .function("force_pay_bail", &GameState::force_pay_bail)
        .function("force_leave_jail", &GameState::force_leave_jail)
        .function("force_roll", &GameState::force_roll)
        .function("force_advance", &GameState::force_advance)
        .function("force_advance_without_landing", &GameState::force_advance_without_landing)
        .function("force_advance_to", &GameState::force_advance_to)
        .function("force_advance_to_without_landing", &GameState::force_advance_to_without_landing)
        .function("force_land", &GameState::force_land)
        .function("force_position", &GameState::force_position)
        .function("force_property_offer", &GameState::force_property_offer)
        .function("force_stack_deck", &GameState::force_stack_deck)
        .function("force_draw_chance_card", &GameState::force_draw_chance_card)
        .function("force_draw_community_chest_card", &GameState::force_draw_community_chest_card)
        .function("force_draw_card", &GameState::force_draw_card)
        .function("force_income_tax", &GameState::force_income_tax)
        .function("force_luxury_tax", &GameState::force_luxury_tax)
        .function("force_give_deed", &GameState::force_give_deed)
        .function("force_give_deeds", &GameState::force_give_deeds)
        .function("force_transfer_deed", &GameState::force_transfer_deed)
        .function("force_transfer_deeds", &GameState::force_transfer_deeds)
        .function("force_set_mortgaged", &GameState::force_set_mortgaged)
        .function("force_sell_all_buildings", &GameState::force_sell_all_buildings)
        .function("force_add_building", &GameState::force_add_building)
        .function("force_remove_building", &GameState::force_remove_building)
        .function("force_set_building_levels", &GameState::force_set_building_levels)
        .function("force_give_get_out_of_jail_free_card", &GameState::force_give_get_out_of_jail_free_card)
        .function("force_transfer_get_out_of_jail_free_card", &GameState::force_transfer_get_out_of_jail_free_card)
        .function("force_transfer_get_out_of_jail_free_cards", &GameState::force_transfer_get_out_of_jail_free_cards)
        .function("force_return_get_out_of_jail_free_card", &GameState::force_return_get_out_of_jail_free_card)
        .function("force_return_get_out_of_jail_free_cards", &GameState::force_return_get_out_of_jail_free_cards)
        .function("force_bankrupt_by_bank", &GameState::force_bankrupt_by_bank)
        .function("force_bankrupt_by_player", &GameState::force_bankrupt_by_player)
        .function("force_property_offer_prompt", &GameState::force_property_offer_prompt)
        .function("force_liquidate_to_pay_bank_prompt", &GameState::force_liquidate_to_pay_bank_prompt)
        .function("force_liquidate_to_pay_player_prompt", &GameState::force_liquidate_to_pay_player_prompt)
        .function("force_trade", &GameState::force_trade)
        .function("force_transfer_promise", &GameState::force_transfer_promise)
    ;

    // Game.h
    class_<Game>("Game")
        .constructor<IInterface *>()
        .function("get_state", &Game::get_state)
        .function("set_state", &Game::set_state)
        .function("reset", &Game::reset)
        .function("process", &Game::process)
        ;

}

#endif

