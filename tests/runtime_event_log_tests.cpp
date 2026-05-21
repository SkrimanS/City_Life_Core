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
    require(summary.cargo_events == 0, "runtime event log should not append cargo events without explicit delivery");
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

    auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "event_log_caravan");
    require(delivery.ok(), "runtime event log cargo delivery should succeed after fulfillment");
    require(delivery.total_amount == 10, "runtime event log cargo delivery should move remaining cargo");

    auto delivery_summary = clc::sim::append_runtime_caravan_cargo_delivery_event(log, orchestration.arrival.arrival_day, delivery);
    require(delivery_summary.events_appended == 1, "runtime event log should append cargo delivery event");
    require(delivery_summary.cargo_events == 1, "runtime event log should count cargo delivery event");
    require(log.events().back().type == "runtime.caravan.cargo_delivered", "runtime event log should record cargo delivery event");
    require(log.events().back().payload == "event_log_caravan->hillford:total=10", "runtime event log should record cargo delivery payload");
    require(clc::sim::validate_runtime_event_log(log).ok(), "runtime event log with cargo delivery should validate");

    const auto delivery_analysis = clc::sim::analyze_runtime_event_log(log);
    require(delivery_analysis.caravan_cargo_delivered_events == 1, "runtime event analysis should count cargo delivery events");

    clc::sim::RuntimeScenarioBootstrapConfig overdue_config{};
    overdue_config.contract_due_day = 1;
    auto overdue_bootstrap = clc::sim::make_basic_runtime_scenario(overdue_config);
    require(overdue_bootstrap.ok(), "runtime failed-contract event bootstrap should succeed");

    const auto overdue_run = clc::sim::run_runtime_days(overdue_bootstrap.runtime, 2);
    require(overdue_run.ok(), "runtime failed-contract event run should succeed");
    require(overdue_run.summary.contract_failures == 1, "runtime failed-contract event run should fail one contract");

    clc::EventLog failed_log{};
    const auto failed_summary = clc::sim::append_runtime_run_events(failed_log, overdue_run);
    require(failed_summary.contract_events == 1, "runtime event log should append one failed contract event");

    const auto failed_analysis = clc::sim::analyze_runtime_event_log(failed_log);
    require(failed_analysis.contract_failed_events == 1, "runtime event analysis should count failed contracts");
    require(failed_analysis.contract_fulfilled_events == 0, "runtime event analysis should not count fulfilled contracts in failed run");
    require(clc::sim::validate_runtime_event_log(failed_log).ok(), "runtime failed-contract event log should validate");

    bool found_failed_contract_event = false;
    for (const auto& event : failed_log.events()) {
        if (event.type == "runtime.contract.failed" && event.payload == "grain_delivery_runtime") {
            found_failed_contract_event = true;
        }
    }
    require(found_failed_contract_event, "runtime event log should record failed contract event payload");

    return 0;
}
