#include "clc/core/Time.hpp"
#include "clc/core/Version.hpp"
#include "clc/sim/Routes.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <iostream>

int main() {
    std::cout << "City Life Core " << clc::core_version_string() << " tick runtime example\n";

    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        std::cerr << "failed to create basic runtime scenario\n";
        return 1;
    }

    auto& runtime = bootstrap.runtime;

    auto route_report = clc::sim::add_runtime_route(runtime, clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford_3h",
        "Riverwatch to Hillford 3h",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(3)
    ));
    if (!route_report.ok()) {
        std::cerr << "failed to add tick route\n";
        return 1;
    }

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford_3h",
        "tick_example_caravan",
        "Tick Example Caravan"
    );
    if (!caravan.ok()) {
        std::cerr << "failed to create tick caravan\n";
        return 1;
    }

    if (!clc::sim::set_runtime_caravan_owner(runtime, "tick_example_caravan", "riverwatch").ok()) {
        std::cerr << "failed to assign caravan owner\n";
        return 1;
    }

    if (!clc::sim::load_runtime_caravan_at_origin(runtime, "tick_example_caravan", "grain", 40).ok()) {
        std::cerr << "failed to load caravan cargo\n";
        return 1;
    }

    const auto fulfilled = clc::sim::run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(
        runtime,
        clc::hours_to_ticks(6),
        clc::minutes_to_ticks(30),
        "riverwatch"
    );
    if (!fulfilled.ok()) {
        std::cerr << "failed to reach arrival and fulfill contract\n";
        return 1;
    }

    const auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(
        runtime,
        "tick_example_caravan"
    );
    if (!delivery.ok()) {
        std::cerr << "failed to deliver remaining cargo\n";
        return 1;
    }

    std::cout << "Arrival tick: " << fulfilled.arrival.arrival_tick << "\n";
    std::cout << "Arrived caravan: " << fulfilled.arrival.arrived_caravan_id << "\n";
    std::cout << "Fulfilled contract: " << fulfilled.fulfillment.contract_id << "\n";
    std::cout << "Wallet coins: " << runtime.wallet.coins << "\n";
    std::cout << "Ledger entries: " << runtime.ledger.entries().size() << "\n";
    std::cout << "Remaining cargo delivered: " << delivery.total_amount << "\n";
    std::cout << "Destination grain: " << runtime.engine.settlement_resource_amount("hillford", "grain") << "\n";

    return 0;
}
