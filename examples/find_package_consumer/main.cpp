#include "clc/CityLifeCore.hpp"

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

    const auto action_result = clc::sim::dispatch_runtime_action_json(
        runtime.engine,
        R"({"action_id":"consumer-a1","type":"advance_days","payload":{"days":1}})"
    );
    if (!action_result.accepted) {
        return 1;
    }

    const auto action_json = clc::sim::runtime_action_result_to_json(action_result);
    if (action_json.find("\"validation_status\":\"accepted\"") == std::string::npos) {
        return 1;
    }

    std::cout << clc::core_version_string() << " " << runtime.time.current_tick() << "\n";
    return 0;
}
