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
    require(bootstrap.ok(), "absolute tick event log bootstrap should succeed");
    auto& runtime = bootstrap.runtime;

    runtime.time.advance(clc::hours_to_ticks(6));
    const auto start_tick = runtime.time.current_tick();
    require(start_tick == clc::hours_to_ticks(6), "absolute tick runtime should start from non-zero clock");

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "absolute_tick_caravan",
        "Absolute Tick Caravan"
    );
    require(created.ok(), "absolute tick caravan should create");
    require(clc::sim::set_runtime_caravan_owner(runtime, "absolute_tick_caravan", "riverwatch").ok(), "absolute tick owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "absolute_tick_caravan", "grain", 40).ok(), "absolute tick caravan should load");

    auto orchestration = clc::sim::run_runtime_until_first_caravan_arrival_and_fulfill_contract(
        runtime,
        5,
        "riverwatch"
    );
    require(orchestration.ok(), "absolute tick orchestration should succeed");
    require(orchestration.arrival.arrival_elapsed_ticks == clc::days_to_ticks(2), "absolute tick elapsed arrival should remain duration-only");
    require(runtime.time.current_tick() == start_tick + clc::days_to_ticks(2), "absolute tick runtime should end at start plus travel duration");

    clc::EventLog log{};
    const auto summary = clc::sim::append_runtime_arrival_contract_events(log, orchestration);
    require(summary.events_appended == 5, "absolute tick event log should append expected events");
    require(log.size() == 5, "absolute tick event log should contain five events");

    const auto& events = log.events();
    require(events[0].type == "runtime.day.completed", "absolute tick first event should be day event");
    require(events[0].tick == start_tick + clc::ticks_per_day(), "absolute tick first day event should include runtime offset");
    require(events[3].type == "runtime.caravan.arrived", "absolute tick arrival event should exist");
    require(events[3].tick == start_tick + clc::days_to_ticks(2), "absolute tick arrival event should use absolute runtime tick");
    require(events[4].type == "runtime.contract.fulfilled", "absolute tick fulfilled event should exist");
    require(events[4].tick == start_tick + clc::days_to_ticks(2), "absolute tick fulfilled event should use absolute runtime tick, not elapsed duration");
    require(events[4].tick != orchestration.arrival.arrival_elapsed_ticks, "absolute tick fulfilled event should not collapse to elapsed duration when runtime clock is offset");
    require(clc::sim::validate_runtime_event_log(log).ok(), "absolute tick event log should validate");

    return 0;
}
