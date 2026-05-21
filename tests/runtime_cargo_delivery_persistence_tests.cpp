#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view caravan_id = "cargo_delivery_persistence_caravan";
constexpr std::string_view route_id = "riverwatch_to_hillford";
constexpr std::string_view faction_id = "riverwatch";
constexpr std::string_view resource_id = "grain";
constexpr std::string_view destination_settlement_id = "hillford";

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void prepare_delivered_runtime(clc::sim::SimulationRuntime& runtime) {
    const auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        route_id,
        std::string{caravan_id},
        "Cargo Delivery Persistence Caravan"
    );
    require(created.ok(), "cargo delivery persistence caravan creation failed");

    require(
        clc::sim::set_runtime_caravan_owner(runtime, std::string{caravan_id}, std::string{faction_id}).ok(),
        "cargo delivery persistence caravan ownership setup failed"
    );

    require(
        clc::sim::load_runtime_caravan_at_origin(runtime, caravan_id, resource_id, 40).ok(),
        "cargo delivery persistence caravan load failed"
    );

    require(
        clc::sim::advance_runtime_caravan_day(runtime, caravan_id).ok(),
        "cargo delivery persistence caravan first advance failed"
    );
    require(
        clc::sim::advance_runtime_caravan_day(runtime, caravan_id).ok(),
        "cargo delivery persistence caravan second advance failed"
    );

    const auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, caravan_id);
    require(delivery.ok(), "cargo delivery persistence delivery failed");
    require(delivery.total_amount == 40, "cargo delivery persistence delivered unexpected amount");
    require(runtime.engine.settlement_resource_amount(destination_settlement_id, resource_id) == 40, "cargo delivery persistence destination storage missing delivered cargo");
    require(runtime.caravans.caravans[0].cargo.empty(), "cargo delivery persistence caravan cargo should be empty after delivery");
}

void expect_runtime_drift_detected(
    const clc::sim::SimulationRuntime& expected,
    const clc::sim::SimulationRuntime& drifted,
    std::string_view message
) {
    const auto match = clc::sim::validate_simulation_runtimes_match(expected, drifted);
    require(!match.ok(), message);
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "cargo delivery persistence bootstrap failed");

    auto& runtime = bootstrap.runtime;
    prepare_delivered_runtime(runtime);

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_cargo_delivery_persistence_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto path = directory / "runtime_cargo_delivery.clcs";

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(runtime, loaded, path);
    require(validation.ok(), "cargo delivery persistence roundtrip validation failed");

    const auto match = clc::sim::validate_simulation_runtimes_match(runtime, loaded);
    require(match.ok(), "cargo delivery persistence runtime match failed");
    require(loaded.engine.settlement_resource_amount(destination_settlement_id, resource_id) == 40, "cargo delivery persistence loaded destination storage mismatch");
    require(!loaded.caravans.caravans.empty(), "cargo delivery persistence loaded caravan missing");
    require(loaded.caravans.caravans[0].cargo.empty(), "cargo delivery persistence loaded caravan cargo should remain empty");

    auto storage_drifted = loaded;
    require(storage_drifted.engine.add_resource_to_settlement(std::string{destination_settlement_id}, std::string{resource_id}, 1).ok(), "cargo delivery persistence storage drift setup failed");
    expect_runtime_drift_detected(runtime, storage_drifted, "runtime match unexpectedly accepted delivered destination storage drift");

    auto cargo_drifted = loaded;
    require(cargo_drifted.caravans.caravans[0].cargo.add(std::string{resource_id}, 1).ok(), "cargo delivery persistence cargo drift setup failed");
    expect_runtime_drift_detected(runtime, cargo_drifted, "runtime match unexpectedly accepted redelivered caravan cargo drift");

    std::filesystem::remove_all(directory);
    return 0;
}
