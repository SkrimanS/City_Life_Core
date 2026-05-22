#include "clc/CityLifeCore.hpp"

#include <iostream>
#include <string_view>

namespace {

void print_failure(std::string_view step, const clc::data::ValidationReport& report) {
    std::cerr << "Failed step: " << step << '\n';
    for (const auto& message : report.messages()) {
        std::cerr << "  - " << message.path << ": " << message.message << '\n';
    }
}

bool require_ok(std::string_view step, const clc::data::ValidationReport& report) {
    if (report.ok()) {
        return true;
    }
    print_failure(step, report);
    return false;
}

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;

    registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    });

    registry.add(clc::data::SettlementDefinition{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .starting_population = 120,
    });

    registry.add(clc::data::SettlementDefinition{
        .id = "hillford",
        .display_name = "Hillford",
        .starting_population = 80,
    });

    return registry;
}

} // namespace

int main() {
    clc::sim::SimulationRuntime runtime{make_registry()};
    runtime.wallet.coins = 10;

    if (!require_ok("create origin settlement", clc::sim::create_runtime_settlement(runtime, "riverwatch"))) {
        return 1;
    }
    if (!require_ok("create destination settlement", clc::sim::create_runtime_settlement(runtime, "hillford"))) {
        return 1;
    }
    if (!require_ok("stock origin grain", runtime.engine.add_resource_to_settlement("riverwatch", "grain", 100))) {
        return 1;
    }

    if (!require_ok("add origin faction", clc::sim::add_runtime_faction(runtime, clc::sim::FactionState{
        .id = "riverwatch_faction",
        .display_name = "Riverwatch Faction",
    }))) {
        return 1;
    }
    if (!require_ok("add receiver faction", clc::sim::add_runtime_faction(runtime, clc::sim::FactionState{
        .id = "traders_guild",
        .display_name = "Traders Guild",
    }))) {
        return 1;
    }

    if (!require_ok("set origin owner", clc::sim::set_runtime_settlement_owner(runtime, "riverwatch", "riverwatch_faction"))) {
        return 1;
    }
    if (!require_ok("set destination owner", clc::sim::set_runtime_settlement_owner(runtime, "hillford", "traders_guild"))) {
        return 1;
    }

    const auto route = clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford_3h",
        "Riverwatch to Hillford 3h",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(3)
    );
    if (!require_ok("add route", clc::sim::add_runtime_route(runtime, route))) {
        return 1;
    }

    if (!require_ok("add delivery contract", clc::sim::add_runtime_resource_delivery_contract(runtime, clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery",
        .display_name = "Deliver grain to Hillford",
        .issuer_faction_id = "riverwatch_faction",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 25,
        .reward_coins = 75,
        .due_day = 0,
        .due_ticks = clc::hours_to_ticks(6),
    }))) {
        return 1;
    }

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford_3h",
        "grain_runner",
        "Grain Runner"
    );
    if (!caravan.ok()) {
        print_failure("create caravan", caravan.validation);
        return 1;
    }

    if (!require_ok("set caravan owner", clc::sim::set_runtime_caravan_owner(runtime, "grain_runner", "riverwatch_faction"))) {
        return 1;
    }
    if (!require_ok("load caravan cargo", clc::sim::load_runtime_caravan_at_origin(runtime, "grain_runner", "grain", 30))) {
        return 1;
    }

    auto run = clc::sim::run_runtime_ticks(
        runtime,
        clc::hours_to_ticks(3),
        clc::hours_to_ticks(1)
    );
    if (!run.ok()) {
        print_failure("run runtime ticks", run.validation);
        return 1;
    }

    auto fulfillment = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery",
        "grain_runner",
        "riverwatch_faction"
    );
    if (!fulfillment.ok()) {
        print_failure("fulfill contract", fulfillment.validation);
        return 1;
    }

    std::cout << "runtime_tick=" << runtime.time.current_tick() << '\n';
    std::cout << "wallet_coins=" << runtime.wallet.coins << '\n';
    std::cout << "ledger_entries=" << runtime.ledger.entries().size() << '\n';
    std::cout << "contract_reward=" << fulfillment.reward_coins << '\n';
    std::cout << "remaining_caravan_grain=" << runtime.caravans.caravans[0].cargo.amount("grain") << '\n';

    return 0;
}
