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
    require(first_tick.caravans.size() == 1, "runtime tick should advance caravan fleet");
    require(first_tick.caravans[0].ok(), "runtime tick caravan advance should succeed");
    require(first_tick.caravans[0].advance.days_remaining_after == 1, "runtime tick should reduce caravan travel days");
    require(!first_tick.caravans[0].advance.arrived, "runtime tick caravan should not arrive first day");

    auto second_tick = clc::sim::advance_runtime_day(runtime);
    require(second_tick.ok(), "second runtime tick should succeed");
    require(second_tick.engine.day == 2, "second runtime tick should advance engine day again");
    require(second_tick.caravans.size() == 1, "second runtime tick should still advance caravan fleet");
    require(second_tick.caravans[0].advance.arrived, "runtime tick caravan should arrive on second day");

    auto fulfillment = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "tick_caravan",
        "riverwatch"
    );
    require(fulfillment.ok(), "runtime tick flow should still fulfill contracts after orchestrated ticks");
    require(runtime.wallet.coins == 85, "runtime tick flow should reward wallet");

    return 0;
}
