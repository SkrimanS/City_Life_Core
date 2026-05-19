#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

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
    require(bootstrap.ok(), "bootstrap scenario should be valid");
    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "roundtrip_caravan",
        "Roundtrip Caravan"
    );
    require(created.ok(), "roundtrip caravan should create");
    require(clc::sim::set_runtime_caravan_owner(runtime, "roundtrip_caravan", "riverwatch").ok(), "roundtrip caravan owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "roundtrip_caravan", "grain", 40).ok(), "roundtrip caravan should load cargo");
    require(runtime.engine.settlement_resource_amount("riverwatch", "grain") == 10, "origin storage should be debited before save");

    require(clc::sim::advance_runtime_caravan_day(runtime, "roundtrip_caravan").ok(), "roundtrip caravan should advance before save");
    require(runtime.caravans.caravans[0].days_remaining == 1, "caravan progress should be partially advanced before save");

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_full_roundtrip_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "runtime_roundtrip.clcs";

    require(clc::sim::save_simulation_runtime_to_file(runtime, file_path).ok(), "runtime should save mid-scenario");

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto loaded_result = clc::sim::load_simulation_runtime_from_file(file_path, loaded);
    require(loaded_result.ok(), "runtime should load mid-scenario");

    require(loaded.engine.settlement_resource_amount("riverwatch", "grain") == 10, "loaded runtime should preserve origin storage debit");
    require(loaded.engine.settlement_resource_amount("hillford", "grain") == 0, "loaded runtime should preserve destination before arrival");
    require(loaded.caravans.caravans.size() == 1, "loaded runtime should preserve caravan");
    require(loaded.caravans.caravans[0].id == "roundtrip_caravan", "loaded runtime should preserve caravan id");
    require(loaded.caravans.caravans[0].days_remaining == 1, "loaded runtime should preserve partial travel progress");
    require(loaded.caravans.caravans[0].cargo.amount("grain") == 40, "loaded runtime should preserve cargo");
    require(loaded.wallet.coins == 10, "loaded runtime should preserve wallet before reward");
    require(loaded.ledger.entries().empty(), "loaded runtime should preserve empty ledger before fulfillment");

    const auto early_load_after_departure = clc::sim::load_runtime_caravan_at_origin(loaded, "roundtrip_caravan", "grain", 1);
    require(!early_load_after_departure.ok(), "loaded runtime should still reject loading after departure");

    auto second_day = clc::sim::advance_runtime_caravan_day(loaded, "roundtrip_caravan");
    require(second_day.ok(), "loaded runtime should continue caravan travel");
    require(second_day.report.arrived, "loaded runtime caravan should arrive after continuing");

    auto fulfilled = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        loaded,
        "grain_delivery_runtime",
        "roundtrip_caravan",
        "riverwatch"
    );
    require(fulfilled.ok(), "loaded runtime should fulfill contract after arrival");
    require(loaded.wallet.coins == 85, "loaded runtime should credit reward wallet");
    require(loaded.ledger.entries().size() == 1, "loaded runtime should record reward ledger");
    require(loaded.ledger.entries()[0].sequence == 1, "loaded runtime ledger should start at sequence one");
    require(loaded.ledger.entries()[0].reference_id == "grain_delivery_runtime", "loaded runtime ledger should reference contract");
    require(loaded.contracts.contracts[0].status == clc::sim::ContractStatus::fulfilled, "loaded runtime should update contract status");
    require(loaded.caravans.caravans[0].cargo.amount("grain") == 10, "loaded runtime fulfillment should debit cargo");

    require(clc::sim::unload_runtime_caravan_at_destination(loaded, "roundtrip_caravan", "grain", 10).ok(), "loaded runtime should unload remaining cargo");
    require(loaded.engine.settlement_resource_amount("hillford", "grain") == 10, "loaded runtime should credit destination after unload");
    require(loaded.caravans.caravans[0].cargo.empty(), "loaded runtime cargo should be empty after unload");

    const auto captured = clc::sim::capture_simulation_world_state(loaded);
    require(clc::sim::validate_simulation_world_state(captured).ok(), "loaded continued runtime should remain valid");

    std::filesystem::remove_all(directory);
    return 0;
}
