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
    require(first_day.ticks.tick_after == clc::ticks_per_day(), "runtime event log roundtrip first day should expose runtime tick");

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_event_log_roundtrip_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "event_log_roundtrip.clcs";

    require(clc::sim::save_simulation_runtime_to_file(runtime, file_path).ok(), "runtime event log roundtrip should save");

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    auto load_result = clc::sim::load_simulation_runtime_from_file(file_path, loaded);
    require(load_result.ok(), "runtime event log roundtrip should load");
    require(loaded.time.current_tick() == clc::ticks_per_day(), "runtime event log roundtrip should restore runtime clock after save");

    auto completed = clc::sim::run_runtime_until_first_caravan_arrival_and_fulfill_contract(
        loaded,
        5,
        "riverwatch"
    );
    require(completed.ok(), "loaded runtime event log roundtrip should complete arrival contract helper");
    require(completed.arrival.arrival_reached, "loaded runtime event log roundtrip should reach arrival");
    require(completed.arrival.arrival_day == 2, "loaded runtime event log roundtrip should preserve absolute arrival day");
    require(completed.arrival.run.reports.back().ticks.tick_after == clc::days_to_ticks(2), "loaded runtime event log roundtrip should preserve absolute arrival tick");

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

    auto cargo_summary = clc::sim::append_runtime_caravan_cargo_delivery_event(log, clc::days_to_ticks(2), delivery);
    require(cargo_summary.events_appended == 1, "runtime event log roundtrip should append cargo delivery event");
    require(cargo_summary.day_events == 0, "runtime event log roundtrip cargo summary should not count day events");
    require(cargo_summary.caravan_events == 0, "runtime event log roundtrip cargo summary should not count caravan progress events");
    require(cargo_summary.cargo_events == 1, "runtime event log roundtrip cargo summary should count one cargo event");
    require(cargo_summary.contract_events == 0, "runtime event log roundtrip cargo summary should not count contract events");

    require(log.size() == 6, "runtime event log roundtrip should contain six events total");
    const auto& events = log.events();

    require(events[0].tick == clc::ticks_per_day(), "runtime event log roundtrip first event should use runtime tick day one");
    require(events[1].type == "runtime.caravan.progress", "runtime event log roundtrip second event should be progress");
    require(events[3].tick == clc::days_to_ticks(2), "runtime event log roundtrip arrival event should use runtime tick day two");
    require(events[3].type == "runtime.caravan.arrived", "runtime event log roundtrip should record arrival after load");
    require(events[4].tick == clc::days_to_ticks(2), "runtime event log roundtrip contract event should use arrival runtime tick");
    require(events[4].type == "runtime.contract.fulfilled", "runtime event log roundtrip should record contract fulfillment after load");
    require(events[4].payload == "grain_delivery_runtime", "runtime event log roundtrip contract payload should use contract id");
    require(events[5].tick == clc::days_to_ticks(2), "runtime event log roundtrip cargo event should use arrival runtime tick");
    require(events[5].type == "runtime.caravan.cargo_delivered", "runtime event log roundtrip should record cargo delivery after load");
    require(events[5].payload == "event_log_roundtrip_caravan->hillford:total=10", "runtime event log roundtrip cargo payload should be deterministic");

    const auto analysis = clc::sim::analyze_runtime_event_log(log);
    require(analysis.total_events == 6, "runtime event log roundtrip analysis should count all events");
    require(analysis.first_tick == clc::ticks_per_day(), "runtime event log roundtrip analysis should expose first runtime tick");
    require(analysis.last_tick == clc::days_to_ticks(2), "runtime event log roundtrip analysis should expose last runtime tick");
    require(analysis.caravan_cargo_delivered_events == 1, "runtime event log roundtrip analysis should count cargo delivery event");
    require(analysis.contract_fulfilled_events == 1, "runtime event log roundtrip analysis should count contract fulfillment event");
    require(analysis.unknown_events == 0, "runtime event log roundtrip analysis should not report unknown events");

    const auto validation = clc::sim::validate_runtime_event_log(log);
    require(validation.ok(), "runtime event log roundtrip should pass combined event validation");

    const clc::EventLog expected = log;
    const auto pair_validation = clc::sim::validate_runtime_event_logs_match(expected, log);
    require(pair_validation.ok(), "runtime event log roundtrip should pass pair event log validation");

    clc::EventLog drifted = log;
    drifted.append(clc::days_to_ticks(3), "runtime.caravan.cargo_delivered", "event_log_roundtrip_caravan->hillford:total=11");
    const auto drift_validation = clc::sim::validate_runtime_event_logs_match(expected, drifted);
    require(!drift_validation.ok(), "runtime event log roundtrip should reject cargo delivery event drift");

    auto bulk_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bulk_bootstrap.ok(), "runtime event log bulk roundtrip bootstrap should succeed");
    auto& bulk_runtime = bulk_bootstrap.runtime;
    require(bulk_runtime.engine.add_resource_to_settlement("riverwatch", "grain", 50).ok(), "runtime event log bulk roundtrip origin top-up should succeed");

    auto bulk_a = clc::sim::create_runtime_caravan_for_route(
        bulk_runtime,
        "riverwatch_to_hillford",
        "event_log_roundtrip_bulk_a",
        "Event Log Roundtrip Bulk A"
    );
    require(bulk_a.ok(), "runtime event log bulk roundtrip caravan a should create");
    auto bulk_b = clc::sim::create_runtime_caravan_for_route(
        bulk_runtime,
        "riverwatch_to_hillford",
        "event_log_roundtrip_bulk_b",
        "Event Log Roundtrip Bulk B"
    );
    require(bulk_b.ok(), "runtime event log bulk roundtrip caravan b should create");

    require(clc::sim::load_runtime_caravan_at_origin(bulk_runtime, "event_log_roundtrip_bulk_a", "grain", 15).ok(), "runtime event log bulk roundtrip caravan a should load");
    require(clc::sim::load_runtime_caravan_at_origin(bulk_runtime, "event_log_roundtrip_bulk_b", "grain", 20).ok(), "runtime event log bulk roundtrip caravan b should load");

    const auto bulk_file_path = directory / "event_log_bulk_roundtrip.clcs";
    require(clc::sim::save_simulation_runtime_to_file(bulk_runtime, bulk_file_path).ok(), "runtime event log bulk roundtrip should save");

    clc::sim::SimulationRuntime bulk_loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto bulk_load_result = clc::sim::load_simulation_runtime_from_file(bulk_file_path, bulk_loaded);
    require(bulk_load_result.ok(), "runtime event log bulk roundtrip should load");

    require(clc::sim::advance_runtime_caravan_day(bulk_loaded, "event_log_roundtrip_bulk_a").ok(), "runtime event log bulk roundtrip caravan a first advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(bulk_loaded, "event_log_roundtrip_bulk_a").ok(), "runtime event log bulk roundtrip caravan a second advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(bulk_loaded, "event_log_roundtrip_bulk_b").ok(), "runtime event log bulk roundtrip caravan b first advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(bulk_loaded, "event_log_roundtrip_bulk_b").ok(), "runtime event log bulk roundtrip caravan b second advance should succeed");

    const auto bulk_delivery = clc::sim::deliver_all_runtime_arrived_caravan_cargo_to_destinations(bulk_loaded);
    require(bulk_delivery.ok(), "runtime event log bulk roundtrip delivery should succeed");
    require(bulk_delivery.delivered_caravans == 2, "runtime event log bulk roundtrip should deliver two caravans");
    require(bulk_delivery.total_amount == 35, "runtime event log bulk roundtrip should deliver expected total cargo");
    require(bulk_loaded.engine.settlement_resource_amount("hillford", "grain") == 35, "runtime event log bulk roundtrip should credit destination storage");

    clc::EventLog bulk_log{};
    const auto bulk_summary = clc::sim::append_runtime_bulk_caravan_cargo_delivery_events(bulk_log, clc::days_to_ticks(2), bulk_delivery);
    require(bulk_summary.events_appended == 2, "runtime event log bulk roundtrip should append two cargo events");
    require(bulk_summary.cargo_events == 2, "runtime event log bulk roundtrip should count two cargo events");
    require(bulk_log.size() == 2, "runtime event log bulk roundtrip should contain two events");
    require(bulk_log.events()[0].tick == clc::days_to_ticks(2), "runtime event log bulk roundtrip first cargo event should use runtime tick");
    require(bulk_log.events()[0].payload == "event_log_roundtrip_bulk_a->hillford:total=15", "runtime event log bulk roundtrip first payload should be deterministic");
    require(bulk_log.events()[1].payload == "event_log_roundtrip_bulk_b->hillford:total=20", "runtime event log bulk roundtrip second payload should be deterministic");
    require(clc::sim::validate_runtime_event_log(bulk_log).ok(), "runtime event log bulk roundtrip should pass combined event validation");

    const auto bulk_analysis = clc::sim::analyze_runtime_event_log(bulk_log);
    require(bulk_analysis.total_events == 2, "runtime event log bulk roundtrip analysis should count all events");
    require(bulk_analysis.first_tick == clc::days_to_ticks(2), "runtime event log bulk roundtrip analysis should expose runtime tick");
    require(bulk_analysis.caravan_cargo_delivered_events == 2, "runtime event log bulk roundtrip analysis should count cargo delivery events");
    require(bulk_analysis.unknown_events == 0, "runtime event log bulk roundtrip analysis should not report unknown events");

    const clc::EventLog bulk_expected = bulk_log;
    require(clc::sim::validate_runtime_event_logs_match(bulk_expected, bulk_log).ok(), "runtime event log bulk roundtrip should pass pair validation");
    clc::EventLog bulk_drifted = bulk_log;
    bulk_drifted.append(clc::days_to_ticks(2), "runtime.caravan.cargo_delivered", "event_log_roundtrip_bulk_b->hillford:total=21");
    require(!clc::sim::validate_runtime_event_logs_match(bulk_expected, bulk_drifted).ok(), "runtime event log bulk roundtrip should reject cargo delivery payload drift");

    std::filesystem::remove_all(directory);
    return 0;
}
