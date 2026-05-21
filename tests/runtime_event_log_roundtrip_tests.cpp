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
    require(summary.cargo_events == 0, "runtime event log roundtrip should not append pre-save cargo events");

    auto post_load_summary = clc::sim::append_runtime_arrival_contract_events(log, completed);
    require(post_load_summary.events_appended == 3, "runtime event log roundtrip should append post-load events");
    require(post_load_summary.day_events == 1, "runtime event log roundtrip should append one post-load day event");
    require(post_load_summary.caravan_events == 1, "runtime event log roundtrip should append one post-load caravan event");
    require(post_load_summary.cargo_events == 0, "runtime event log roundtrip should not append cargo event before explicit delivery");
    require(post_load_summary.contract_events == 1, "runtime event log roundtrip should append one post-load contract event");

    auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(loaded, "event_log_roundtrip_caravan");
    require(delivery.ok(), "runtime event log roundtrip cargo delivery should succeed after load");
    require(delivery.total_amount == 10, "runtime event log roundtrip should deliver remaining cargo after contract fulfillment");
    require(loaded.engine.settlement_resource_amount("hillford", "grain") == 10, "runtime event log roundtrip should credit destination storage after delivery");

    auto cargo_summary = clc::sim::append_runtime_caravan_cargo_delivery_event(log, completed.arrival.arrival_day, delivery);
    require(cargo_summary.events_appended == 1, "runtime event log roundtrip should append cargo delivery event");
    require(cargo_summary.day_events == 0, "runtime event log roundtrip cargo summary should not count day events");
    require(cargo_summary.caravan_events == 0, "runtime event log roundtrip cargo summary should not count caravan progress events");
    require(cargo_summary.cargo_events == 1, "runtime event log roundtrip cargo summary should count one cargo event");
    require(cargo_summary.contract_events == 0, "runtime event log roundtrip cargo summary should not count contract events");

    require(log.size() == 6, "runtime event log roundtrip should contain six events total");
    const auto& events = log.events();

    require(events[0].tick == 1, "runtime event log roundtrip first event should use day one");
    require(events[1].type == "runtime.caravan.progress", "runtime event log roundtrip second event should be progress");
    require(events[3].tick == 2, "runtime event log roundtrip arrival event should use day two");
    require(events[3].type == "runtime.caravan.arrived", "runtime event log roundtrip should record arrival after load");
    require(events[4].tick == 2, "runtime event log roundtrip contract event should use arrival day");
    require(events[4].type == "runtime.contract.fulfilled", "runtime event log roundtrip should record contract fulfillment after load");
    require(events[4].payload == "grain_delivery_runtime", "runtime event log roundtrip contract payload should use contract id");
    require(events[5].tick == 2, "runtime event log roundtrip cargo event should use arrival day");
    require(events[5].type == "runtime.caravan.cargo_delivered", "runtime event log roundtrip should record cargo delivery after load");
    require(events[5].payload == "event_log_roundtrip_caravan->hillford:total=10", "runtime event log roundtrip cargo payload should be deterministic");

    const auto analysis = clc::sim::analyze_runtime_event_log(log);
    require(analysis.total_events == 6, "runtime event log roundtrip analysis should count all events");
    require(analysis.caravan_cargo_delivered_events == 1, "runtime event log roundtrip analysis should count cargo delivery event");
    require(analysis.contract_fulfilled_events == 1, "runtime event log roundtrip analysis should count contract fulfillment event");
    require(analysis.unknown_events == 0, "runtime event log roundtrip analysis should not report unknown events");

    const auto validation = clc::sim::validate_runtime_event_log(log);
    require(validation.ok(), "runtime event log roundtrip should pass combined event validation");

    const clc::EventLog expected = log;
    const auto pair_validation = clc::sim::validate_runtime_event_logs_match(expected, log);
    require(pair_validation.ok(), "runtime event log roundtrip should pass pair event log validation");

    clc::EventLog drifted = log;
    drifted.append(3, "runtime.caravan.cargo_delivered", "event_log_roundtrip_caravan->hillford:total=11");
    const auto drift_validation = clc::sim::validate_runtime_event_logs_match(expected, drifted);
    require(!drift_validation.ok(), "runtime event log roundtrip should reject cargo delivery event drift");

    std::filesystem::remove_all(directory);
    return 0;
}
