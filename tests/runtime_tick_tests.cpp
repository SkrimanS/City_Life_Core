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
    require(bootstrap.ok(), "runtime tick bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "tick_caravan",
        "Tick Caravan"
    );
    require(created.ok(), "runtime tick caravan should create");

    require(clc::sim::set_runtime_caravan_owner(runtime, "tick_caravan", "riverwatch").ok(), "runtime tick owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "tick_caravan", "grain", 40).ok(), "runtime tick caravan should load");

    auto first_tick = clc::sim::advance_runtime_day(runtime);
    require(first_tick.ok(), "runtime tick should succeed");
    require(first_tick.engine.day == 1, "runtime tick should advance engine day");
    require(first_tick.ticks.elapsed_ticks == clc::ticks_per_day(), "runtime day tick should expose elapsed ticks");
    require(first_tick.caravans.size() == 1, "runtime tick should advance caravan fleet");
    require(first_tick.caravans[0].ok(), "runtime tick caravan advance should succeed");
    require(first_tick.caravans[0].advance.days_remaining_after == 1, "runtime tick should reduce caravan travel days");
    require(first_tick.caravans[0].advance.ticks_remaining_after == clc::ticks_per_day(), "runtime tick should reduce caravan travel ticks");
    require(!first_tick.caravans[0].advance.arrived, "runtime tick caravan should not arrive first day");
    require(first_tick.arrived_caravan_ids.empty(), "runtime tick should not report first-day arrivals");
    require(first_tick.contracts.failed_count == 0, "runtime tick should not fail contracts before due day");

    auto second_tick = clc::sim::advance_runtime_day(runtime);
    require(second_tick.ok(), "second runtime tick should succeed");
    require(second_tick.engine.day == 2, "second runtime tick should advance engine day again");
    require(second_tick.caravans.size() == 1, "second runtime tick should still advance caravan fleet");
    require(second_tick.caravans[0].advance.arrived, "runtime tick caravan should arrive on second day");
    require(second_tick.arrived_caravan_ids.size() == 1, "runtime tick should report arrived caravan ids");
    require(second_tick.arrived_caravan_ids[0] == "tick_caravan", "runtime tick should report arrived caravan id");
    require(second_tick.contracts.failed_count == 0, "runtime tick should keep fulfillable contract open before due day");

    auto fulfillment = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "tick_caravan",
        "riverwatch"
    );
    require(fulfillment.ok(), "runtime tick flow should still fulfill contracts after orchestrated ticks");
    require(runtime.wallet.coins == 85, "runtime tick flow should reward wallet");

    auto hourly_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(hourly_bootstrap.ok(), "hourly runtime tick bootstrap should succeed");
    auto& hourly_runtime = hourly_bootstrap.runtime;
    require(clc::sim::add_runtime_route(hourly_runtime, clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford_hours",
        "Riverwatch to Hillford Hours",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(3)
    )).ok(), "hourly runtime route should add");

    auto hourly_created = clc::sim::create_runtime_caravan_for_route(
        hourly_runtime,
        "riverwatch_to_hillford_hours",
        "runtime_hourly_caravan",
        "Runtime Hourly Caravan"
    );
    require(hourly_created.ok(), "hourly runtime caravan should create");

    require(clc::sim::load_runtime_caravan_at_origin(hourly_runtime, "runtime_hourly_caravan", "grain", 10).ok(), "hourly runtime caravan should load");
    const auto engine_day_before_hourly_ticks = hourly_runtime.engine.current_day();
    auto first_hour = clc::sim::advance_runtime_ticks(hourly_runtime, clc::hours_to_ticks(1));
    require(first_hour.ok(), "hourly runtime tick should succeed");
    require(first_hour.elapsed_ticks == clc::hours_to_ticks(1), "hourly runtime tick should expose elapsed ticks");
    require(first_hour.caravans.size() == 1, "hourly runtime tick should advance hourly caravan");
    require(first_hour.caravans[0].advance.ticks_remaining_after == clc::hours_to_ticks(2), "hourly runtime tick should leave two hours remaining");
    require(!first_hour.caravans[0].advance.arrived, "hourly runtime caravan should not arrive after one hour");
    require(first_hour.arrived_caravan_ids.empty(), "hourly runtime tick should not report early arrival");
    require(hourly_runtime.engine.current_day() == engine_day_before_hourly_ticks, "hourly runtime tick should not advance daily engine day");
    require(!clc::sim::load_runtime_caravan_at_origin(hourly_runtime, "runtime_hourly_caravan", "grain", 1).ok(), "hourly runtime caravan should reject loading after tick departure");

    auto final_hours = clc::sim::advance_runtime_ticks(hourly_runtime, clc::hours_to_ticks(2));
    require(final_hours.ok(), "hourly runtime final tick should succeed");
    require(final_hours.caravans[0].advance.ticks_remaining_after == 0, "hourly runtime final tick should exhaust remaining ticks");
    require(final_hours.caravans[0].advance.arrived, "hourly runtime caravan should arrive after three hours");
    require(final_hours.arrived_caravan_ids.size() == 1, "hourly runtime tick should report arrival");
    require(final_hours.arrived_caravan_ids[0] == "runtime_hourly_caravan", "hourly runtime tick should report arrived caravan id");
    require(hourly_runtime.engine.current_day() == engine_day_before_hourly_ticks, "hourly runtime arrival should not require daily engine advancement");

    clc::sim::RuntimeScenarioBootstrapConfig overdue_config{};
    overdue_config.contract_due_day = 1;
    auto overdue_bootstrap = clc::sim::make_basic_runtime_scenario(overdue_config);
    require(overdue_bootstrap.ok(), "overdue runtime tick bootstrap should succeed");

    auto overdue_run = clc::sim::run_runtime_days(overdue_bootstrap.runtime, 2);
    require(overdue_run.ok(), "overdue runtime run should succeed");
    require(overdue_run.reports.size() == 2, "overdue runtime run should produce two reports");
    require(overdue_run.summary.ticks_elapsed == clc::days_to_ticks(2), "runtime summary should count elapsed day ticks");
    require(overdue_run.reports[0].contracts.failed_count == 0, "contract should not fail on due day");
    require(overdue_run.reports[1].contracts.failed_count == 1, "contract should fail after due day");
    require(overdue_run.reports[1].contracts.failed_contract_ids[0] == "grain_delivery_runtime", "overdue report should expose failed contract id");
    require(overdue_run.summary.contract_failures == 1, "runtime summary should count contract failures");

    const auto* failed_contract = clc::sim::contract_by_id(overdue_bootstrap.runtime.contracts, "grain_delivery_runtime");
    require(failed_contract != nullptr, "failed contract should still exist");
    require(failed_contract->status == clc::sim::ContractStatus::failed, "overdue runtime tick should mark contract failed");

    return 0;
}
