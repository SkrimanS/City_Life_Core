#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
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
    require(bootstrap.ok(), "basic runtime should bootstrap");

    const auto report = clc::sim::orchestrate_standard_runtime_core_processors(bootstrap.runtime, {
        .tick_step = clc::ticks_per_minute(),
        .max_event_count = 8,
    });
    require(report.ok(), "standard orchestration should be valid");
    require(report.tick_after == report.tick_before + clc::ticks_per_minute(), "orchestration should advance one configured step");
    require(report.processor_run.enabled_processor_count >= 20, "standard orchestration should cover core processors");
    require(report.events.size() == 8, "bounded stream should honor max_event_count");
    require(report.dropped_event_count > 0, "bounded stream should count dropped processor events");
    require(report.deep_event_count > 0, "deep events should be counted");
    require(report.regional_event_count > 0, "regional events should be counted");
    require(clc::sim::runtime_processor_orchestration_digest(report).find("runtime_processor_orchestration") != std::string::npos, "digest should identify orchestration");
    require(clc::sim::runtime_processor_orchestration_markdown(report).find("Runtime Processor Orchestration") != std::string::npos, "markdown should identify orchestration");

    auto invalid_bootstrap = clc::sim::make_basic_runtime_scenario();
    const auto invalid = clc::sim::orchestrate_standard_runtime_core_processors(invalid_bootstrap.runtime, {
        .tick_step = 0,
        .max_event_count = 8,
    });
    require(!invalid.ok(), "zero tick step should be rejected");

    return 0;
}
