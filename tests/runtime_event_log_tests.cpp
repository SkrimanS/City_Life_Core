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

    require(events[0].tick == clc::ticks_per_day(), "runtime event log first event should use one day in ticks");
    require(events[0].type == "runtime.day.completed", "runtime event log first event should be day event");
    require(events[0].payload == "day=1", "runtime event log day payload should keep day number");

    require(events[1].tick == clc::ticks_per_day(), "runtime event log progress should use runtime tick");
    require(events[1].type == "runtime.caravan.progress", "runtime event log second event should be progress event");
    require(events[1].payload == "event_log_caravan", "runtime event log progress payload should use caravan id");

    require(events[3].type == "runtime.caravan.arrived", "runtime event log should record arrival event");
    require(events[3].tick == clc::days_to_ticks(2), "runtime event log arrival should use arrival runtime tick");

    require(events[4].type == "runtime.contract.fulfilled", "runtime event log should record contract event");
    require(events[4].tick == clc::days_to_ticks(2), "runtime event log fulfilled contract should use arrival elapsed tick");
    require(events[4].payload == "grain_delivery_runtime", "runtime event log contract payload should use contract id");

    auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "event_log_caravan");
    require(delivery.ok(), "runtime event log cargo delivery should succeed after fulfillment");
    require(delivery.total_amount == 10, "runtime event log cargo delivery should move remaining cargo");

    auto delivery_summary = clc::sim::append_runtime_caravan_cargo_delivery_event(log, orchestration.arrival.arrival_elapsed_ticks, delivery);
    require(delivery_summary.events_appended == 1, "runtime event log should append cargo delivery event");
    require(delivery_summary.cargo_events == 1, "runtime event log should count cargo delivery event");
    require(log.events().back().tick == clc::days_to_ticks(2), "runtime event log cargo delivery should use runtime tick");
    require(log.events().back().type == "runtime.caravan.cargo_delivered", "runtime event log should record cargo delivery event");
    require(log.events().back().payload == "event_log_caravan->hillford:total=10", "runtime event log should record cargo delivery payload");
    require(clc::sim::validate_runtime_event_log(log).ok(), "runtime event log with cargo delivery should validate");

    const auto size_before_zero_delivery_append = log.size();
    const auto repeated_delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "event_log_caravan");
    require(repeated_delivery.ok(), "runtime event log repeated cargo delivery should succeed as empty result");
    require(repeated_delivery.total_amount == 0, "runtime event log repeated cargo delivery should have zero total");
    const auto zero_delivery_summary = clc::sim::append_runtime_caravan_cargo_delivery_event(log, orchestration.arrival.arrival_elapsed_ticks, repeated_delivery);
    require(zero_delivery_summary.events_appended == 0, "runtime event log should not append zero cargo delivery event");
    require(zero_delivery_summary.cargo_events == 0, "runtime event log should not count zero cargo delivery event");
    require(log.size() == size_before_zero_delivery_append, "runtime event log should not mutate for zero cargo delivery result");

    auto invalid_delivery_result = delivery;
    invalid_delivery_result.total_amount = 11;
    const auto size_before_invalid_delivery_append = log.size();
    const auto invalid_delivery_summary = clc::sim::append_runtime_caravan_cargo_delivery_event(log, orchestration.arrival.arrival_elapsed_ticks, invalid_delivery_result);
    require(invalid_delivery_summary.events_appended == 0, "runtime event log should reject structurally invalid cargo delivery result");
    require(invalid_delivery_summary.cargo_events == 0, "runtime event log should not count invalid cargo delivery result");
    require(log.size() == size_before_invalid_delivery_append, "runtime event log should not mutate for invalid cargo delivery result");

    const auto delivery_analysis = clc::sim::analyze_runtime_event_log(log);
    require(delivery_analysis.first_tick == clc::ticks_per_day(), "runtime event analysis should expose first runtime tick");
    require(delivery_analysis.last_tick == clc::days_to_ticks(2), "runtime event analysis should expose last runtime tick");
    require(delivery_analysis.caravan_cargo_delivered_events == 1, "runtime event analysis should count cargo delivery events");

    auto bulk_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bulk_bootstrap.ok(), "runtime bulk event bootstrap should succeed");
    auto& bulk_runtime = bulk_bootstrap.runtime;
    require(bulk_runtime.engine.add_resource_to_settlement("riverwatch", "grain", 50).ok(), "runtime bulk event origin should receive extra grain");

    auto bulk_a = clc::sim::create_runtime_caravan_for_route(
        bulk_runtime,
        "riverwatch_to_hillford",
        "event_bulk_a",
        "Event Bulk A"
    );
    require(bulk_a.ok(), "runtime bulk event caravan a should create");
    auto bulk_b = clc::sim::create_runtime_caravan_for_route(
        bulk_runtime,
        "riverwatch_to_hillford",
        "event_bulk_b",
        "Event Bulk B"
    );
    require(bulk_b.ok(), "runtime bulk event caravan b should create");

    require(clc::sim::load_runtime_caravan_at_origin(bulk_runtime, "event_bulk_a", "grain", 15).ok(), "runtime bulk event caravan a should load");
    require(clc::sim::load_runtime_caravan_at_origin(bulk_runtime, "event_bulk_b", "grain", 20).ok(), "runtime bulk event caravan b should load");
    require(clc::sim::advance_runtime_caravan_day(bulk_runtime, "event_bulk_a").ok(), "runtime bulk event caravan a first advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(bulk_runtime, "event_bulk_a").ok(), "runtime bulk event caravan a second advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(bulk_runtime, "event_bulk_b").ok(), "runtime bulk event caravan b first advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(bulk_runtime, "event_bulk_b").ok(), "runtime bulk event caravan b second advance should succeed");

    const auto bulk_delivery = clc::sim::deliver_all_runtime_arrived_caravan_cargo_to_destinations(bulk_runtime);
    require(bulk_delivery.ok(), "runtime bulk event delivery should succeed");
    require(bulk_delivery.delivered_caravans == 2, "runtime bulk event delivery should count two caravans");
    require(bulk_delivery.total_amount == 35, "runtime bulk event delivery should total cargo amount");

    clc::EventLog bulk_log{};
    const auto bulk_summary = clc::sim::append_runtime_bulk_caravan_cargo_delivery_events(bulk_log, clc::days_to_ticks(2), bulk_delivery);
    require(bulk_summary.events_appended == 2, "runtime bulk event log should append two cargo delivery events");
    require(bulk_summary.cargo_events == 2, "runtime bulk event log should count two cargo delivery events");
    require(bulk_summary.day_events == 0, "runtime bulk event log should not count day events");
    require(bulk_summary.caravan_events == 0, "runtime bulk event log should not count caravan progress events");
    require(bulk_summary.contract_events == 0, "runtime bulk event log should not count contract events");
    require(bulk_log.size() == 2, "runtime bulk event log should contain two events");
    require(bulk_log.events()[0].tick == clc::days_to_ticks(2), "runtime bulk event log first tick should be runtime tick");
    require(bulk_log.events()[0].type == "runtime.caravan.cargo_delivered", "runtime bulk event log first event should be cargo delivery");
    require(bulk_log.events()[0].payload == "event_bulk_a->hillford:total=15", "runtime bulk event log first payload should be deterministic");
    require(bulk_log.events()[1].payload == "event_bulk_b->hillford:total=20", "runtime bulk event log second payload should be deterministic");
    require(clc::sim::validate_runtime_event_log(bulk_log).ok(), "runtime bulk event log should validate");

    auto invalid_bulk_delivery = bulk_delivery;
    invalid_bulk_delivery.delivered_caravans = 3;
    const auto bulk_size_before_invalid_append = bulk_log.size();
    const auto invalid_bulk_summary = clc::sim::append_runtime_bulk_caravan_cargo_delivery_events(bulk_log, clc::days_to_ticks(2), invalid_bulk_delivery);
    require(invalid_bulk_summary.events_appended == 0, "runtime bulk event log should reject structurally invalid bulk delivery result");
    require(invalid_bulk_summary.cargo_events == 0, "runtime bulk event log should not count invalid bulk delivery result");
    require(bulk_log.size() == bulk_size_before_invalid_append, "runtime bulk event log should not mutate for invalid bulk delivery result");

    const auto bulk_analysis = clc::sim::analyze_runtime_event_log(bulk_log);
    require(bulk_analysis.first_tick == clc::days_to_ticks(2), "runtime bulk event analysis should expose runtime tick");
    require(bulk_analysis.caravan_cargo_delivered_events == 2, "runtime bulk event analysis should count cargo delivery events");
    require(bulk_analysis.total_events == 2, "runtime bulk event analysis should count total events");

    const auto repeated_bulk_delivery = clc::sim::deliver_all_runtime_arrived_caravan_cargo_to_destinations(bulk_runtime);
    require(repeated_bulk_delivery.ok(), "runtime repeated bulk delivery should succeed");
    require(repeated_bulk_delivery.deliveries.empty(), "runtime repeated bulk delivery should produce no deliveries");
    const auto repeated_bulk_summary = clc::sim::append_runtime_bulk_caravan_cargo_delivery_events(bulk_log, clc::days_to_ticks(2), repeated_bulk_delivery);
    require(repeated_bulk_summary.events_appended == 0, "runtime empty bulk event append should append no events");
    require(bulk_log.size() == 2, "runtime empty bulk event append should not mutate log");

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
    require(failed_analysis.last_tick == clc::days_to_ticks(2), "runtime failed-contract event analysis should expose runtime tick");
    require(failed_analysis.contract_failed_events == 1, "runtime event analysis should count failed contracts");
    require(failed_analysis.contract_fulfilled_events == 0, "runtime event analysis should not count fulfilled contracts in failed run");
    require(clc::sim::validate_runtime_event_log(failed_log).ok(), "runtime failed-contract event log should validate");

    bool found_failed_contract_event = false;
    for (const auto& event : failed_log.events()) {
        if (event.type == "runtime.contract.failed" && event.payload == "grain_delivery_runtime" && event.tick == clc::days_to_ticks(2)) {
            found_failed_contract_event = true;
        }
    }
    require(found_failed_contract_event, "runtime event log should record failed contract event payload at runtime tick");

    return 0;
}
