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

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "grain should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "riverwatch", .display_name = "Riverwatch", .starting_population = 120}).ok(), "riverwatch should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "hillford", .display_name = "Hillford", .starting_population = 80}).ok(), "hillford should register");
    return registry;
}

} // namespace

int main() {
    clc::sim::SimulationRuntime runtime{make_registry()};
    require(clc::sim::create_runtime_settlement(runtime, "riverwatch").ok(), "runtime should create origin settlement");
    require(clc::sim::create_runtime_settlement(runtime, "hillford").ok(), "runtime should create destination settlement");
    require(!clc::sim::add_runtime_route(runtime, clc::sim::SettlementRoute{
        .id = "bad_route",
        .display_name = "Bad Route",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "missing_settlement",
        .travel_days = 2,
    }).ok(), "runtime route should reject unknown settlement references");
    require(runtime.engine.add_resource_to_settlement("riverwatch", "grain", 50).ok(), "origin settlement should receive grain");
    require(clc::sim::add_runtime_route(runtime, clc::sim::SettlementRoute{
        .id = "riverwatch_to_hillford",
        .display_name = "Riverwatch to Hillford",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "hillford",
        .travel_days = 2,
    }).ok(), "runtime route should add");
    require(clc::sim::add_runtime_faction(runtime, clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "origin faction should add");
    require(clc::sim::add_runtime_faction(runtime, clc::sim::FactionState{.id = "traders_guild", .display_name = "Traders Guild"}).ok(), "receiver faction should add");
    require(!clc::sim::set_runtime_faction_reputation(runtime, "riverwatch", "missing_faction", 10).ok(), "runtime reputation should reject unknown faction");
    require(clc::sim::set_runtime_faction_reputation(runtime, "riverwatch", "traders_guild", 10).ok(), "runtime reputation should set");
    require(!clc::sim::set_runtime_settlement_owner(runtime, "missing_settlement", "riverwatch").ok(), "runtime settlement owner should reject unknown settlement");
    require(clc::sim::set_runtime_settlement_owner(runtime, "riverwatch", "riverwatch").ok(), "runtime settlement owner should set");
    require(!clc::sim::add_runtime_resource_delivery_contract(runtime, clc::sim::ResourceDeliveryContract{
        .id = "bad_contract",
        .display_name = "Bad Contract",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "missing_faction",
        .resource_id = "grain",
        .quantity = 30,
        .reward_coins = 75,
        .due_day = 8,
    }).ok(), "runtime contract should reject unknown faction references");
    require(clc::sim::add_runtime_resource_delivery_contract(runtime, clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery_runtime",
        .display_name = "Runtime Grain Delivery",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 30,
        .reward_coins = 75,
        .due_day = 8,
    }).ok(), "runtime contract should add");

    clc::sim::ResourceStorage cargo;
    require(cargo.add("grain", 40).ok(), "cargo should accept grain");
    const auto missing_route = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "missing_route",
        "bad_caravan",
        "Bad Caravan",
        cargo
    );
    require(!missing_route.ok(), "runtime caravan creation should reject unknown route");
    require(runtime.caravans.caravans.empty(), "failed caravan creation should not mutate fleet");

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "caravan_runtime",
        "Runtime Caravan"
    );
    require(created.ok(), "runtime caravan should create from route");
    require(runtime.caravans.caravans.size() == 1, "runtime fleet should contain created caravan");
    require(runtime.caravans.caravans[0].cargo.empty(), "created runtime caravan should start empty when no cargo is provided");

    require(!clc::sim::set_runtime_caravan_owner(runtime, "missing_caravan", "riverwatch").ok(), "runtime caravan owner should reject unknown caravan");
    require(!clc::sim::set_runtime_caravan_owner(runtime, "caravan_runtime", "missing_faction").ok(), "runtime caravan owner should reject unknown faction");
    require(clc::sim::set_runtime_caravan_owner(runtime, "caravan_runtime", "riverwatch").ok(), "runtime caravan owner should set");

    require(!clc::sim::load_runtime_caravan_at_origin(runtime, "missing_caravan", "grain", 1).ok(), "runtime load should reject unknown caravan");
    require(!clc::sim::load_runtime_caravan_at_origin(runtime, "caravan_runtime", "grain", 60).ok(), "runtime load should reject insufficient origin storage");
    require(runtime.engine.settlement_resource_amount("riverwatch", "grain") == 50, "failed runtime load should not debit origin storage");
    require(runtime.caravans.caravans[0].cargo.empty(), "failed runtime load should not credit cargo");

    require(clc::sim::load_runtime_caravan_at_origin(runtime, "caravan_runtime", "grain", 40).ok(), "runtime load should move grain from origin into cargo");
    require(runtime.engine.settlement_resource_amount("riverwatch", "grain") == 10, "runtime load should debit origin storage");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 40, "runtime load should credit caravan cargo");

    runtime.wallet.coins = 10;

    const auto early_unload = clc::sim::unload_runtime_caravan_at_destination(runtime, "caravan_runtime", "grain", 5);
    require(!early_unload.ok(), "runtime unload should require arrival");
    require(runtime.engine.settlement_resource_amount("hillford", "grain") == 0, "early unload should not credit destination");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 40, "early unload should not debit cargo");

    const auto early_delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "caravan_runtime");
    require(!early_delivery.ok(), "runtime delivery should require arrival");
    require(runtime.engine.settlement_resource_amount("hillford", "grain") == 0, "early delivery should not credit destination");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 40, "early delivery should not debit cargo");

    const auto early_fulfillment = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "caravan_runtime",
        "riverwatch"
    );
    require(!early_fulfillment.ok(), "runtime contract fulfillment should require caravan arrival");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 40, "early fulfillment should not debit cargo");
    require(runtime.wallet.coins == 10, "early fulfillment should not credit wallet");
    require(runtime.ledger.entries().empty(), "early fulfillment should not record ledger");

    auto first_day = clc::sim::advance_runtime_caravan_day(runtime, "caravan_runtime");
    require(first_day.ok(), "runtime caravan should advance day one");
    require(first_day.report.days_remaining_after == 1, "runtime caravan should have one day remaining");
    require(!clc::sim::load_runtime_caravan_at_origin(runtime, "caravan_runtime", "grain", 1).ok(), "runtime load should reject departed caravan");
    auto second_day = clc::sim::advance_runtime_caravan_day(runtime, "caravan_runtime");
    require(second_day.ok(), "runtime caravan should advance day two");
    require(second_day.report.arrived, "runtime caravan should arrive on second day");

    const auto wrong_owner = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "caravan_runtime",
        "traders_guild"
    );
    require(!wrong_owner.ok(), "runtime contract fulfillment should reject wrong owner");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 40, "wrong owner should not debit cargo");
    require(runtime.wallet.coins == 10, "wrong owner should not credit wallet");
    require(runtime.ledger.entries().empty(), "wrong owner should not record ledger");

    const auto fulfilled = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "caravan_runtime",
        "riverwatch"
    );
    require(fulfilled.ok(), "runtime contract fulfillment should succeed for owned arrived caravan");
    require(runtime.caravans.caravans[0].cargo.amount("grain") == 10, "fulfillment should debit caravan cargo");
    require(runtime.wallet.coins == 85, "fulfillment should credit runtime wallet");
    require(runtime.ledger.entries().size() == 1, "fulfillment should record ledger entry");
    require(runtime.ledger.entries()[0].reference_id == "grain_delivery_runtime", "ledger should reference fulfilled contract");
    require(runtime.contracts.contracts[0].status == clc::sim::ContractStatus::fulfilled, "contract should be fulfilled");

    require(!clc::sim::unload_runtime_caravan_at_destination(runtime, "caravan_runtime", "grain", 20).ok(), "runtime unload should reject insufficient cargo");

    const auto delivered = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "caravan_runtime");
    require(delivered.ok(), "runtime delivery should move all remaining arrived cargo to destination");
    require(delivered.caravan_id == "caravan_runtime", "delivery result should expose caravan id");
    require(delivered.destination_settlement_id == "hillford", "delivery result should expose destination settlement id");
    require(delivered.delivered.size() == 1, "delivery result should expose delivered resource entry");
    require(delivered.delivered[0].resource_id == "grain", "delivery result should expose delivered resource id");
    require(delivered.delivered[0].amount == 10, "delivery result should expose delivered amount");
    require(delivered.total_amount == 10, "delivery result should expose total delivered amount");
    require(runtime.caravans.caravans[0].cargo.empty(), "runtime delivery should empty remaining cargo");
    require(runtime.engine.settlement_resource_amount("hillford", "grain") == 10, "runtime delivery should credit destination storage");

    const auto repeated_delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "caravan_runtime");
    require(repeated_delivery.ok(), "empty repeated delivery should be accepted");
    require(repeated_delivery.delivered.empty(), "empty repeated delivery should not report resource entries");
    require(repeated_delivery.total_amount == 0, "empty repeated delivery should report zero total");
    require(runtime.engine.settlement_resource_amount("hillford", "grain") == 10, "empty repeated delivery should not mutate destination storage");

    const auto missing_caravan = clc::sim::advance_runtime_caravan_day(runtime, "missing_caravan");
    require(!missing_caravan.ok(), "runtime caravan advance should reject unknown caravan");

    const auto missing_contract_caravan = clc::sim::fulfill_runtime_contract_from_arrived_caravan_with_reward_and_ledger(
        runtime,
        "grain_delivery_runtime",
        "missing_caravan"
    );
    require(!missing_contract_caravan.ok(), "runtime fulfillment should reject unknown caravan");

    const auto missing_delivery_caravan = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "missing_caravan");
    require(!missing_delivery_caravan.ok(), "runtime delivery should reject unknown caravan");

    return 0;
}
