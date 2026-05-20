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
    require(bootstrap.ok(), "arrival contract roundtrip bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "arrival_contract_roundtrip_caravan",
        "Arrival Contract Roundtrip Caravan"
    );
    require(caravan.ok(), "arrival contract roundtrip caravan should create");

    require(clc::sim::set_runtime_caravan_owner(runtime, "arrival_contract_roundtrip_caravan", "riverwatch").ok(), "arrival contract roundtrip owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "arrival_contract_roundtrip_caravan", "grain", 40).ok(), "arrival contract roundtrip cargo should load");

    auto first_day = clc::sim::advance_runtime_day(runtime);
    require(first_day.ok(), "arrival contract roundtrip first day should advance");
    require(first_day.engine.day == 1, "arrival contract roundtrip should be on day one before save");
    require(runtime.caravans.caravans[0].days_remaining == 1, "arrival contract roundtrip should save partial caravan progress");

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_arrival_contract_roundtrip_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "arrival_contract_roundtrip.clcs";

    require(clc::sim::save_simulation_runtime_to_file(runtime, file_path).ok(), "arrival contract roundtrip runtime should save");

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    auto load_result = clc::sim::load_simulation_runtime_from_file(file_path, loaded);
    require(load_result.ok(), "arrival contract roundtrip runtime should load");

    require(loaded.engine.current_day() == 1, "loaded arrival contract roundtrip should preserve day");
    require(loaded.caravans.caravans.size() == 1, "loaded arrival contract roundtrip should preserve caravan");
    require(loaded.caravans.caravans[0].days_remaining == 1, "loaded arrival contract roundtrip should preserve partial progress");
    require(loaded.caravans.caravans[0].cargo.amount("grain") == 40, "loaded arrival contract roundtrip should preserve cargo");
    require(loaded.wallet.coins == 10, "loaded arrival contract roundtrip should preserve wallet before reward");
    require(loaded.ledger.entries().empty(), "loaded arrival contract roundtrip should preserve empty ledger before reward");

    auto completed = clc::sim::run_runtime_until_first_caravan_arrival_and_fulfill_contract(
        loaded,
        5,
        "riverwatch"
    );
    require(completed.ok(), "loaded arrival contract roundtrip helper should complete");
    require(completed.arrival.arrival_reached, "loaded arrival contract roundtrip helper should detect arrival");
    require(completed.arrival.arrived_caravan_id == "arrival_contract_roundtrip_caravan", "loaded arrival contract roundtrip helper should preserve arrived caravan id");
    require(completed.arrival.arrival_day == 2, "loaded arrival contract roundtrip helper should report absolute arrival day");
    require(completed.fulfillment.contract_id == "grain_delivery_runtime", "loaded arrival contract roundtrip helper should fulfill matching contract");

    require(loaded.wallet.coins == 85, "loaded arrival contract roundtrip helper should reward wallet");
    require(loaded.ledger.entries().size() == 1, "loaded arrival contract roundtrip helper should record ledger");
    require(loaded.ledger.entries()[0].sequence == 1, "loaded arrival contract roundtrip ledger should start sequence one");
    require(loaded.contracts.contracts[0].status == clc::sim::ContractStatus::fulfilled, "loaded arrival contract roundtrip should fulfill contract");
    require(loaded.caravans.caravans[0].cargo.amount("grain") == 10, "loaded arrival contract roundtrip should debit cargo");

    auto captured = clc::sim::capture_simulation_world_state(loaded);
    require(clc::sim::validate_simulation_world_state(captured).ok(), "loaded arrival contract roundtrip final world state should validate");

    std::filesystem::remove_all(directory);
    return 0;
}
