#include "clc/core/Version.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

#include <iostream>

int main() {
    std::cout << "City Life Core " << clc::core_version_string() << "\n";

    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        std::cerr << "failed to create basic runtime scenario\n";
        return 1;
    }

    auto& runtime = bootstrap.runtime;

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "example_caravan",
        "Example Caravan"
    );

    if (!caravan.ok()) {
        std::cerr << "failed to create caravan\n";
        return 1;
    }

    const auto run = clc::sim::run_runtime_days(runtime, 2);

    if (!run.ok()) {
        std::cerr << "runtime run failed\n";
        return 1;
    }

    std::cout << "Days run: " << run.summary.days_run << "\n";
    std::cout << "Caravan ticks: " << run.summary.caravan_ticks << "\n";
    std::cout << "Caravan arrivals: " << run.summary.caravan_arrivals << "\n";
    std::cout << "Contract failures: " << run.summary.contract_failures << "\n";

    return 0;
}
