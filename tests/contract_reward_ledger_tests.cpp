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

    return 0;
}
