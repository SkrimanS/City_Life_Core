#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <filesystem>
#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        std::cerr << "failed to create basic runtime scenario\n";
        return 1;
    }

    auto& runtime = bootstrap.runtime;

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "roundtrip_caravan",
        "Roundtrip Caravan"
    );

    if (!caravan.ok()) {
        std::cerr << "failed to create caravan\n";
        return 1;
    }

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto path = std::filesystem::temp_directory_path() / "city_life_core_example_runtime.clcs";

    const auto result = clc::sim::validate_simulation_runtime_save_load_roundtrip(
        runtime,
        loaded,
        path
    );

    if (!result.ok()) {
        std::cerr << "save/load roundtrip failed\n";
        std::filesystem::remove(path);
        return 1;
    }

    std::filesystem::remove(path);
    std::cout << "Runtime save/load roundtrip succeeded\n";
    return 0;
}
