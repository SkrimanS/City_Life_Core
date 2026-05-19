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
    require(bootstrap.ok(), "runtime run bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "runner_caravan",
        "Runner Caravan"
    );
    require(created.ok(), "runtime run caravan should create");

    require(clc::sim::set_runtime_caravan_owner(runtime, "runner_caravan", "riverwatch").ok(), "runtime run owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "runner_caravan", "grain", 40).ok(), "runtime run cargo should load");

    auto run = clc::sim::run_runtime_days(runtime, 2);
    require(run.ok(), "runtime multi-day run should succeed");
    require(run.reports.size() == 2, "runtime multi-day run should emit two reports");
    require(run.summary.days_run == 2, "runtime multi-day summary should count days");
    require(run.summary.first_day == 1, "runtime multi-day summary should track first day");
    require(run.summary.last_day == 2, "runtime multi-day summary should track last day");
    require(run.summary.caravan_ticks == 2, "runtime multi-day summary should count caravan ticks");
    require(run.summary.caravan_arrivals == 1, "runtime multi-day summary should count arrivals");

    require(run.reports[0].engine.day == 1, "runtime run report one should match engine day");
    require(run.reports[1].engine.day == 2, "runtime run report two should match engine day");
    require(run.reports[1].caravans[0].advance.arrived, "runtime run should arrive caravan on second report");

    auto fulfillment = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "runner_caravan",
        "riverwatch"
    );
    require(fulfillment.ok(), "runtime run flow should still fulfill contract");
    require(runtime.wallet.coins == 85, "runtime run flow should reward wallet");

    return 0;
}
