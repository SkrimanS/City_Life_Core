#include "clc/sim/SimulationEngine.hpp"

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
    clc::sim::SimulationScenarioResult empty_result;
    require(clc::sim::scenario_result_digest(empty_result) == "scenario days=0 start=0 end=0 events=0 warnings=0 status=success", "empty scenario digest should be stable");

    clc::sim::SimulationScenarioResult success_result;
    success_result.initial_snapshot.day = 4;
    success_result.final_snapshot.day = 6;
    success_result.events_delta.push_back(clc::sim::SimulationEvent{.day = 5, .type = "simulation.day.started", .message = "simulation day started"});
    success_result.events_delta.push_back(clc::sim::SimulationEvent{.day = 5, .type = "simulation.day.completed", .message = "simulation day completed"});
    require(clc::sim::scenario_result_digest(success_result) == "scenario days=2 start=4 end=6 events=2 warnings=0 status=success", "warning-free scenario digest should report success");

    clc::sim::SimulationScenarioResult warning_result;
    warning_result.initial_snapshot.day = 6;
    warning_result.final_snapshot.day = 9;
    warning_result.events_delta.push_back(clc::sim::SimulationEvent{.day = 7, .type = "simulation.day.started", .message = "simulation day started"});
    warning_result.events_delta.push_back(clc::sim::SimulationEvent{.day = 7, .type = "simulation.settlement.advanced", .message = "advanced settlement: riverwatch"});
    warning_result.events_delta.push_back(clc::sim::SimulationEvent{.day = 7, .type = "simulation.day.completed", .message = "simulation day completed"});
    warning_result.warnings_delta.push_back("riverwatch: not enough food");
    warning_result.warnings_delta.push_back("riverwatch: missing input resource");
    require(clc::sim::scenario_result_digest(warning_result) == "scenario days=3 start=6 end=9 events=3 warnings=2 status=warning", "warning scenario digest should report warning status");

    clc::sim::SimulationScenarioResult defensive_result;
    defensive_result.initial_snapshot.day = 9;
    defensive_result.final_snapshot.day = 7;
    require(clc::sim::scenario_result_digest(defensive_result) == "scenario days=0 start=9 end=7 events=0 warnings=0 status=success", "defensive digest should clamp negative duration to zero");

    return 0;
}
