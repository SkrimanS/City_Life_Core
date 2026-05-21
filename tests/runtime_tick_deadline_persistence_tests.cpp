#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

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

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "runtime tick deadline persistence bootstrap should succeed");
    auto& runtime = bootstrap.runtime;

    require(clc::sim::add_runtime_resource_delivery_contract(runtime, clc::sim::ResourceDeliveryContract{
        .id = "runtime_tick_deadline_persisted",
        .display_name = "Runtime Tick Deadline Persisted",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 1,
        .reward_coins = 1,
        .due_ticks = clc::hours_to_ticks(3),
    }).ok(), "runtime tick deadline persisted contract should add");

    const auto before_save = clc::sim::advance_runtime_ticks(runtime, clc::hours_to_ticks(2));
    require(before_save.ok(), "runtime should advance before save");
    require(before_save.tick_after == clc::hours_to_ticks(2), "runtime clock should be two hours before save");
    require(before_save.contracts.failed_count == 0, "contract should stay open before persisted deadline");

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_tick_deadline_persistence_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "runtime_tick_deadline.clcs";

    require(clc::sim::save_simulation_runtime_to_file(runtime, file_path).ok(), "runtime with tick deadline should save");

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto load = clc::sim::load_simulation_runtime_from_file(file_path, loaded);
    require(load.ok(), "runtime with tick deadline should load");
    require(load.state.time.current_tick() == clc::hours_to_ticks(2), "loaded world state should restore clock");
    require(loaded.time.current_tick() == clc::hours_to_ticks(2), "loaded runtime should restore clock");

    const auto* loaded_contract = clc::sim::contract_by_id(loaded.contracts, "runtime_tick_deadline_persisted");
    require(loaded_contract != nullptr, "loaded runtime should restore tick deadline contract");
    require(loaded_contract->due_ticks == clc::hours_to_ticks(3), "loaded runtime should restore contract due ticks");
    require(loaded_contract->status == clc::sim::ContractStatus::open, "loaded runtime should keep contract open before deadline");

    const auto serialized = clc::sim::serialize_simulation_world_state(load.state);
    require(serialized.find("time\t") != std::string::npos, "serialized world state should include runtime time row");
    require(serialized.find("runtime_tick_deadline_persisted") != std::string::npos, "serialized world state should include tick deadline contract");
    require(serialized.find(std::to_string(clc::hours_to_ticks(3))) != std::string::npos, "serialized world state should include contract due ticks");

    const auto boundary = clc::sim::advance_runtime_ticks(loaded, clc::hours_to_ticks(1));
    require(boundary.ok(), "loaded runtime should advance to deadline boundary");
    require(boundary.tick_before == clc::hours_to_ticks(2), "loaded runtime should continue from restored clock");
    require(boundary.tick_after == clc::hours_to_ticks(3), "loaded runtime should reach deadline boundary");
    require(boundary.contracts.failed_count == 0, "loaded runtime should not fail contract on deadline boundary");
    require(clc::sim::contract_by_id(loaded.contracts, "runtime_tick_deadline_persisted")->status == clc::sim::ContractStatus::open, "loaded runtime should keep contract open on boundary");

    const auto after_deadline = clc::sim::advance_runtime_ticks(loaded, 1);
    require(after_deadline.ok(), "loaded runtime should advance after deadline");
    require(after_deadline.contracts.failed_count == 1, "loaded runtime should fail persisted tick deadline after boundary");
    require(after_deadline.contracts.failed_contract_ids[0] == "runtime_tick_deadline_persisted", "loaded runtime should report persisted deadline failure");
    require(clc::sim::contract_by_id(loaded.contracts, "runtime_tick_deadline_persisted")->status == clc::sim::ContractStatus::failed, "loaded runtime should mark persisted tick deadline failed");
    require(loaded.engine.current_day() == 0, "persisted tick deadline should not require daily engine advancement");

    std::filesystem::remove_all(directory);
    return 0;
}
