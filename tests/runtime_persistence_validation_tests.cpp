#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();

    if (!bootstrap.ok()) {
        std::cerr << "bootstrap failed\n";
        return 1;
    }

    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "persistence_validation_caravan",
        "Persistence Validation Caravan"
    );

    if (!created.ok()) {
        std::cerr << "caravan creation failed\n";
        return 1;
    }

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_persistence_validation_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    const auto path = directory / "runtime_validation.clcs";

    const auto validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(
        runtime,
        loaded,
        path
    );

    if (!validation.ok()) {
        std::cerr << "roundtrip validation failed\n";
        return 1;
    }

    if (loaded.caravans.caravan_count() != runtime.caravans.caravan_count()) {
        std::cerr << "caravan count mismatch after restore\n";
        return 1;
    }

    clc::sim::SimulationRuntime failed_target{clc::sim::make_basic_runtime_scenario_registry()};
    const auto invalid_path = directory / "missing" / "runtime_validation.clcs";

    const auto failed_validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(
        runtime,
        failed_target,
        invalid_path
    );

    if (failed_validation.ok()) {
        std::cerr << "invalid save path unexpectedly succeeded\n";
        return 1;
    }

    if (failed_validation.validation.ok()) {
        std::cerr << "invalid save path did not return validation error\n";
        return 1;
    }

    const auto malformed_path = directory / "malformed_runtime.clcs";
    {
        std::ofstream malformed_file{malformed_path};
        malformed_file << "not a city life runtime save\n";
    }

    clc::sim::SimulationRuntime malformed_target{clc::sim::make_basic_runtime_scenario_registry()};
    const auto malformed_load = clc::sim::load_simulation_runtime_from_file(malformed_path, malformed_target);

    if (malformed_load.ok()) {
        std::cerr << "malformed runtime file unexpectedly loaded\n";
        return 1;
    }

    std::filesystem::remove_all(directory);
    return 0;
}
