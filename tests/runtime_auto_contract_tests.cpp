#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

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
    require(bootstrap.ok(), "runtime auto contract bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "auto_contract_caravan",
        "Auto Contract Caravan"
    );
    require(created.ok(), "runtime auto contract caravan should create");

    require(clc::sim::set_runtime_caravan_owner(runtime, "auto_contract_caravan", "riverwatch").ok(), "runtime auto contract owner should set");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "auto_contract_caravan", "grain", 40).ok(), "runtime auto contract cargo should load");

    auto arrival = clc::sim::run_runtime_until_first_caravan_arrival(runtime, 5);
    require(arrival.ok(), "runtime auto contract arrival run should succeed");
    require(arrival.arrival_reached, "runtime auto contract should arrive caravan");

    auto fulfilled = clc::sim::fulfill_first_runtime_contract_for_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "auto_contract_caravan",
        "riverwatch"
    );
    require(fulfilled.ok(), "runtime auto contract helper should fulfill matching contract");
    require(fulfilled.contract_id == "grain_delivery_runtime", "runtime auto contract helper should choose matching contract");
    require(fulfilled.reward_coins == 75, "runtime auto contract helper should preserve reward amount");

    require(runtime.wallet.coins == 85, "runtime auto contract helper should reward wallet");
    require(runtime.ledger.entries().size() == 1, "runtime auto contract helper should record ledger entry");
    require(runtime.contracts.contracts[0].status == clc::sim::ContractStatus::fulfilled, "runtime auto contract helper should mark contract fulfilled");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 10, "runtime auto contract helper should debit delivered cargo");

    auto second_attempt = clc::sim::fulfill_first_runtime_contract_for_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "auto_contract_caravan",
        "riverwatch"
    );
    require(!second_attempt.ok(), "runtime auto contract helper should reject second fulfillment without matching open contract");

    return 0;
}
