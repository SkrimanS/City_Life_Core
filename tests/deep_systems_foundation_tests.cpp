#include "clc/CityLifeCore.hpp"

#include <cassert>
#include <string_view>

namespace {

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    assert(registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    }).ok());
    assert(registry.add(clc::data::ResourceDefinition{
        .id = "tools",
        .display_name = "Tools",
        .category = "crafted",
        .base_value = 40,
    }).ok());
    return registry;
}

void market_snapshot_tracks_shortage_surplus_and_digest() {
    auto registry = make_registry();
    clc::sim::ResourceStorage storage;
    assert(storage.add("grain", 5).ok());
    assert(storage.add("tools", 100).ok());

    clc::economy::MarketState market;
    assert(market.set_demand("grain", 10).ok());
    assert(market.set_demand("tools", 25).ok());

    const auto report = clc::economy::make_market_report(registry, storage, market);
    const auto snapshot = clc::economy::make_market_snapshot(report);

    assert(snapshot.signals.size() == 2);
    assert(snapshot.shortage_count == 1);
    assert(snapshot.surplus_count == 1);
    assert(snapshot.depleted_count == 0);
    assert(snapshot.total_supply == 105);
    assert(snapshot.total_demand == 35);

    const auto* grain = clc::economy::market_signal_by_resource(snapshot, "grain");
    assert(grain != nullptr);
    assert(grain->pressure == clc::economy::MarketPressureLevel::shortage);
    assert(!grain->can_fulfill_demand);
    assert(grain->absolute_gap == 5);

    const auto* tools = clc::economy::market_signal_by_resource(snapshot, "tools");
    assert(tools != nullptr);
    assert(tools->pressure == clc::economy::MarketPressureLevel::surplus);
    assert(tools->can_fulfill_demand);

    const auto digest = clc::economy::market_snapshot_digest(snapshot);
    assert(digest.find("shortage=1") != std::string_view::npos);
    assert(digest.find("surplus=1") != std::string_view::npos);
}

void ledger_summary_tracks_resource_flows() {
    clc::economy::EconomyLedger ledger;
    clc::economy::TradeResult buy{
        .ok = true,
        .resource_id = "grain",
        .quantity = 4,
        .unit_price = 10,
        .total_price = 40,
        .message = "bought resource",
    };
    clc::economy::TradeResult sell{
        .ok = true,
        .resource_id = "grain",
        .quantity = 2,
        .unit_price = 12,
        .total_price = 24,
        .message = "sold resource",
    };

    assert(ledger.record(clc::economy::LedgerEntryType::buy, buy, "stockpile"));
    assert(ledger.record(clc::economy::LedgerEntryType::sell, sell, "market sale"));
    assert(ledger.record_contract_reward("contract-1", "grain", 3, 60, "delivery reward"));

    const auto summary = clc::economy::make_ledger_summary(ledger);
    assert(summary.entry_count == 3);
    assert(summary.buy_count == 1);
    assert(summary.sell_count == 1);
    assert(summary.contract_reward_count == 1);
    assert(summary.total_buy_value == 40);
    assert(summary.total_sell_value == 24);
    assert(summary.total_contract_reward_value == 60);

    const auto* grain = clc::economy::ledger_resource_summary_by_id(summary, "grain");
    assert(grain != nullptr);
    assert(grain->bought_quantity == 4);
    assert(grain->sold_quantity == 2);
    assert(grain->contract_reward_quantity == 3);
    assert(grain->net_quantity == 1);
    assert(grain->net_value == 44);

    const auto digest = clc::economy::ledger_summary_digest(summary);
    assert(digest.find("entries=3") != std::string_view::npos);
    assert(digest.find("resources=1") != std::string_view::npos);
}

void faction_access_and_contract_lifecycle_are_connected() {
    clc::sim::FactionCatalog factions;
    assert(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "city", .display_name = "City"}).ok());
    assert(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "guild", .display_name = "Guild"}).ok());
    assert(clc::sim::set_faction_reputation(factions, "city", "guild", -90).ok());

    const auto access = clc::sim::make_faction_access_report(factions, "city", "guild");
    assert(access.access == clc::sim::FactionAccessLevel::blocked);
    assert(!access.can_trade);
    assert(!access.can_issue_contract);

    clc::sim::ContractCatalog contracts;
    assert(clc::sim::add_contract(contracts, clc::sim::ResourceDeliveryContract{
        .id = "deliver-grain",
        .display_name = "Deliver Grain",
        .issuer_faction_id = "city",
        .receiver_faction_id = "guild",
        .resource_id = "grain",
        .quantity = 10,
        .reward_coins = 100,
        .due_day = 1,
    }).ok());

    const auto summary = clc::sim::make_contract_lifecycle_summary_for_factions(
        contracts,
        factions,
        clc::days_to_ticks(2)
    );
    assert(summary.total_count == 1);
    assert(summary.open_count == 1);
    assert(summary.overdue_open_count == 1);
    assert(summary.blocked_by_reputation_contract_ids.size() == 1);
    assert(summary.blocked_by_reputation_contract_ids.front() == "deliver-grain");

    const auto digest = clc::sim::contract_lifecycle_summary_digest(summary);
    assert(digest.find("blocked_by_reputation=1") != std::string_view::npos);
}

} // namespace

int main() {
    market_snapshot_tracks_shortage_surplus_and_digest();
    ledger_summary_tracks_resource_flows();
    faction_access_and_contract_lifecycle_are_connected();
    return 0;
}
