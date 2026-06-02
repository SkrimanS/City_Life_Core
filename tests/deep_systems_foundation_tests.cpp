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
    assert(registry.add(clc::data::BuildingDefinition{
        .id = "toolsmith",
        .display_name = "Toolsmith",
        .category = "production",
        .worker_slots = 3,
        .required_profession_id = "smith",
        .input_resource_ids = {"grain"},
        .output_resource_ids = {"tools"},
    }).ok());
    assert(registry.add(clc::data::ProfessionDefinition{
        .id = "smith",
        .display_name = "Smith",
        .category = "crafting",
    }).ok());
    return registry;
}

clc::economy::MarketSnapshot make_tools_shortage_market_snapshot(const clc::data::DataRegistry& registry) {
    clc::sim::ResourceStorage market_storage;
    assert(!market_storage.add("tools", 0).ok());
    assert(market_storage.add("grain", 8).ok());
    clc::economy::MarketState market;
    assert(market.set_demand("tools", 20).ok());
    const auto market_report = clc::economy::make_market_report(registry, market_storage, market);
    return clc::economy::make_market_snapshot(market_report);
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

void production_snapshot_tracks_workers_inputs_and_market_pressure() {
    auto registry = make_registry();
    clc::sim::SettlementState settlement{
        .id = "settlement-a",
        .display_name = "Settlement A",
        .population = 50,
    };
    assert(settlement.storage.add("grain", 8).ok());
    settlement.buildings.push_back(clc::sim::BuildingInstance{.definition_id = "toolsmith", .assigned_workers = 2});

    const auto market_snapshot = make_tools_shortage_market_snapshot(registry);
    const auto snapshot = clc::sim::make_settlement_production_snapshot_with_market(
        settlement,
        registry,
        market_snapshot
    );

    assert(snapshot.building_count == 1);
    assert(snapshot.active_building_count == 1);
    assert(snapshot.blocked_building_count == 0);
    assert(snapshot.total_worker_slots == 3);
    assert(snapshot.assigned_workers == 2);
    assert(snapshot.idle_worker_slots == 1);
    assert(snapshot.missing_input_count == 0);
    assert(snapshot.pressured_output_count == 1);
    assert(snapshot.highest_pressure_output_resource_id == "tools");

    const auto* building = clc::sim::production_signal_by_building(snapshot, "toolsmith");
    assert(building != nullptr);
    assert(building->has_workers);
    assert(building->has_required_inputs);
    assert(building->output_has_market_pressure);

    const auto* tools = clc::sim::production_resource_signal(*building, "tools", clc::sim::ProductionResourceRole::output);
    assert(tools != nullptr);
    assert(tools->market_shortage);
    assert(tools->market_pressure == clc::economy::MarketPressureLevel::depleted);

    const auto digest = clc::sim::settlement_production_snapshot_digest(snapshot);
    assert(digest.find("active=1") != std::string_view::npos);
    assert(digest.find("pressured_outputs=1") != std::string_view::npos);
}

void logistics_snapshot_tracks_cargo_shortage_and_contract_support() {
    auto registry = make_registry();
    const auto market_snapshot = make_tools_shortage_market_snapshot(registry);

    clc::sim::SettlementRouteCatalog routes;
    const auto route = clc::sim::make_settlement_route_days("route-a", "Route A", "origin", "destination", 2);
    assert(clc::sim::add_settlement_route(routes, route).ok());

    clc::sim::ResourceStorage cargo;
    assert(cargo.add("tools", 5).ok());
    auto caravan = clc::sim::create_caravan_for_route(route, "caravan-a", "Caravan A", cargo);
    assert(clc::sim::advance_caravan_day(caravan).moved);

    clc::sim::CaravanFleet fleet;
    assert(clc::sim::add_caravan(fleet, caravan).ok());

    clc::sim::ContractCatalog contracts;
    assert(clc::sim::add_contract(contracts, clc::sim::ResourceDeliveryContract{
        .id = "deliver-tools",
        .display_name = "Deliver Tools",
        .issuer_faction_id = "city",
        .receiver_faction_id = "guild",
        .resource_id = "tools",
        .quantity = 5,
        .reward_coins = 120,
        .due_day = 3,
    }).ok());

    const auto snapshot = clc::sim::make_logistics_network_snapshot_with_market_and_contracts(
        routes,
        fleet,
        market_snapshot,
        contracts
    );

    assert(snapshot.route_count == 1);
    assert(snapshot.caravan_count == 1);
    assert(snapshot.active_caravan_count == 1);
    assert(snapshot.arrived_caravan_count == 0);
    assert(snapshot.total_cargo == 5);
    assert(snapshot.cargo_supporting_shortage_count == 1);
    assert(snapshot.cargo_supporting_contract_count == 1);

    const auto* caravan_signal = clc::sim::logistics_signal_by_caravan(snapshot, "caravan-a");
    assert(caravan_signal != nullptr);
    assert(!caravan_signal->empty);
    assert(caravan_signal->route_known);

    const auto* tools = clc::sim::logistics_cargo_signal(*caravan_signal, "tools");
    assert(tools != nullptr);
    assert(tools->supports_market_shortage);
    assert(tools->may_fulfill_open_contract);
    assert(tools->market_pressure == clc::economy::MarketPressureLevel::depleted);

    const auto digest = clc::sim::logistics_network_snapshot_digest(snapshot);
    assert(digest.find("shortage_cargo=1") != std::string_view::npos);
    assert(digest.find("contract_cargo=1") != std::string_view::npos);
}

void deep_systems_diagnostics_summarizes_cross_system_pressure() {
    auto registry = make_registry();
    const auto market_snapshot = make_tools_shortage_market_snapshot(registry);

    clc::economy::EconomyLedger ledger;
    const auto ledger_summary = clc::economy::make_ledger_summary(ledger);

    clc::sim::SettlementState settlement{
        .id = "settlement-a",
        .display_name = "Settlement A",
        .population = 50,
    };
    settlement.buildings.push_back(clc::sim::BuildingInstance{.definition_id = "toolsmith", .assigned_workers = 2});
    const auto production = clc::sim::make_settlement_production_snapshot_with_market(settlement, registry, market_snapshot);

    clc::sim::ContractCatalog contracts;
    assert(clc::sim::add_contract(contracts, clc::sim::ResourceDeliveryContract{
        .id = "deliver-tools",
        .display_name = "Deliver Tools",
        .issuer_faction_id = "city",
        .receiver_faction_id = "guild",
        .resource_id = "tools",
        .quantity = 5,
        .reward_coins = 120,
        .due_day = 1,
    }).ok());
    const auto contract_summary = clc::sim::make_contract_lifecycle_summary(contracts, clc::days_to_ticks(2));

    clc::sim::SettlementRouteCatalog routes;
    clc::sim::CaravanFleet fleet;
    const auto logistics = clc::sim::make_logistics_network_snapshot_with_market_and_contracts(routes, fleet, market_snapshot, contracts);

    const auto diagnostics = clc::sim::make_deep_systems_diagnostics(
        market_snapshot,
        ledger_summary,
        contract_summary,
        production,
        logistics
    );

    assert(clc::sim::deep_systems_diagnostics_has_warnings(diagnostics));
    assert(clc::sim::deep_systems_diagnostics_has_critical_warnings(diagnostics));
    assert(diagnostics.warning_count >= 4);
    assert(clc::sim::deep_systems_warning_by_prefix(diagnostics, "market:") != nullptr);
    assert(clc::sim::deep_systems_warning_by_prefix(diagnostics, "contracts:") != nullptr);
    assert(clc::sim::deep_systems_warning_by_prefix(diagnostics, "production:") != nullptr);

    const auto digest = clc::sim::deep_systems_diagnostics_digest(diagnostics);
    assert(digest.find("critical=") != std::string_view::npos);
    assert(digest.find("market_depleted=1") != std::string_view::npos);

    const auto markdown = clc::sim::deep_systems_diagnostics_markdown(diagnostics);
    assert(markdown.find("# Deep systems diagnostics") != std::string_view::npos);
    assert(markdown.find("## Warnings") != std::string_view::npos);
}

} // namespace

int main() {
    market_snapshot_tracks_shortage_surplus_and_digest();
    ledger_summary_tracks_resource_flows();
    faction_access_and_contract_lifecycle_are_connected();
    production_snapshot_tracks_workers_inputs_and_market_pressure();
    logistics_snapshot_tracks_cargo_shortage_and_contract_support();
    deep_systems_diagnostics_summarizes_cross_system_pressure();
    return 0;
}
