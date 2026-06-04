#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <string_view>

namespace {

void require(bool condition) {
    if (!condition) {
        std::abort();
    }
}

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    }).ok());
    require(registry.add(clc::data::ResourceDefinition{
        .id = "tools",
        .display_name = "Tools",
        .category = "crafted",
        .base_value = 40,
    }).ok());
    require(registry.add(clc::data::BuildingDefinition{
        .id = "toolsmith",
        .display_name = "Toolsmith",
        .category = "production",
        .worker_slots = 3,
        .required_profession_id = "smith",
        .input_resource_ids = {"grain"},
        .output_resource_ids = {"tools"},
    }).ok());
    require(registry.add(clc::data::ProfessionDefinition{
        .id = "smith",
        .display_name = "Smith",
        .category = "crafting",
    }).ok());
    return registry;
}

clc::economy::MarketSnapshot make_tools_shortage_market_snapshot(const clc::data::DataRegistry& registry) {
    clc::sim::ResourceStorage market_storage;
    require(!market_storage.add("tools", 0).ok());
    require(market_storage.add("grain", 8).ok());
    clc::economy::MarketState market;
    require(market.set_demand("tools", 20).ok());
    const auto market_report = clc::economy::make_market_report(registry, market_storage, market);
    return clc::economy::make_market_snapshot(market_report);
}

void market_snapshot_tracks_shortage_surplus_and_digest() {
    auto registry = make_registry();
    clc::sim::ResourceStorage storage;
    require(storage.add("grain", 5).ok());
    require(storage.add("tools", 100).ok());

    clc::economy::MarketState market;
    require(market.set_demand("grain", 10).ok());
    require(market.set_demand("tools", 25).ok());

    const auto report = clc::economy::make_market_report(registry, storage, market);
    const auto snapshot = clc::economy::make_market_snapshot(report);

    require(snapshot.signals.size() == 2);
    require(snapshot.shortage_count == 1);
    require(snapshot.surplus_count == 1);
    require(snapshot.depleted_count == 0);
    require(snapshot.total_supply == 105);
    require(snapshot.total_demand == 35);

    const auto* grain = clc::economy::market_signal_by_resource(snapshot, "grain");
    require(grain != nullptr);
    require(grain->pressure == clc::economy::MarketPressureLevel::shortage);
    require(!grain->can_fulfill_demand);
    require(grain->absolute_gap == 5);

    const auto* tools = clc::economy::market_signal_by_resource(snapshot, "tools");
    require(tools != nullptr);
    require(tools->pressure == clc::economy::MarketPressureLevel::surplus);
    require(tools->can_fulfill_demand);

    const auto digest = clc::economy::market_snapshot_digest(snapshot);
    require(digest.find("shortage=1") != std::string_view::npos);
    require(digest.find("surplus=1") != std::string_view::npos);
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

    require(ledger.record(clc::economy::LedgerEntryType::buy, buy, "stockpile"));
    require(ledger.record(clc::economy::LedgerEntryType::sell, sell, "market sale"));
    require(ledger.record_contract_reward("contract-1", "grain", 3, 60, "delivery reward"));

    const auto summary = clc::economy::make_ledger_summary(ledger);
    require(summary.entry_count == 3);
    require(summary.buy_count == 1);
    require(summary.sell_count == 1);
    require(summary.contract_reward_count == 1);
    require(summary.total_buy_value == 40);
    require(summary.total_sell_value == 24);
    require(summary.total_contract_reward_value == 60);

    const auto* grain = clc::economy::ledger_resource_summary_by_id(summary, "grain");
    require(grain != nullptr);
    require(grain->bought_quantity == 4);
    require(grain->sold_quantity == 2);
    require(grain->contract_reward_quantity == 3);
    require(grain->net_quantity == 1);
    require(grain->net_value == 44);

    const auto digest = clc::economy::ledger_summary_digest(summary);
    require(digest.find("entries=3") != std::string_view::npos);
    require(digest.find("resources=1") != std::string_view::npos);
}

void faction_access_and_contract_lifecycle_are_connected() {
    clc::sim::FactionCatalog factions;
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "city", .display_name = "City"}).ok());
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "guild", .display_name = "Guild"}).ok());
    require(clc::sim::set_faction_reputation(factions, "city", "guild", -90).ok());

    const auto access = clc::sim::make_faction_access_report(factions, "city", "guild");
    require(access.access == clc::sim::FactionAccessLevel::blocked);
    require(!access.can_trade);
    require(!access.can_issue_contract);

    clc::sim::ContractCatalog contracts;
    require(clc::sim::add_contract(contracts, clc::sim::ResourceDeliveryContract{
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
    require(summary.total_count == 1);
    require(summary.open_count == 1);
    require(summary.overdue_open_count == 1);
    require(summary.blocked_by_reputation_contract_ids.size() == 1);
    require(summary.blocked_by_reputation_contract_ids.front() == "deliver-grain");

    const auto digest = clc::sim::contract_lifecycle_summary_digest(summary);
    require(digest.find("blocked_by_reputation=1") != std::string_view::npos);
}

void production_snapshot_tracks_workers_inputs_and_market_pressure() {
    auto registry = make_registry();
    clc::sim::SettlementState settlement{
        .id = "settlement-a",
        .display_name = "Settlement A",
        .population = 50,
    };
    require(settlement.storage.add("grain", 8).ok());
    settlement.buildings.push_back(clc::sim::BuildingInstance{.definition_id = "toolsmith", .assigned_workers = 2});

    const auto market_snapshot = make_tools_shortage_market_snapshot(registry);
    const auto snapshot = clc::sim::make_settlement_production_snapshot_with_market(
        settlement,
        registry,
        market_snapshot
    );

    require(snapshot.building_count == 1);
    require(snapshot.active_building_count == 1);
    require(snapshot.blocked_building_count == 0);
    require(snapshot.total_worker_slots == 3);
    require(snapshot.assigned_workers == 2);
    require(snapshot.idle_worker_slots == 1);
    require(snapshot.missing_input_count == 0);
    require(snapshot.pressured_output_count == 1);
    require(snapshot.highest_pressure_output_resource_id == "tools");

    const auto* building = clc::sim::production_signal_by_building(snapshot, "toolsmith");
    require(building != nullptr);
    require(building->has_workers);
    require(building->has_required_inputs);
    require(building->output_has_market_pressure);

    const auto* tools = clc::sim::production_resource_signal(*building, "tools", clc::sim::ProductionResourceRole::output);
    require(tools != nullptr);
    require(tools->market_shortage);
    require(tools->market_pressure == clc::economy::MarketPressureLevel::depleted);

    const auto digest = clc::sim::settlement_production_snapshot_digest(snapshot);
    require(digest.find("active=1") != std::string_view::npos);
    require(digest.find("pressured_outputs=1") != std::string_view::npos);
}

void logistics_snapshot_tracks_cargo_shortage_and_contract_support() {
    auto registry = make_registry();
    const auto market_snapshot = make_tools_shortage_market_snapshot(registry);

    clc::sim::SettlementRouteCatalog routes;
    const auto route = clc::sim::make_settlement_route_days("route-a", "Route A", "origin", "destination", 2);
    require(clc::sim::add_settlement_route(routes, route).ok());

    clc::sim::ResourceStorage cargo;
    require(cargo.add("tools", 5).ok());
    auto caravan = clc::sim::create_caravan_for_route(route, "caravan-a", "Caravan A", cargo);
    require(clc::sim::advance_caravan_day(caravan).moved);

    clc::sim::CaravanFleet fleet;
    require(clc::sim::add_caravan(fleet, caravan).ok());

    clc::sim::ContractCatalog contracts;
    require(clc::sim::add_contract(contracts, clc::sim::ResourceDeliveryContract{
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

    require(snapshot.route_count == 1);
    require(snapshot.caravan_count == 1);
    require(snapshot.active_caravan_count == 1);
    require(snapshot.arrived_caravan_count == 0);
    require(snapshot.total_cargo == 5);
    require(snapshot.cargo_supporting_shortage_count == 1);
    require(snapshot.cargo_supporting_contract_count == 1);

    const auto* caravan_signal = clc::sim::logistics_signal_by_caravan(snapshot, "caravan-a");
    require(caravan_signal != nullptr);
    require(!caravan_signal->empty);
    require(caravan_signal->route_known);

    const auto* tools = clc::sim::logistics_cargo_signal(*caravan_signal, "tools");
    require(tools != nullptr);
    require(tools->supports_market_shortage);
    require(tools->may_fulfill_open_contract);
    require(tools->market_pressure == clc::economy::MarketPressureLevel::depleted);

    const auto digest = clc::sim::logistics_network_snapshot_digest(snapshot);
    require(digest.find("shortage_cargo=1") != std::string_view::npos);
    require(digest.find("contract_cargo=1") != std::string_view::npos);
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
    require(clc::sim::add_contract(contracts, clc::sim::ResourceDeliveryContract{
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

    require(clc::sim::deep_systems_diagnostics_has_warnings(diagnostics));
    require(clc::sim::deep_systems_diagnostics_has_critical_warnings(diagnostics));
    require(diagnostics.warning_count >= 4);
    require(clc::sim::deep_systems_warning_by_prefix(diagnostics, "market:") != nullptr);
    require(clc::sim::deep_systems_warning_by_prefix(diagnostics, "contracts:") != nullptr);
    require(clc::sim::deep_systems_warning_by_prefix(diagnostics, "production:") != nullptr);

    const auto digest = clc::sim::deep_systems_diagnostics_digest(diagnostics);
    require(digest.find("critical=") != std::string_view::npos);
    require(digest.find("market_depleted=1") != std::string_view::npos);

    const auto markdown = clc::sim::deep_systems_diagnostics_markdown(diagnostics);
    require(markdown.find("# Deep systems diagnostics") != std::string_view::npos);
    require(markdown.find("## Warnings") != std::string_view::npos);
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
