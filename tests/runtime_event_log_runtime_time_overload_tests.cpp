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
    require(bootstrap.ok(), "runtime-time overload bootstrap should succeed");
    auto& runtime = bootstrap.runtime;
    runtime.time.advance(clc::hours_to_ticks(9));

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "runtime_time_overload_caravan",
        "Runtime Time Overload Caravan"
    );
    require(created.ok(), "runtime-time overload caravan should create");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "runtime_time_overload_caravan", "grain", 10).ok(), "runtime-time overload caravan should load");
    require(clc::sim::advance_runtime_caravan_day(runtime, "runtime_time_overload_caravan").ok(), "runtime-time overload caravan first day should advance");
    require(clc::sim::advance_runtime_caravan_day(runtime, "runtime_time_overload_caravan").ok(), "runtime-time overload caravan second day should advance");

    const auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "runtime_time_overload_caravan");
    require(delivery.ok(), "runtime-time overload delivery should succeed");
    require(delivery.total_amount == 10, "runtime-time overload delivery should move cargo");

    clc::EventLog log{};
    const auto single_summary = clc::sim::append_runtime_caravan_cargo_delivery_event(log, runtime, delivery);
    require(single_summary.events_appended == 1, "runtime-time overload single cargo append should append one event");
    require(log.size() == 1, "runtime-time overload single cargo log should contain one event");
    require(log.events()[0].tick == clc::hours_to_ticks(9), "runtime-time overload single cargo event should use runtime clock");
    require(log.events()[0].payload == "runtime_time_overload_caravan->hillford:total=10", "runtime-time overload single cargo payload should be deterministic");

    clc::EventLog bulk_log{};
    clc::sim::RuntimeBulkCargoDeliveryResult bulk{};
    bulk.delivered_caravans = 1;
    bulk.total_amount = delivery.total_amount;
    bulk.deliveries.push_back(delivery);
    const auto bulk_summary = clc::sim::append_runtime_bulk_caravan_cargo_delivery_events(bulk_log, runtime, bulk);
    require(bulk_summary.events_appended == 1, "runtime-time overload bulk cargo append should append one event");
    require(bulk_log.size() == 1, "runtime-time overload bulk cargo log should contain one event");
    require(bulk_log.events()[0].tick == clc::hours_to_ticks(9), "runtime-time overload bulk cargo event should use runtime clock");
    require(clc::sim::validate_runtime_event_log(log).ok(), "runtime-time overload single log should validate");
    require(clc::sim::validate_runtime_event_log(bulk_log).ok(), "runtime-time overload bulk log should validate");

    return 0;
}
