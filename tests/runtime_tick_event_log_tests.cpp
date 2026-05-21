#include "clc/core/EventLog.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

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
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "tick event log bootstrap should succeed");
    auto& runtime = bootstrap.runtime;

    require(clc::sim::add_runtime_route(runtime, clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford_tick_events",
        "Riverwatch to Hillford Tick Events",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(2)
    )).ok(), "tick event log route should add");

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford_tick_events",
        "tick_event_caravan",
        "Tick Event Caravan"
    );
    require(created.ok(), "tick event log caravan should create");
    require(clc::sim::set_runtime_caravan_owner(runtime, "tick_event_caravan", "riverwatch").ok(), "tick event log owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "tick_event_caravan", "grain", 40).ok(), "tick event log caravan should load");

    const auto result = clc::sim::run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(
        runtime,
        clc::hours_to_ticks(3),
        clc::hours_to_ticks(1),
        "riverwatch"
    );
    require(result.ok(), "tick event log arrival contract helper should succeed");
    require(result.arrival.arrival_tick == clc::hours_to_ticks(2), "tick event log arrival should happen at two hours");
    require(result.fulfillment.contract_id == "grain_delivery_runtime", "tick event log fulfillment should use runtime contract");

    clc::EventLog log{};
    const auto summary = clc::sim::append_runtime_tick_arrival_contract_events(log, result);
    require(summary.events_appended == 5, "tick event log should append two ticks, two caravan events, one contract event");
    require(summary.tick_events == 2, "tick event log should count tick events");
    require(summary.day_events == 0, "tick event log should not count day events");
    require(summary.caravan_events == 2, "tick event log should count caravan events");
    require(summary.contract_events == 1, "tick event log should count fulfilled contract event");

    require(log.size() == 5, "tick event log should contain five events");
    const auto& events = log.events();
    require(events[0].tick == clc::hours_to_ticks(1), "tick event log first tick event should use one hour tick");
    require(events[0].type == "runtime.tick.completed", "tick event log first event should be tick completed");
    require(events[0].payload == "elapsed=3600", "tick event log first tick payload should expose elapsed ticks");
    require(events[1].type == "runtime.caravan.progress", "tick event log second event should be caravan progress");
    require(events[2].tick == clc::hours_to_ticks(2), "tick event log third event should use arrival tick");
    require(events[2].type == "runtime.tick.completed", "tick event log third event should be second tick completed");
    require(events[3].type == "runtime.caravan.arrived", "tick event log should record caravan arrival");
    require(events[4].type == "runtime.contract.fulfilled", "tick event log should record contract fulfillment");
    require(events[4].tick == clc::hours_to_ticks(2), "tick event log fulfilled contract should use absolute arrival tick");

    const auto analysis = clc::sim::analyze_runtime_event_log(log);
    require(analysis.total_events == 5, "tick event log analysis should count total events");
    require(analysis.tick_events == 2, "tick event log analysis should count tick events");
    require(analysis.day_events == 0, "tick event log analysis should not count day events");
    require(analysis.caravan_progress_events == 1, "tick event log analysis should count progress events");
    require(analysis.caravan_arrival_events == 1, "tick event log analysis should count arrival events");
    require(analysis.contract_fulfilled_events == 1, "tick event log analysis should count fulfilled event");
    require(analysis.first_tick == clc::hours_to_ticks(1), "tick event log analysis should expose first tick");
    require(analysis.last_tick == clc::hours_to_ticks(2), "tick event log analysis should expose last tick");
    require(clc::sim::validate_runtime_event_log(log).ok(), "tick event log should validate");

    clc::EventLog bad_tick_payload{};
    bad_tick_payload.append(clc::hours_to_ticks(1), "runtime.tick.completed", "elapsed=0");
    require(!clc::sim::validate_runtime_event_log_payloads(bad_tick_payload).ok(), "tick event log payload validator should reject zero elapsed ticks");

    return 0;
}
