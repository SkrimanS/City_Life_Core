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
    require(bootstrap.ok(), "runtime arrival bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "arrival_caravan",
        "Arrival Caravan"
    );
    require(created.ok(), "runtime arrival caravan should create");

    require(clc::sim::set_runtime_caravan_owner(runtime, "arrival_caravan", "riverwatch").ok(), "runtime arrival owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "arrival_caravan", "grain", 40).ok(), "runtime arrival cargo should load");

    auto incomplete = clc::sim::run_runtime_until_first_caravan_arrival(runtime, 1);
    require(incomplete.ok(), "runtime arrival limited run should succeed");
    require(!incomplete.arrival_reached, "runtime arrival limited run should not reach destination");
    require(incomplete.run.summary.days_run == 1, "runtime arrival limited run should advance one day");

    auto completed = clc::sim::run_runtime_until_first_caravan_arrival(runtime, 5);
    require(completed.ok(), "runtime arrival completion run should succeed");
    require(completed.arrival_reached, "runtime arrival completion should detect arrival");
    require(completed.arrived_caravan_id == "arrival_caravan", "runtime arrival completion should report caravan id");
    require(completed.arrival_day == 2, "runtime arrival completion should report arrival day");
    require(completed.run.summary.caravan_arrivals == 1, "runtime arrival completion should count arrival");
    require(completed.run.summary.last_day == 2, "runtime arrival completion should stop after arrival");

    auto fulfillment = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "arrival_caravan",
        "riverwatch"
    );
    require(fulfillment.ok(), "runtime arrival flow should still fulfill contract");
    require(runtime.wallet.coins == 85, "runtime arrival flow should reward wallet");

    return 0;
}
