#include "clc/CityLifeCore.hpp"

#include <iostream>
#include <string_view>

namespace {

void print_validation(const clc::data::ValidationReport& report) {
    for (const auto& message : report.messages()) {
        std::cerr << message.path << ": " << message.message << '\n';
    }
}

bool require_ok(std::string_view step, const clc::data::ValidationReport& report) {
    if (report.ok()) {
        return true;
    }
    std::cerr << "Failed step: " << step << '\n';
    print_validation(report);
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

    if (!require_ok("create riverwatch", clc::sim::create_runtime_settlement(runtime, "riverwatch"))) {
        return 1;
    }
    if (!require_ok("create hillford", clc::sim::create_runtime_settlement(runtime, "hillford"))) {
        return 1;
    }

    if (!require_ok("add riverwatch faction", clc::sim::add_runtime_faction(runtime, clc::sim::FactionState{
        .id = "riverwatch_faction",
        .display_name = "Riverwatch Faction",
    }))) {
        return 1;
    }
    if (!require_ok("add hillford faction", clc::sim::add_runtime_faction(runtime, clc::sim::FactionState{
        .id = "hillford_faction",
        .display_name = "Hillford Faction",
    }))) {
        return 1;
    }

    if (!require_ok("set faction reputation", clc::sim::set_runtime_faction_reputation(runtime, "riverwatch_faction", "hillford_faction", 25))) {
        return 1;
    }

    if (!require_ok("set settlement owner", clc::sim::set_runtime_settlement_owner(runtime, "riverwatch", "riverwatch_faction"))) {
        return 1;
    }

    if (!require_ok("add route", clc::sim::add_runtime_route(runtime, clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford",
        "Riverwatch to Hillford",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(1)
    )))) {
        return 1;
    }

    if (!require_ok("stock grain", runtime.engine.add_resource_to_settlement("riverwatch", "grain", 100))) {
        return 1;
    }

    if (!require_ok("add contract", clc::sim::add_runtime_resource_delivery_contract(runtime, clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery",
        .display_name = "Grain Delivery",
        .issuer_faction_id = "riverwatch_faction",
        .receiver_faction_id = "hillford_faction",
        .resource_id = "grain",
        .quantity = 20,
        .reward_coins = 50,
        .due_day = 0,
        .due_ticks = clc::hours_to_ticks(2),
    }))) {
        return 1;
    }

    auto created = clc::sim::create_runtime_caravan_for_route(runtime, "riverwatch_to_hillford", "caravan_01", "Caravan 01");
    if (!created.ok()) {
        print_validation(created.validation);
        return 1;
    }

    if (!require_ok("assign wrong owner first", clc::sim::set_runtime_caravan_owner(runtime, "caravan_01", "hillford_faction"))) {
        return 1;
    }
    if (!require_ok("load cargo", clc::sim::load_runtime_caravan_at_origin(runtime, "caravan_01", "grain", 25))) {
        return 1;
    }

    auto run = clc::sim::run_runtime_ticks(runtime, clc::hours_to_ticks(1), clc::minutes_to_ticks(30));
    if (!run.ok()) {
        print_validation(run.validation);
        return 1;
    }

    auto wrong_owner = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery",
        "caravan_01",
        "riverwatch_faction"
    );
    if (wrong_owner.ok()) {
        std::cerr << "Contract fulfillment should fail when caravan owner does not match expected faction\n";
        return 1;
    }

    if (!require_ok("reassign correct owner", clc::sim::set_runtime_caravan_owner(runtime, "caravan_01", "riverwatch_faction"))) {
        return 1;
    }

    auto fulfilled = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery",
        "caravan_01",
        "riverwatch_faction"
    );
    if (!fulfilled.ok()) {
        print_validation(fulfilled.validation);
        return 1;
    }

    std::cout << "settlement_owner=" << clc::sim::settlement_owner(runtime.ownership, "riverwatch") << '\n';
    std::cout << "caravan_owner=" << clc::sim::caravan_owner(runtime.ownership, "caravan_01") << '\n';
    std::cout << "reputation=" << clc::sim::faction_reputation(runtime.factions, "riverwatch_faction", "hillford_faction") << '\n';
    std::cout << "owned_caravans=" << clc::sim::caravans_owned_by(runtime.ownership, "riverwatch_faction").size() << '\n';
    std::cout << "wallet_coins=" << runtime.wallet.coins << '\n';
    std::cout << "ledger_entries=" << runtime.ledger.entries().size() << '\n';

    return 0;
}
