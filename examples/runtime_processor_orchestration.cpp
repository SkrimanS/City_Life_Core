#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return EXIT_FAILURE;
    }

    const auto report = clc::sim::orchestrate_standard_runtime_core_processors(bootstrap.runtime, {
        .tick_step = clc::ticks_per_minute(),
        .max_event_count = 16,
    });

    std::cout << clc::sim::runtime_processor_orchestration_digest(report) << '\n';
    std::cout << clc::sim::runtime_processor_orchestration_markdown(report) << '\n';
    return report.ok() ? EXIT_SUCCESS : 2;
}
