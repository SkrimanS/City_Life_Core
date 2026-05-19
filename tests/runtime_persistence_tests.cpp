#include "clc/sim/SimulationPersistence.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "grain should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "riverwatch", .display_name = "Riverwatch", .starting_population = 120}).ok(), "riverwatch should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "hillford", .display_name = "Hillford", .starting_population = 80}).ok(), "hillford should register");
    return registry;
}

clc::sim::SimulationEngineState make_engine_state() {
    clc::sim::SimulationEngineState state;
    state.current_day = 4;
    clc::sim::SettlementState riverwatch{.id = "riverwatch", .display_name = "Riverwatch", .population = 120};
    require(riverwatch.storage.add("grain", 80).ok(), "riverwatch should accept grain");
    state.settlements.push_back(riverwatch);
    clc::sim::SettlementState hillford{.id = "hillford", .display_name = "Hillford", .population = 80};
    require(hillford.storage.add("grain", 20).ok(), "hillford should accept grain");
    state.settlements.push_back(hillford);
    state.market_demands.push_back(clc::sim::SimulationMarketDemand{.resource_id = "grain", .demand = 70});
    state.events.push_back(clc::sim::SimulationEvent{.day = 4, .type = "runtime", .message = "saved"});
    return state;
}

} // namespace

int main() {
    auto registry = make_registry();
    clc::sim::SimulationEngine source_engine{registry};
    require(source_engine.restore_state(make_engine_state()).ok(), "source engine should restore state");

    clc::sim::SettlementRouteCatalog routes;
    require(clc::sim::add_settlement_route(routes, clc::sim::SettlementRoute{
        .id = "riverwatch_to_hillford",
        .display_name = "Riverwatch to Hillford",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "hillford",
        .travel_days = 2,
    }).ok(), "route should add");

    clc::sim::CaravanFleet caravans;
    auto caravan = clc::sim::create_caravan_for_route(routes.routes[0], "caravan_001", "First Caravan");
    require(caravan.cargo.add("grain", 30).ok(), "caravan cargo should accept grain");
    require(clc::sim::advance_caravan_day(caravan).days_remaining_after == 1, "caravan should advance once");
    require(clc::sim::add_caravan(caravans, caravan).ok(), "caravan should add");

    clc::sim::FactionCatalog factions;
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "riverwatch faction should add");
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "traders_guild", .display_name = "Traders Guild"}).ok(), "traders faction should add");

    clc::sim::OwnershipCatalog ownership;
    require(clc::sim::set_settlement_owner(ownership, "riverwatch", "riverwatch").ok(), "settlement owner should set");
    require(clc::sim::set_caravan_owner(ownership, "caravan_001", "riverwatch").ok(), "caravan owner should set");

    clc::sim::ContractCatalog contracts;
    require(clc::sim::add_contract(contracts, clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery_001",
        .display_name = "Grain Delivery 001",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 30,
        .reward_coins = 75,
        .due_day = 8,
    }).ok(), "contract should add");

    clc::economy::Wallet wallet{.coins = 45};
    clc::economy::EconomyLedger ledger;
    require(ledger.record_contract_reward("bootstrap_reward", "grain", 5, 10, "bootstrap"), "ledger should record bootstrap reward");

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_persistence_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "runtime.clcs";

    require(clc::sim::save_simulation_runtime_to_file(
        source_engine,
        routes,
        caravans,
        factions,
        ownership,
        contracts,
        wallet,
        ledger,
        file_path
    ).ok(), "runtime should save to file");

    clc::sim::SimulationEngine target_engine{registry};
    clc::sim::SettlementRouteCatalog target_routes;
    clc::sim::CaravanFleet target_caravans;
    clc::sim::FactionCatalog target_factions;
    clc::sim::OwnershipCatalog target_ownership;
    clc::sim::ContractCatalog target_contracts;
    clc::economy::Wallet target_wallet;
    clc::economy::EconomyLedger target_ledger;

    const auto load_result = clc::sim::load_simulation_runtime_from_file(
        file_path,
        target_engine,
        target_routes,
        target_caravans,
        target_factions,
        target_ownership,
        target_contracts,
        target_wallet,
        target_ledger
    );
    require(load_result.ok(), "runtime should load from file");
    require(target_engine.current_day() == 4, "runtime load should restore engine day");
    require(target_engine.settlement_resource_amount("riverwatch", "grain") == 80, "runtime load should restore settlement storage");
    require(target_engine.settlement_resource_amount("hillford", "grain") == 20, "runtime load should restore destination settlement storage");
    require(target_routes.routes.size() == 1, "runtime load should restore routes");
    require(target_caravans.caravans.size() == 1, "runtime load should restore caravans");
    require(target_caravans.caravans[0].days_remaining == 1, "runtime load should restore caravan progress");
    require(target_factions.factions.size() == 2, "runtime load should restore factions");
    require(target_ownership.caravans.size() == 1, "runtime load should restore ownership");
    require(target_contracts.contracts.size() == 1, "runtime load should restore contracts");
    require(target_wallet.coins == 45, "runtime load should restore wallet");
    require(target_ledger.next_sequence() == 2, "runtime load should restore live ledger sequence");
    require(target_ledger.record_contract_reward("after_runtime_load", "grain", 1, 2, "after load"), "runtime-loaded ledger should continue recording");
    require(target_ledger.entries()[1].sequence == 2, "runtime-loaded ledger should continue sequence");
    require(target_engine.advance_day().day == 5, "runtime-loaded engine should continue simulation");

    auto broken_state = clc::sim::capture_simulation_world_state(
        source_engine,
        routes,
        caravans,
        factions,
        ownership,
        contracts,
        wallet,
        ledger
    );
    broken_state.caravans.caravans[0].route_id = "missing_route";
    const auto broken_text = clc::sim::serialize_simulation_world_state(broken_state);
    std::ofstream output{file_path};
    output << broken_text;
    output.close();

    clc::sim::SimulationEngine untouched_engine{registry};
    clc::sim::SettlementRouteCatalog untouched_routes;
    clc::sim::CaravanFleet untouched_caravans;
    clc::sim::FactionCatalog untouched_factions;
    clc::sim::OwnershipCatalog untouched_ownership;
    clc::sim::ContractCatalog untouched_contracts;
    clc::economy::Wallet untouched_wallet{.coins = 999};
    clc::economy::EconomyLedger untouched_ledger;
    require(!clc::sim::load_simulation_runtime_from_file(
        file_path,
        untouched_engine,
        untouched_routes,
        untouched_caravans,
        untouched_factions,
        untouched_ownership,
        untouched_contracts,
        untouched_wallet,
        untouched_ledger
    ).ok(), "runtime load should reject invalid referenced world state");
    require(untouched_engine.current_day() == 0, "failed runtime load should not mutate engine");
    require(untouched_routes.routes.empty(), "failed runtime load should not mutate routes");
    require(untouched_wallet.coins == 999, "failed runtime load should not mutate wallet");
    require(untouched_ledger.entries().empty(), "failed runtime load should not mutate ledger");

    std::filesystem::remove_all(directory);
    return 0;
}
