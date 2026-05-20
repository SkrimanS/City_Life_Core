#include "clc/core/EventLog.hpp"
#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

#include <cstdlib>
#include <filesystem>
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
    require(bootstrap.ok(), "runtime event log roundtrip bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "event_log_roundtrip_caravan",
        "Event Log Roundtrip Caravan"
    );
    require(created.ok(), "runtime event log roundtrip caravan should create");

    require(clc::sim::set_runtime_caravan_owner(runtime, "event_log_roundtrip_caravan", "riverwatch").ok(), "runtime event log roundtrip owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "event_log_roundtrip_caravan", "grain", 40).ok(), "runtime event log roundtrip cargo should load");

    auto first_day = clc::sim::advance_runtime_day(runtime);
    require(first_day.ok(), "runtime event log roundtrip first day should advance");
    require(first_day.engine.day == 1, "runtime event log roundtrip should be day one before save");

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_event_log_roundtrip_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "event_log_roundtrip.clcs";

    require(clc::sim::save_simulation_runtime_to_file(runtime, file_path).ok(), "runtime event log roundtrip should save");

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    auto load_result = clc::sim::load_simulation_runtime_from_file(file_path, loaded);
    require(load_result.ok(), "runtime event log roundtrip should load");

    auto completed = clc::sim::run_runtime_until_first_caravan_arrival_and_fulfill_contract(
        loaded,
        5,
        "riverwatch"
    );
    require(completed.ok(), "loaded runtime event log roundtrip should complete arrival contract helper");
    require(completed.arrival.arrival_reached, "loaded runtime event log roundtrip should reach arrival");
    require(completed.arrival.arrival_day == 2, "loaded runtime event log roundtrip should preserve absolute arrival day");

    clc::EventLog log{};
    auto summary = clc::sim::append_runtime_day_report_events(log, first_day);
    require(summary.events_appended == 2, "runtime event log roundtrip should append pre-save day events");
    require(summary.day_events == 1, "runtime event log roundtrip should append one pre-save day event");
    require(summary.caravan_events == 1, "runtime event log roundtrip should append one pre-save caravan event");

    auto post_load_summary = clc::sim::append_runtime_arrival_contract_events(log, completed);
    require(post_load_summary.events_appended == 3, "runtime event log roundtrip should append post-load events");
    require(post_load_summary.day_events == 1, "runtime event log roundtrip should append one post-load day event");
    require(post_load_summary.caravan_events == 1, "runtime event log roundtrip should append one post-load caravan event");
    require(post_load_summary.contract_events == 1, "runtime event log roundtrip should append one post-load contract event");

    require(log.size() == 5, "runtime event log roundtrip should contain five events total");
    const auto& events = log.events();

    require(events[0].tick == 1, "runtime event log roundtrip first event should use day one");
    require(events[1].type == "runtime.caravan.progress", "runtime event log roundtrip second event should be progress");
    require(events[3].tick == 2, "runtime event log roundtrip arrival event should use day two");
    require(events[3].type == "runtime.caravan.arrived", "runtime event log roundtrip should record arrival after load");
    require(events[4].tick == 2, "runtime event log roundtrip contract event should use arrival day");
    require(events[4].type == "runtime.contract.fulfilled", "runtime event log roundtrip should record contract fulfillment after load");
    require(events[4].payload == "grain_delivery_runtime", "runtime event log roundtrip contract payload should use contract id");

    const auto validation = clc::sim::validate_runtime_event_log(log);
    require(validation.ok(), "runtime event log roundtrip should pass combined event validation");

    const auto checksum = clc::sim::calculate_runtime_event_log_checksum(log);
    const auto checksum_copy = clc::sim::calculate_runtime_event_log_checksum(log);
    const auto checksum_validation = clc::sim::validate_runtime_event_log_checksum_match(checksum, checksum_copy);
    require(checksum_validation.ok(), "runtime event log roundtrip should pass checksum validation");

    std::filesystem::remove_all(directory);
    return 0;
}
