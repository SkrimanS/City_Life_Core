#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void expect_runtime_drift_detected(
    const clc::sim::SimulationRuntime& expected,
    const clc::sim::SimulationRuntime& drifted,
    std::string_view message
) {
    const auto drift_match = clc::sim::validate_simulation_runtimes_match(expected, drifted);
    require(!drift_match.ok(), message);
}

void advance_runtime_for_continuation(clc::sim::SimulationRuntime& runtime) {
    const auto reports = runtime.engine.run_days(2);
    require(reports.size() == 2, "runtime continuation engine advance failed");

    for (auto& caravan : runtime.caravans.caravans) {
        const auto report = clc::sim::advance_caravan_day(caravan);
        (void)report;
    }
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "bootstrap failed");

    auto& runtime = bootstrap.runtime;
    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "persistence_validation_caravan",
        "Persistence Validation Caravan"
    );
    require(created.ok(), "caravan creation failed");

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_persistence_validation_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto path = directory / "runtime_validation.clcs";

    const auto validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(runtime, loaded, path);
    require(validation.ok(), "roundtrip validation failed");

    const auto runtime_match = clc::sim::validate_simulation_runtimes_match(runtime, loaded);
    require(runtime_match.ok(), "runtime match validation failed");

    auto continued_runtime = runtime;
    auto continued_loaded = loaded;
    advance_runtime_for_continuation(continued_runtime);
    advance_runtime_for_continuation(continued_loaded);
    require(
        clc::sim::validate_simulation_runtimes_match(continued_runtime, continued_loaded).ok(),
        "post-load continuation runtime match failed"
    );

    auto engine_event_drifted = loaded;
    require(
        engine_event_drifted.engine.add_resource_to_settlement_command("riverwatch", "grain", 1).ok,
        "engine event drift setup failed"
    );
    expect_runtime_drift_detected(runtime, engine_event_drifted, "runtime match unexpectedly accepted engine event drift");

    auto wallet_drifted = loaded;
    ++wallet_drifted.wallet.coins;
    expect_runtime_drift_detected(runtime, wallet_drifted, "runtime match unexpectedly accepted wallet drift");

    auto route_id_drifted = loaded;
    route_id_drifted.routes.routes[0].id = "drifted_route";
    expect_runtime_drift_detected(runtime, route_id_drifted, "runtime match unexpectedly accepted route id drift");

    auto caravan_id_drifted = loaded;
    caravan_id_drifted.caravans.caravans[0].id = "drifted_caravan";
    expect_runtime_drift_detected(runtime, caravan_id_drifted, "runtime match unexpectedly accepted caravan id drift");

    clc::sim::SimulationRuntime failed_target{clc::sim::make_basic_runtime_scenario_registry()};
    const auto invalid_path = directory / "missing" / "runtime_validation.clcs";
    const auto failed_validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(runtime, failed_target, invalid_path);
    require(!failed_validation.ok(), "invalid save path unexpectedly succeeded");
    require(!failed_validation.validation.ok(), "invalid save path did not return validation error");

    const auto bad_path = directory / "bad_runtime.clcs";
    {
        std::ofstream bad_file{bad_path};
        bad_file << "not a city life runtime save\n";
    }

    clc::sim::SimulationRuntime bad_target{clc::sim::make_basic_runtime_scenario_registry()};
    const auto bad_load = clc::sim::load_simulation_runtime_from_file(bad_path, bad_target);
    require(!bad_load.ok(), "bad runtime file unexpectedly loaded");

    std::filesystem::remove_all(directory);
    return 0;
}
