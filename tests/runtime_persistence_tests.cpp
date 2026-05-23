#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

void add_tick_remainder_to_first_settlement(clc::sim::SimulationEngine& engine) {
    auto state = engine.export_state();
    require(!state.settlements.empty(), "engine should have settlement for tick remainder setup");
    state.settlements[0].tick_remainders.push_back(clc::sim::SettlementTickRemainder{
        .key = "food:grain",
        .numerator = 1,
    });
    require(engine.restore_state(std::move(state)).ok(), "engine should restore tick remainder setup state");
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "runtime scenario should bootstrap");

    auto& runtime = bootstrap.runtime;
    auto& source_engine = runtime.engine;
    auto& routes = runtime.routes;
    auto& caravans = runtime.caravans;
    auto& factions = runtime.factions;
    auto& ownership = runtime.ownership;
    auto& contracts = runtime.contracts;
    auto& wallet = runtime.wallet;
    auto& ledger = runtime.ledger;

    require(source_engine.add_resource_to_settlement("riverwatch", "grain", 50).ok(), "source engine should add more grain");
    require(source_engine.transfer_resource_between_settlements("riverwatch", "hillford", "grain", 20).ok(), "source engine should transfer grain");
    require(source_engine.run_days(3).size() == 3, "source engine should run three days");

    auto default_caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "caravan_001",
        "Runtime Caravan 001"
    );
    require(default_caravan.ok(), "runtime should create default caravan");

    auto hourly_route = clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford_hours",
        "Riverwatch to Hillford Hours",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(3)
    );
    require(clc::sim::add_settlement_route(routes, std::move(hourly_route)).ok(), "runtime should add hourly route");

    auto hourly_caravan = clc::sim::create_caravan_for_route(
        routes.routes[1],
        "hourly_caravan_001",
        "Hourly Caravan"
    );
    require(clc::sim::add_caravan(caravans, std::move(hourly_caravan)).ok(), "runtime should add hourly caravan");
    const auto hourly_advance = clc::sim::advance_caravan_ticks(caravans.caravans[1], clc::hours_to_ticks(1));
    require(hourly_advance.ticks_elapsed == clc::hours_to_ticks(1), "runtime should advance hourly caravan");

    require(clc::sim::advance_runtime_caravan_day(runtime, "caravan_001").ok(), "runtime should advance default caravan");
    add_tick_remainder_to_first_settlement(source_engine);

    require(clc::sim::set_runtime_caravan_owner(runtime, "caravan_001", "riverwatch").ok(), "runtime should set first caravan owner");
    require(clc::sim::set_runtime_caravan_owner(runtime, "hourly_caravan_001", "hillford").ok(), "runtime should set second caravan owner");
    wallet.coins = 45;
    require(ledger.record_contract_reward("before_runtime_save", "grain", 2, 5, "before save"), "runtime ledger should record reward");

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_persistence_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "runtime.clcs";

    const auto save_result = clc::sim::save_simulation_runtime_to_file(
        source_engine,
        routes,
        caravans,
        factions,
        ownership,
        contracts,
        wallet,
        ledger,
        file_path
    );
    require(save_result.ok(), "runtime should save to file");

    clc::sim::SimulationEngine target_engine{clc::sim::make_basic_runtime_scenario_registry()};
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
    require(target_routes.routes.size() == 2, "runtime load should restore routes");
    require(target_routes.routes[1].travel_ticks == clc::hours_to_ticks(3), "runtime load should restore hourly route ticks");
    require(target_caravans.caravans.size() == 2, "runtime load should restore caravans");
    require(target_caravans.caravans[0].days_remaining == 1, "runtime load should restore caravan progress");
    require(target_caravans.caravans[0].ticks_remaining == clc::days_to_ticks(1), "runtime load should restore caravan tick progress");
    require(target_caravans.caravans[1].total_travel_ticks == clc::hours_to_ticks(3), "runtime load should restore hourly caravan total ticks");
    require(target_caravans.caravans[1].ticks_remaining == clc::hours_to_ticks(2), "runtime load should restore hourly caravan remaining ticks");
    const auto target_state = target_engine.export_state();
    require(target_state.settlements[0].tick_remainders.size() == 1, "runtime load should restore settlement tick remainders");
    require(target_state.settlements[0].tick_remainders[0].key == "food:grain", "runtime load should restore settlement tick remainder key");
    require(target_factions.factions.size() == 2, "runtime load should restore factions");
    require(target_ownership.caravans.size() == 2, "runtime load should restore ownership");
    require(target_contracts.contracts.size() == 1, "runtime load should restore contracts");
    require(target_wallet.coins == 45, "runtime load should restore wallet");
    require(target_ledger.next_sequence() == 2, "runtime load should restore live ledger sequence");
    require(target_ledger.record_contract_reward("after_runtime_load", "grain", 1, 2, "after load"), "runtime-loaded ledger should continue recording");
    require(target_ledger.entries()[1].sequence == 2, "runtime-loaded ledger should continue sequence");
    require(target_engine.advance_day().day == 5, "runtime-loaded engine should continue simulation");

    const auto serialized = clc::sim::serialize_simulation_world_state(load_result.state);
    require(serialized.find("settlement_tick_remainder") != std::string::npos, "serialized runtime should include settlement tick remainders");
    require(serialized.find("riverwatch_to_hillford_hours") != std::string::npos, "serialized runtime should include hourly route");
    require(serialized.find("hourly_caravan_001") != std::string::npos, "serialized runtime should include hourly caravan");

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

    clc::sim::SimulationEngine broken_engine{clc::sim::make_basic_runtime_scenario_registry()};
    clc::sim::SettlementRouteCatalog broken_routes;
    clc::sim::CaravanFleet broken_caravans;
    clc::sim::FactionCatalog broken_factions;
    clc::sim::OwnershipCatalog broken_ownership;
    clc::sim::ContractCatalog broken_contracts;
    clc::economy::Wallet broken_wallet;
    clc::economy::EconomyLedger broken_ledger;

    const auto broken_load = clc::sim::load_simulation_runtime_from_file(
        file_path,
        broken_engine,
        broken_routes,
        broken_caravans,
        broken_factions,
        broken_ownership,
        broken_contracts,
        broken_wallet,
        broken_ledger
    );
    require(!broken_load.ok(), "runtime load should reject corrupted route reference");

    std::filesystem::remove_all(directory);
    return 0;
}
