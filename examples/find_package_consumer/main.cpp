#include "clc/core/Version.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;
    const auto report = clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
    if (!report.ok()) {
        return 1;
    }

    std::cout << clc::core_version_string() << " " << runtime.time.current_tick() << "\n";
    return 0;
}
