#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <filesystem>
#include <iostream>
#include <string_view>

namespace {

constexpr std::string_view route_id = "riverwatch_to_hillford";
constexpr std::string_view caravan_id = "example_replay_caravan";
constexpr std::string_view faction_id = "riverwatch";
constexpr std::string_view contract_id = "grain_delivery_runtime";

bool prepare_runtime(clc::sim::SimulationRuntime& runtime) {
    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        route_id,
        std::string{caravan_id},
        "Example Replay Caravan"
    );

    if (!caravan.ok()) {
        return false;
    }

    if (!clc::sim::set_runtime_caravan_owner(runtime, caravan_id, faction_id).ok()) {
        return false;
    }

    if (!clc::sim::load_runtime_caravan_at_origin(runtime, caravan_id, "grain", 30).ok()) {
        return false;
    }

    if (!clc::sim::advance_runtime_caravan_day(runtime, caravan_id).ok()) {
        return false;
    }

    const auto reports = runtime.engine.run_days(1);
    return reports.size() == 1;
}

bool finish_runtime(clc::sim::SimulationRuntime& runtime) {
    if (!clc::sim::advance_runtime_caravan_day(runtime, caravan_id).ok()) {
        return false;
    }

    auto fulfilled = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        contract_id,
        caravan_id,
        faction_id
    );

    return fulfilled.ok();
}

} // namespace

int main() {
    auto source_bootstrap = clc::sim::make_basic_runtime_scenario();
    auto control_bootstrap = clc::sim::make_basic_runtime_scenario();

    if (!source_bootstrap.ok() || !control_bootstrap.ok()) {
        std::cerr << "failed to create runtime scenarios\n";
        return 1;
    }

    auto& source = source_bootstrap.runtime;
    auto& control = control_bootstrap.runtime;

    if (!prepare_runtime(source) || !prepare_runtime(control)) {
        std::cerr << "failed to prepare replay runtimes\n";
        return 1;
    }

    const auto path = std::filesystem::temp_directory_path() / "city_life_core_example_replay.clcs";
    const auto save = clc::sim::save_simulation_runtime_to_file(source, path);

    if (!save.ok()) {
        std::cerr << "failed to save replay midpoint\n";
        return 1;
    }

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto load = clc::sim::load_simulation_runtime_from_file(path, loaded);
    std::filesystem::remove(path);

    if (!load.ok()) {
        std::cerr << "failed to load replay midpoint\n";
        return 1;
    }

    if (!finish_runtime(control) || !finish_runtime(loaded)) {
        std::cerr << "failed to finish replay runtimes\n";
        return 1;
    }

    const auto match = clc::sim::validate_simulation_runtimes_match(control, loaded);
    if (!match.ok()) {
        std::cerr << "replay runtimes diverged\n";
        return 1;
    }

    std::cout << "Replay persistence example succeeded\n";
    return 0;
}
