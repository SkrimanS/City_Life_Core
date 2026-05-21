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

    const auto runtime_match = clc::sim::validate_simulation_runtimes_match(runtime, loaded);

    if (!runtime_match.ok()) {
        std::cerr << "runtime match validation failed\n";
        return 1;
    }

    auto drifted = loaded;
    ++drifted.wallet.coins;

    const auto drift_match = clc::sim::validate_simulation_runtimes_match(runtime, drifted);

    if (drift_match.ok()) {
        std::cerr << "runtime match unexpectedly accepted wallet drift\n";
        return 1;
    }

    auto route_drifted = loaded;
    route_drifted.routes.routes[0].id = "drifted_route";

    const auto route_drift_match = clc::sim::validate_simulation_runtimes_match(runtime, route_drifted);

    if (route_drift_match.ok()) {
        std::cerr << "runtime match unexpectedly accepted route id drift\n";
        return 1;
    }

    auto route_duration_drifted = loaded;
    ++route_duration_drifted.routes.routes[0].travel_days;

    const auto route_duration_drift_match = clc::sim::validate_simulation_runtimes_match(runtime, route_duration_drifted);

    if (route_duration_drift_match.ok()) {
        std::cerr << "runtime match unexpectedly accepted route travel duration drift\n";
        return 1;
    }

    auto caravan_progress_drifted = loaded;
    --caravan_progress_drifted.caravans.caravans[0].days_remaining;

    const auto caravan_progress_drift_match = clc::sim::validate_simulation_runtimes_match(runtime, caravan_progress_drifted);

    if (caravan_progress_drift_match.ok()) {
        std::cerr << "runtime match unexpectedly accepted caravan progress drift\n";
        return 1;
    }

    auto faction_reputation_drifted = loaded;
    ++faction_reputation_drifted.factions.reputations[0].value;

    const auto faction_reputation_drift_match = clc::sim::validate_simulation_runtimes_match(runtime, faction_reputation_drifted);

    if (faction_reputation_drift_match.ok()) {
        std::cerr << "runtime match unexpectedly accepted faction reputation drift\n";
        return 1;
    }

    auto contract_drifted = loaded;
    contract_drifted.contracts.contracts[0].id = "drifted_contract";

    const auto contract_drift_match = clc::sim::validate_simulation_runtimes_match(runtime, contract_drifted);

    if (contract_drift_match.ok()) {
        std::cerr << "runtime match unexpectedly accepted contract id drift\n";
        return 1;
    }

    auto contract_reward_drifted = loaded;
    ++contract_reward_drifted.contracts.contracts[0].reward_coins;

    const auto contract_reward_drift_match = clc::sim::validate_simulation_runtimes_match(runtime, contract_reward_drifted);

    if (contract_reward_drift_match.ok()) {
        std::cerr << "runtime match unexpectedly accepted contract reward drift\n";
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
