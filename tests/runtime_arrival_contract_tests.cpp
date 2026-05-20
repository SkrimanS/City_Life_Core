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
    require(bootstrap.ok(), "arrival contract bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "arrival_contract_caravan",
        "Arrival Contract Caravan"
    );
    require(caravan.ok(), "arrival contract caravan should create");

    require(clc::sim::set_runtime_caravan_owner(runtime, "arrival_contract_caravan", "riverwatch").ok(), "arrival contract owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "arrival_contract_caravan", "grain", 40).ok(), "arrival contract cargo should load");

    auto too_short = clc::sim::run_runtime_until_first_caravan_arrival_and_fulfill_contract(
        runtime,
        1,
        "riverwatch"
    );
    require(!too_short.ok(), "arrival contract helper should reject when max_days ends before arrival");
    require(!too_short.arrival.arrival_reached, "arrival contract helper should report no arrival before limit");
    require(runtime.wallet.coins == 10, "failed arrival contract helper should not reward wallet");
    require(runtime.ledger.entries().empty(), "failed arrival contract helper should not record ledger");
    require(runtime.contracts.contracts[0].status == clc::sim::ContractStatus::open, "failed arrival contract helper should keep contract open");

    auto completed = clc::sim::run_runtime_until_first_caravan_arrival_and_fulfill_contract(
        runtime,
        5,
        "riverwatch"
    );
    require(completed.ok(), "arrival contract helper should succeed after arrival");
    require(completed.arrival.arrival_reached, "arrival contract helper should report arrival");
    require(completed.arrival.arrived_caravan_id == "arrival_contract_caravan", "arrival contract helper should report arrived caravan id");
    require(completed.arrival.arrival_day == 2, "arrival contract helper should report absolute arrival day");
    require(completed.fulfillment.contract_id == "grain_delivery_runtime", "arrival contract helper should fulfill matching contract");
    require(completed.fulfillment.reward_coins == 75, "arrival contract helper should report reward coins");

    require(runtime.wallet.coins == 85, "arrival contract helper should reward wallet");
    require(runtime.ledger.entries().size() == 1, "arrival contract helper should record ledger");
    require(runtime.contracts.contracts[0].status == clc::sim::ContractStatus::fulfilled, "arrival contract helper should fulfill contract");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 10, "arrival contract helper should debit cargo");

    auto repeated = clc::sim::run_runtime_until_first_caravan_arrival_and_fulfill_contract(
        runtime,
        1,
        "riverwatch"
    );
    require(!repeated.ok(), "arrival contract helper should reject repeated run without matching open contract");
    require(runtime.wallet.coins == 85, "repeated arrival contract helper should not double reward wallet");
    require(runtime.ledger.entries().size() == 1, "repeated arrival contract helper should not double record ledger");

    return 0;
}
