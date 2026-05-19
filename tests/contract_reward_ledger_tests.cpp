#include "clc/sim/Contracts.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::ResourceDeliveryContract make_contract() {
    return clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery_ledger",
        .display_name = "Ledger Grain Delivery",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 50,
        .reward_coins = 120,
        .due_day = 7,
    };
}

clc::sim::SettlementRoute make_route() {
    return clc::sim::SettlementRoute{
        .id = "riverwatch_to_traders",
        .display_name = "Riverwatch to Traders Guild",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "traders_guild",
        .travel_days = 2,
    };
}

} // namespace

int main() {
    clc::sim::ContractCatalog catalog;
    require(clc::sim::add_contract(catalog, make_contract()).ok(), "ledger contract should add");

    clc::sim::ResourceStorage delivered;
    require(delivered.add("grain", 60).ok(), "delivered storage should accept grain");

    clc::economy::Wallet wallet{.coins = 5};
    clc::economy::EconomyLedger ledger;

    const auto result = clc::sim::fulfill_contract_from_storage_with_reward_and_ledger(
        catalog,
        "grain_delivery_ledger",
        delivered,
        wallet,
        ledger
    );

    require(result.ok(), "contract reward ledger fulfillment should succeed");
    require(delivered.amount("grain") == 10, "fulfillment should debit delivered grain");
    require(wallet.coins == 125, "fulfillment should credit reward wallet");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_ledger")->status == clc::sim::ContractStatus::fulfilled, "fulfillment should mark contract fulfilled");
    require(ledger.entries().size() == 1, "ledger should contain one reward entry");
    require(ledger.entries()[0].type == clc::economy::LedgerEntryType::contract_reward, "ledger entry should be contract reward");
    require(ledger.entries()[0].reference_id == "grain_delivery_ledger", "ledger entry should store contract id");
    require(ledger.entries()[0].resource_id == "grain", "ledger entry should store resource id");
    require(ledger.entries()[0].quantity == 50, "ledger entry should store delivered quantity");
    require(ledger.entries()[0].total_price == 120, "ledger entry should store reward coins");
    require(ledger.total_contract_rewards("grain") == 120, "ledger should aggregate contract rewards");

    auto failed_contract = make_contract();
    failed_contract.id = "grain_delivery_ledger_failed";
    failed_contract.display_name = "Failed Ledger Grain Delivery";
    require(clc::sim::add_contract(catalog, failed_contract).ok(), "failed ledger contract should add");

    clc::sim::ResourceStorage insufficient;
    require(insufficient.add("grain", 49).ok(), "insufficient storage should accept grain");
    const auto failed = clc::sim::fulfill_contract_from_storage_with_reward_and_ledger(
        catalog,
        "grain_delivery_ledger_failed",
        insufficient,
        wallet,
        ledger
    );

    require(!failed.ok(), "failed fulfillment should not record ledger entry");
    require(insufficient.amount("grain") == 49, "failed fulfillment should not debit storage");
    require(wallet.coins == 125, "failed fulfillment should not credit wallet");
    require(ledger.entries().size() == 1, "failed fulfillment should not add ledger entry");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_ledger_failed")->status == clc::sim::ContractStatus::open, "failed fulfillment should leave contract open");

    auto caravan_contract = make_contract();
    caravan_contract.id = "grain_delivery_caravan_ledger";
    caravan_contract.display_name = "Caravan Ledger Grain Delivery";
    require(clc::sim::add_contract(catalog, caravan_contract).ok(), "caravan ledger contract should add");

    auto caravan = clc::sim::create_caravan_for_route(make_route(), "ledger_caravan", "Ledger Caravan");
    require(caravan.cargo.add("grain", 60).ok(), "caravan cargo should accept grain");
    clc::economy::Wallet caravan_wallet{.coins = 10};

    const auto early_caravan = clc::sim::fulfill_contract_from_arrived_caravan_with_reward_and_ledger(
        catalog,
        "grain_delivery_caravan_ledger",
        caravan,
        caravan_wallet,
        ledger
    );
    require(!early_caravan.ok(), "caravan ledger fulfillment should require arrival");
    require(caravan.cargo.amount("grain") == 60, "early caravan ledger fulfillment should not debit cargo");
    require(caravan_wallet.coins == 10, "early caravan ledger fulfillment should not credit wallet");
    require(ledger.entries().size() == 1, "early caravan ledger fulfillment should not add ledger entry");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_caravan_ledger")->status == clc::sim::ContractStatus::open, "early caravan ledger fulfillment should leave contract open");

    require(clc::sim::advance_caravan_day(caravan).days_remaining_after == 1, "caravan should move day one");
    require(clc::sim::advance_caravan_day(caravan).arrived, "caravan should arrive day two");
    const auto caravan_result = clc::sim::fulfill_contract_from_arrived_caravan_with_reward_and_ledger(
        catalog,
        "grain_delivery_caravan_ledger",
        caravan,
        caravan_wallet,
        ledger
    );
    require(caravan_result.ok(), "arrived caravan ledger fulfillment should succeed");
    require(caravan.cargo.amount("grain") == 10, "caravan ledger fulfillment should debit cargo");
    require(caravan_wallet.coins == 130, "caravan ledger fulfillment should credit wallet");
    require(ledger.entries().size() == 2, "caravan ledger fulfillment should add ledger entry");
    require(ledger.entries()[1].reference_id == "grain_delivery_caravan_ledger", "caravan ledger entry should store contract id");
    require(ledger.total_contract_rewards("grain") == 240, "ledger should aggregate storage and caravan rewards");

    auto owned_contract = make_contract();
    owned_contract.id = "grain_delivery_owned_ledger";
    owned_contract.display_name = "Owned Ledger Grain Delivery";
    require(clc::sim::add_contract(catalog, owned_contract).ok(), "owned ledger contract should add");

    auto owned_caravan = clc::sim::create_caravan_for_route(make_route(), "owned_ledger_caravan", "Owned Ledger Caravan");
    require(owned_caravan.cargo.add("grain", 60).ok(), "owned caravan cargo should accept grain");
    require(clc::sim::advance_caravan_day(owned_caravan).days_remaining_after == 1, "owned caravan should move day one");
    require(clc::sim::advance_caravan_day(owned_caravan).arrived, "owned caravan should arrive day two");

    clc::sim::OwnershipCatalog ownership;
    require(clc::sim::set_caravan_owner(ownership, "owned_ledger_caravan", "traders_guild").ok(), "wrong owner should set");
    clc::economy::Wallet owned_wallet{.coins = 20};
    const auto wrong_owner = clc::sim::fulfill_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        catalog,
        "grain_delivery_owned_ledger",
        owned_caravan,
        ownership,
        "riverwatch",
        owned_wallet,
        ledger
    );
    require(!wrong_owner.ok(), "owned caravan ledger fulfillment should reject wrong owner");
    require(owned_caravan.cargo.amount("grain") == 60, "wrong owner should not debit cargo");
    require(owned_wallet.coins == 20, "wrong owner should not credit wallet");
    require(ledger.entries().size() == 2, "wrong owner should not add ledger entry");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_owned_ledger")->status == clc::sim::ContractStatus::open, "wrong owner should leave contract open");

    require(clc::sim::set_caravan_owner(ownership, "owned_ledger_caravan", "riverwatch").ok(), "expected owner should set");
    const auto owned_result = clc::sim::fulfill_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        catalog,
        "grain_delivery_owned_ledger",
        owned_caravan,
        ownership,
        "riverwatch",
        owned_wallet,
        ledger
    );
    require(owned_result.ok(), "owned caravan ledger fulfillment should succeed");
    require(owned_caravan.cargo.amount("grain") == 10, "owned caravan ledger fulfillment should debit cargo");
    require(owned_wallet.coins == 140, "owned caravan ledger fulfillment should credit wallet");
    require(ledger.entries().size() == 3, "owned caravan ledger fulfillment should add ledger entry");
    require(ledger.entries()[2].reference_id == "grain_delivery_owned_ledger", "owned ledger entry should store contract id");
    require(ledger.total_contract_rewards("grain") == 360, "ledger should aggregate all rewards");

    return 0;
}
