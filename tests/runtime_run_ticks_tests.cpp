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
    require(bootstrap.ok(), "runtime tick-run bootstrap should succeed");
    auto& runtime = bootstrap.runtime;

    require(clc::sim::add_runtime_route(runtime, clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford_3h",
        "Riverwatch to Hillford 3h",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(3)
    )).ok(), "runtime tick-run route should add");

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford_3h",
        "tick_run_caravan",
        "Tick Run Caravan"
    );
    require(created.ok(), "runtime tick-run caravan should create");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "tick_run_caravan", "grain", 10).ok(), "runtime tick-run caravan should load");

    const auto invalid_run = clc::sim::run_runtime_ticks(runtime, clc::hours_to_ticks(1), 0);
    require(!invalid_run.ok(), "runtime tick-run should reject zero step_ticks");
    require(runtime.time.current_tick() == 0, "runtime tick-run invalid call should not advance clock");

    const auto run = clc::sim::run_runtime_ticks(runtime, clc::hours_to_ticks(2), clc::hours_to_ticks(1));
    require(run.ok(), "runtime tick-run should succeed");
    require(run.reports.size() == 2, "runtime tick-run should emit one report per step");
    require(run.summary.tick_steps == 2, "runtime tick-run summary should count steps");
    require(run.summary.first_tick == clc::hours_to_ticks(1), "runtime tick-run summary should expose first tick");
    require(run.summary.last_tick == clc::hours_to_ticks(2), "runtime tick-run summary should expose last tick");
    require(run.summary.ticks_elapsed == clc::hours_to_ticks(2), "runtime tick-run summary should expose elapsed ticks");
    require(run.summary.caravan_ticks == 2, "runtime tick-run summary should count caravan ticks");
    require(run.summary.caravan_arrivals == 0, "runtime tick-run should not report early arrival");
    require(runtime.time.current_tick() == clc::hours_to_ticks(2), "runtime tick-run should advance runtime clock");
    require(runtime.engine.current_day() == 0, "runtime tick-run should not advance daily engine");

    const auto until_arrival = clc::sim::run_runtime_until_first_caravan_arrival_by_ticks(
        runtime,
        clc::hours_to_ticks(4),
        clc::minutes_to_ticks(30)
    );
    require(until_arrival.ok(), "runtime tick-run until arrival should succeed");
    require(until_arrival.arrival_reached, "runtime tick-run until arrival should reach arrival");
    require(until_arrival.arrived_caravan_id == "tick_run_caravan", "runtime tick-run until arrival should expose caravan id");
    require(until_arrival.arrival_tick == clc::hours_to_ticks(3), "runtime tick-run until arrival should expose absolute arrival tick");
    require(until_arrival.arrival_elapsed_ticks == clc::hours_to_ticks(1), "runtime tick-run until arrival should expose elapsed ticks within this run");
    require(until_arrival.run.summary.tick_steps == 2, "runtime tick-run until arrival should stop when arrival reached");
    require(until_arrival.run.summary.ticks_elapsed == clc::hours_to_ticks(1), "runtime tick-run until arrival summary should expose elapsed run ticks");
    require(until_arrival.run.summary.caravan_arrivals == 1, "runtime tick-run until arrival summary should count arrival");
    require(runtime.time.current_tick() == clc::hours_to_ticks(3), "runtime tick-run until arrival should stop at arrival tick");
    require(runtime.engine.current_day() == 0, "runtime tick-run until arrival should not advance daily engine");

    const auto invalid_until = clc::sim::run_runtime_until_first_caravan_arrival_by_ticks(runtime, clc::hours_to_ticks(1), 0);
    require(!invalid_until.ok(), "runtime tick-run until arrival should reject zero step_ticks");

    auto fulfillment_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(fulfillment_bootstrap.ok(), "runtime tick-run fulfillment bootstrap should succeed");
    auto& fulfillment_runtime = fulfillment_bootstrap.runtime;
    require(clc::sim::add_runtime_route(fulfillment_runtime, clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford_contract_3h",
        "Riverwatch to Hillford Contract 3h",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(3)
    )).ok(), "runtime tick-run fulfillment route should add");
    auto fulfillment_created = clc::sim::create_runtime_caravan_for_route(
        fulfillment_runtime,
        "riverwatch_to_hillford_contract_3h",
        "tick_run_contract_caravan",
        "Tick Run Contract Caravan"
    );
    require(fulfillment_created.ok(), "runtime tick-run fulfillment caravan should create");
    require(clc::sim::set_runtime_caravan_owner(fulfillment_runtime, "tick_run_contract_caravan", "riverwatch").ok(), "runtime tick-run fulfillment owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(fulfillment_runtime, "tick_run_contract_caravan", "grain", 40).ok(), "runtime tick-run fulfillment caravan should load contract cargo");

    const auto fulfilled = clc::sim::run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(
        fulfillment_runtime,
        clc::hours_to_ticks(4),
        clc::minutes_to_ticks(30),
        "riverwatch"
    );
    require(fulfilled.ok(), "runtime tick-run fulfillment helper should succeed");
    require(fulfilled.arrival.arrival_reached, "runtime tick-run fulfillment helper should reach arrival");
    require(fulfilled.arrival.arrival_tick == clc::hours_to_ticks(3), "runtime tick-run fulfillment helper should expose arrival tick");
    require(fulfilled.arrival.arrived_caravan_id == "tick_run_contract_caravan", "runtime tick-run fulfillment helper should expose arrived caravan id");
    require(fulfilled.fulfillment.contract_id == "grain_delivery_runtime", "runtime tick-run fulfillment helper should fulfill matching contract");
    require(fulfillment_runtime.wallet.coins == 85, "runtime tick-run fulfillment helper should credit reward wallet");
    require(fulfillment_runtime.ledger.entries().size() == 1, "runtime tick-run fulfillment helper should record ledger entry");
    require(fulfillment_runtime.engine.current_day() == 0, "runtime tick-run fulfillment helper should not advance daily engine");
    require(fulfillment_runtime.time.current_tick() == clc::hours_to_ticks(3), "runtime tick-run fulfillment helper should stop at arrival tick");

    const auto no_arrival = clc::sim::run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(
        fulfillment_runtime,
        clc::minutes_to_ticks(30),
        clc::minutes_to_ticks(30),
        "riverwatch"
    );
    require(!no_arrival.ok(), "runtime tick-run fulfillment helper should reject missing arrival/contract after caravan already fulfilled");

    return 0;
}
