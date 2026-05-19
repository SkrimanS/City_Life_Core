#include "clc/sim/SimulationPersistence.hpp"

#include <cstdlib>
#include <filesystem>
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

clc::sim::SettlementRoute make_route() {
    return clc::sim::SettlementRoute{
        .id = "riverwatch_to_hillford",
        .display_name = "Riverwatch to Hillford",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "hillford",
        .travel_days = 2,
    };
}

} // namespace

int main() {
    clc::sim::SimulationRuntime runtime{make_registry()};
    require(runtime.engine.create_settlement("riverwatch").ok(), "runtime engine should create riverwatch");
    require(runtime.engine.create_settlement("hillford").ok(), "runtime engine should create hillford");
    require(runtime.engine.add_resource_to_settlement("riverwatch", "grain", 40).ok(), "runtime engine should add grain");
    require(clc::sim::add_settlement_route(runtime.routes, make_route()).ok(), "runtime route should add");

    auto caravan = clc::sim::create_caravan_for_route(runtime.routes.routes[0], "runtime_caravan", "Runtime Caravan");
    require(caravan.cargo.add("grain", 15).ok(), "runtime caravan should accept cargo");
    require(clc::sim::add_caravan(runtime.caravans, caravan).ok(), "runtime caravan should add");

    require(clc::sim::add_faction(runtime.factions, clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "runtime faction should add");
    require(clc::sim::add_faction(runtime.factions, clc::sim::FactionState{.id = "traders_guild", .display_name = "Traders Guild"}).ok(), "runtime faction should add traders");
    require(clc::sim::set_caravan_owner(runtime.ownership, "runtime_caravan", "riverwatch").ok(), "runtime caravan owner should set");
    require(clc::sim::add_contract(runtime.contracts, clc::sim::ResourceDeliveryContract{
        .id = "runtime_contract",
        .display_name = "Runtime Contract",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 10,
        .reward_coins = 25,
        .due_day = 5,
    }).ok(), "runtime contract should add");
    runtime.wallet.coins = 7;
    require(runtime.ledger.record_contract_reward("bootstrap", "grain", 1, 2, "bootstrap"), "runtime ledger should record bootstrap");

    const auto captured = clc::sim::capture_simulation_world_state(runtime);
    require(clc::sim::validate_simulation_world_state(captured).ok(), "captured runtime bundle should validate");
    require(captured.engine.settlements.size() == 2, "captured runtime should include settlements");
    require(captured.caravans.caravans.size() == 1, "captured runtime should include caravans");
    require(captured.wallet.coins == 7, "captured runtime should include wallet");
    require(captured.ledger_entries.size() == 1, "captured runtime should include ledger entries");

    const auto directory = std::filesystem::temp_directory_path() / "clc_simulation_runtime_bundle_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "runtime_bundle.clcs";

    require(clc::sim::save_simulation_runtime_to_file(runtime, file_path).ok(), "runtime bundle should save with short helper");

    clc::sim::SimulationRuntime loaded_runtime{make_registry()};
    const auto load_result = clc::sim::load_simulation_runtime_from_file(file_path, loaded_runtime);
    require(load_result.ok(), "runtime bundle should load with short helper");
    require(loaded_runtime.engine.settlement_resource_amount("riverwatch", "grain") == 40, "loaded runtime should preserve settlement storage");
    require(loaded_runtime.routes.routes.size() == 1, "loaded runtime should preserve routes");
    require(loaded_runtime.caravans.caravans.size() == 1, "loaded runtime should preserve caravans");
    require(loaded_runtime.factions.factions.size() == 2, "loaded runtime should preserve factions");
    require(loaded_runtime.ownership.caravans.size() == 1, "loaded runtime should preserve ownership");
    require(loaded_runtime.contracts.contracts.size() == 1, "loaded runtime should preserve contracts");
    require(loaded_runtime.wallet.coins == 7, "loaded runtime should preserve wallet");
    require(loaded_runtime.ledger.next_sequence() == 2, "loaded runtime should preserve ledger sequence");
    require(loaded_runtime.ledger.record_contract_reward("after_load", "grain", 1, 3, "after load"), "loaded runtime ledger should continue");
    require(loaded_runtime.ledger.entries()[1].sequence == 2, "loaded runtime ledger should continue sequence");

    auto restored_state = captured;
    restored_state.wallet.coins = 99;
    require(clc::sim::restore_simulation_runtime_from_world_state(restored_state, loaded_runtime).ok(), "runtime bundle should restore from state");
    require(loaded_runtime.wallet.coins == 99, "bundle restore should update wallet");
    require(loaded_runtime.ledger.next_sequence() == 2, "bundle restore should reset ledger to restored state");

    std::filesystem::remove_all(directory);
    return 0;
}
