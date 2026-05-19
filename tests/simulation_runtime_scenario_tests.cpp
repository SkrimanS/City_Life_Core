#include "clc/sim/SimulationRuntimeScenario.hpp"

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
    require(bootstrap.ok(), "basic runtime scenario bootstrap should succeed");

    auto& runtime = bootstrap.runtime;

    require(runtime.engine.has_settlement("riverwatch"), "bootstrap should create origin settlement");
    require(runtime.engine.has_settlement("hillford"), "bootstrap should create destination settlement");
    require(runtime.routes.routes.size() == 1, "bootstrap should create route");
    require(runtime.factions.factions.size() == 2, "bootstrap should create factions");
    require(runtime.contracts.contracts.size() == 1, "bootstrap should create contract");
    require(runtime.wallet.coins == 10, "bootstrap should initialize wallet");
    require(runtime.engine.settlement_resource_amount("riverwatch", "grain") == 50, "bootstrap should seed origin storage");

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "scenario_caravan",
        "Scenario Caravan"
    );
    require(caravan.ok(), "bootstrap scenario should create runtime caravan");

    require(clc::sim::set_runtime_caravan_owner(runtime, "scenario_caravan", "riverwatch").ok(), "bootstrap scenario should set caravan owner");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "scenario_caravan", "grain", 40).ok(), "bootstrap scenario should load caravan cargo");

    auto advance_one = clc::sim::advance_runtime_caravan_day(runtime, "scenario_caravan");
    require(advance_one.ok(), "bootstrap scenario should advance caravan first day");
    auto advance_two = clc::sim::advance_runtime_caravan_day(runtime, "scenario_caravan");
    require(advance_two.ok(), "bootstrap scenario should advance caravan second day");
    require(advance_two.report.arrived, "bootstrap scenario caravan should arrive");

    auto fulfillment = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "scenario_caravan",
        "riverwatch"
    );
    require(fulfillment.ok(), "bootstrap scenario should fulfill contract");

    require(runtime.wallet.coins == 85, "bootstrap scenario should reward wallet");
    require(runtime.ledger.entries().size() == 1, "bootstrap scenario should record ledger entry");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 10, "bootstrap scenario should leave remaining caravan cargo");

    auto unload = clc::sim::unload_runtime_caravan_at_destination(runtime, "scenario_caravan", "grain", 10);
    require(unload.ok(), "bootstrap scenario should unload remaining cargo");
    require(runtime.engine.settlement_resource_amount("hillford", "grain") == 10, "bootstrap scenario should credit destination storage");

    auto captured = clc::sim::capture_simulation_world_state(runtime);
    require(clc::sim::validate_simulation_world_state(captured).ok(), "bootstrap scenario should produce valid world state");

    return 0;
}
