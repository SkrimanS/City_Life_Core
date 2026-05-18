#include "clc/data/DataRegistry.hpp"
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

    const clc::sim::SimulationScenarioPreset invalid_preset;
    const auto invalid_report = clc::sim::validate_scenario_preset(invalid_preset);
    require(!invalid_report.ok(), "empty preset should fail validation");
    require(invalid_report.messages.size() == 3, "empty preset should report id, display name, and day count errors");

    const clc::sim::SimulationScenarioPreset valid_preset{
        .id = "quick_check",
        .display_name = "Quick Check",
        .day_count = 2,
    };
    const auto valid_report = clc::sim::validate_scenario_preset(valid_preset);
    require(valid_report.ok(), "valid preset should pass validation");

    clc::sim::SimulationEngine engine{clc::data::DataRegistry{}};
    const auto preset_result = engine.run_scenario_preset(valid_preset);
    require(preset_result.summary.days_run == 2, "valid preset should run configured day count");
    require(clc::sim::scenario_result_duration_days(preset_result) == 2, "valid preset should produce matching result duration");
    require(engine.current_day() == 2, "valid preset should advance engine day count");
    require(clc::sim::scenario_result_digest(preset_result) == "scenario days=2 start=0 end=2 events=4 warnings=0 status=success", "valid preset digest should remain compatible");

    const auto invalid_preset_result = engine.run_scenario_preset(invalid_preset);
    require(invalid_preset_result.summary.days_run == 0, "invalid preset should not run days");
    require(clc::sim::scenario_result_duration_days(invalid_preset_result) == 0, "invalid preset should return unchanged snapshot range");
    require(engine.current_day() == 2, "invalid preset should not advance engine day count");

    return 0;
}
