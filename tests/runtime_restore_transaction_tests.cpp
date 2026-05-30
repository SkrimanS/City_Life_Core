#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

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

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "grain should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "riverwatch", .display_name = "Riverwatch", .starting_population = 100}).ok(), "riverwatch should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "hillford", .display_name = "Hillford", .starting_population = 80}).ok(), "hillford should register");
    return registry;
}

void require_original_runtime_preserved(const clc::sim::SimulationRuntime& runtime) {
    require(runtime.engine.has_settlement("riverwatch"), "failed restore should preserve original settlement");
    require(!runtime.engine.has_settlement("hillford"), "failed restore should not partially apply new settlement");
    require(runtime.engine.settlement_resource_amount("riverwatch", "grain") == 7, "failed restore should preserve original resources");
    require(runtime.wallet.coins == 42, "failed restore should preserve wallet");
    require(runtime.time.current_tick() == 123, "failed restore should preserve runtime time");
    require(runtime.ledger.entries().empty(), "failed restore should preserve original ledger");
}

} // namespace

int main() {
    clc::sim::SimulationRuntime runtime{make_registry()};
    require(clc::sim::create_runtime_settlement(runtime, "riverwatch").ok(), "initial settlement should create");
    require(runtime.engine.add_resource_to_settlement("riverwatch", "grain", 7).ok(), "initial settlement should receive grain");
    runtime.wallet.coins = 42;
    runtime.time = clc::GameTime{123};

    auto state = clc::sim::capture_simulation_world_state(runtime);
    state.engine.settlements.clear();
    state.engine.settlements.push_back(clc::sim::SettlementState{.id = "hillford", .display_name = "Hillford", .population = 80});
    state.wallet.coins = 999;
    state.time = clc::GameTime{9999};
    state.ledger_entries.push_back(clc::economy::LedgerEntry{
        .sequence = 2,
        .type = clc::economy::LedgerEntryType::contract_reward,
        .resource_id = "grain",
        .quantity = 1,
        .unit_price = 0,
        .total_price = 5,
        .reference_id = "bad_sequence_contract",
    });

    const auto report = clc::sim::restore_simulation_runtime_from_world_state(state, runtime);
    require(!report.ok(), "restore should reject non-contiguous ledger sequence");
    require_original_runtime_preserved(runtime);

    state.ledger_entries.clear();

    auto invalid_storage_state = state;
    require(invalid_storage_state.engine.settlements[0].storage.add("grain", 1).ok(), "invalid storage setup should add valid grain first");
    require(invalid_storage_state.engine.settlements[0].storage.add("unknown_resource", 1).ok(), "invalid storage setup should allow raw unknown resource in captured state");
    const auto invalid_storage_report = clc::sim::restore_simulation_runtime_from_world_state(invalid_storage_state, runtime);
    require(!invalid_storage_report.ok(), "restore should reject storage with unknown resource");
    require_original_runtime_preserved(runtime);

    auto invalid_market_state = state;
    invalid_market_state.engine.market_demands.push_back(clc::sim::SimulationMarketDemand{.resource_id = "unknown_resource", .demand = 5});
    const auto invalid_market_report = clc::sim::restore_simulation_runtime_from_world_state(invalid_market_state, runtime);
    require(!invalid_market_report.ok(), "restore should reject market demand with unknown resource");
    require_original_runtime_preserved(runtime);

    const auto valid_report = clc::sim::restore_simulation_runtime_from_world_state(state, runtime);
    require(valid_report.ok(), "valid restore should succeed");
    require(!runtime.engine.has_settlement("riverwatch"), "valid restore should replace old settlement");
    require(runtime.engine.has_settlement("hillford"), "valid restore should apply new settlement");
    require(runtime.wallet.coins == 999, "valid restore should apply wallet");
    require(runtime.time.current_tick() == 9999, "valid restore should apply runtime time");

    return 0;
}
