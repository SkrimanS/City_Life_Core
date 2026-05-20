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
    require(bootstrap.ok(), "runtime event log bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "event_log_caravan",
        "Event Log Caravan"
    );
    require(created.ok(), "runtime event log caravan should create");

    require(clc::sim::set_runtime_caravan_owner(runtime, "event_log_caravan", "riverwatch").ok(), "runtime event log owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "event_log_caravan", "grain", 40).ok(), "runtime event log cargo should load");

    auto orchestration = clc::sim::run_runtime_until_first_caravan_arrival_and_fulfill_contract(
        runtime,
        5,
        "riverwatch"
    );
    require(orchestration.ok(), "runtime event log orchestration should succeed");

    clc::EventLog log{};
    auto summary = clc::sim::append_runtime_arrival_contract_events(log, orchestration);

    require(summary.events_appended == 5, "runtime event log should append expected number of events");
    require(summary.day_events == 2, "runtime event log should append two day events");
    require(summary.caravan_events == 2, "runtime event log should append two caravan events");
    require(summary.contract_events == 1, "runtime event log should append one contract event");

    require(log.size() == 5, "runtime event log should contain five events");

    const auto& events = log.events();

    require(events[0].tick == 1, "runtime event log first event should use day one tick");
    require(events[0].type == "runtime.day.completed", "runtime event log first event should be day event");

    require(events[1].type == "runtime.caravan.progress", "runtime event log second event should be progress event");
    require(events[1].payload == "event_log_caravan", "runtime event log progress payload should use caravan id");

    require(events[3].type == "runtime.caravan.arrived", "runtime event log should record arrival event");
    require(events[3].tick == 2, "runtime event log arrival should use arrival day tick");

    require(events[4].type == "runtime.contract.fulfilled", "runtime event log should record contract event");
    require(events[4].payload == "grain_delivery_runtime", "runtime event log contract payload should use contract id");

    return 0;
}
