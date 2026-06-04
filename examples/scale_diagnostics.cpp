#include "clc/CityLifeCore.hpp"

#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;
    clc::EventLog events;
    const auto run = clc::sim::run_runtime_ticks(runtime, clc::days_to_ticks(2), clc::days_to_ticks(1));
    const auto summary = clc::sim::append_runtime_tick_run_events(events, run);
    if (summary.events_appended == 0) {
        return 1;
    }

    const auto snapshot = clc::sim::make_runtime_scale_snapshot(runtime, &events);
    std::cout << clc::sim::runtime_scale_snapshot_digest(snapshot) << '\n';

    return clc::sim::validate_runtime_scale_snapshot(snapshot).ok() ? 0 : 1;
}
