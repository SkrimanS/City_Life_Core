#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
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
    require(bootstrap.ok(), "legacy time restore bootstrap should succeed");

    auto state = clc::sim::capture_simulation_world_state(bootstrap.runtime);
    state.engine.current_day = 2;
    state.time = clc::GameTime{};

    clc::sim::SimulationRuntime restored{clc::sim::make_basic_runtime_scenario_registry()};
    const auto report = clc::sim::restore_simulation_runtime_from_world_state(state, restored);
    require(report.ok(), "legacy world state should restore");
    require(restored.engine.current_day() == 2, "legacy world state should restore current day");
    require(restored.time.current_tick() == clc::days_to_ticks(2), "legacy world state without explicit time should sync runtime clock from day");

    state.time = clc::GameTime{clc::hours_to_ticks(7)};
    clc::sim::SimulationRuntime explicit_time_restored{clc::sim::make_basic_runtime_scenario_registry()};
    const auto explicit_report = clc::sim::restore_simulation_runtime_from_world_state(state, explicit_time_restored);
    require(explicit_report.ok(), "explicit-time world state should restore");
    require(explicit_time_restored.time.current_tick() == clc::hours_to_ticks(7), "explicit-time world state should preserve exact runtime clock");

    return 0;
}
