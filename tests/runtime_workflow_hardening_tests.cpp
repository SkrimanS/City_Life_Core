#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
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
    require(registry.add(clc::data::ResourceDefinition{.id = "wood", .display_name = "Wood", .category = "construction", .base_value = 6}).ok(), "wood should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "riverwatch", .display_name = "Riverwatch", .starting_population = 120}).ok(), "riverwatch should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "hillford", .display_name = "Hillford", .starting_population = 80}).ok(), "hillford should register");
    return registry;
}

clc::sim::SimulationRuntime make_runtime_with_route() {
    clc::sim::SimulationRuntime runtime{make_registry()};
    require(clc::sim::create_runtime_settlement(runtime, "riverwatch").ok(), "origin settlement should create");
    require(clc::sim::create_runtime_settlement(runtime, "hillford").ok(), "destination settlement should create");
    require(clc::sim::add_runtime_route(runtime, clc::sim::SettlementRoute{
        .id = "riverwatch_to_hillford",
        .display_name = "Riverwatch to Hillford",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "hillford",
        .travel_days = 1,
    }).ok(), "route should add");
    return runtime;
}

} // namespace

int main() {
    const auto max_u64 = std::numeric_limits<std::uint64_t>::max();

    {
        auto runtime = make_runtime_with_route();
        require(runtime.engine.add_resource_to_settlement("riverwatch", "grain", 10).ok(), "origin should receive grain");
        require(runtime.engine.add_resource_to_settlement("hillford", "grain", max_u64 - 5).ok(), "destination should receive near-max grain");

        auto created = clc::sim::create_runtime_caravan_for_route(runtime, "riverwatch_to_hillford", "overflow_unload", "Overflow Unload");
        require(created.ok(), "caravan should create");
        require(clc::sim::load_runtime_caravan_at_origin(runtime, "overflow_unload", "grain", 10).ok(), "caravan should load grain");
        require(clc::sim::advance_runtime_caravan_day(runtime, "overflow_unload").ok(), "caravan should arrive");

        const auto unload = clc::sim::unload_runtime_caravan_at_destination(runtime, "overflow_unload", "grain", 10);
        require(!unload.ok(), "unload should reject destination storage overflow");
        require(runtime.caravans.caravans[0].cargo.amount("grain") == 10, "failed overflow unload should preserve cargo");
        require(runtime.engine.settlement_resource_amount("hillford", "grain") == max_u64 - 5, "failed overflow unload should preserve destination storage");
    }

    {
        auto runtime = make_runtime_with_route();
        clc::sim::ResourceStorage huge_cargo;
        require(huge_cargo.add("grain", max_u64).ok(), "cargo should accept max grain");
        require(huge_cargo.add("wood", 1).ok(), "cargo should accept wood");

        auto created = clc::sim::create_runtime_caravan_for_route(
            runtime,
            "riverwatch_to_hillford",
            "overflow_delivery_total",
            "Overflow Delivery Total",
            huge_cargo
        );
        require(created.ok(), "caravan with huge cargo should create");
        require(clc::sim::advance_runtime_caravan_day(runtime, "overflow_delivery_total").ok(), "huge cargo caravan should arrive");

        const auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "overflow_delivery_total");
        require(!delivery.ok(), "delivery should reject total_amount overflow before mutating runtime");
        require(runtime.caravans.caravans[0].cargo.amount("grain") == max_u64, "failed overflow delivery should preserve grain cargo");
        require(runtime.caravans.caravans[0].cargo.amount("wood") == 1, "failed overflow delivery should preserve wood cargo");
        require(runtime.engine.settlement_resource_amount("hillford", "grain") == 0, "failed overflow delivery should not credit grain destination");
        require(runtime.engine.settlement_resource_amount("hillford", "wood") == 0, "failed overflow delivery should not credit wood destination");
    }

    {
        clc::sim::RuntimeCaravanCargoDeliveryResult invalid_result;
        invalid_result.caravan_id = "synthetic";
        invalid_result.destination_settlement_id = "hillford";
        invalid_result.delivered.push_back(clc::sim::RuntimeCargoDeliveryEntry{.resource_id = "grain", .amount = max_u64});
        invalid_result.delivered.push_back(clc::sim::RuntimeCargoDeliveryEntry{.resource_id = "wood", .amount = 1});
        invalid_result.total_amount = 0;

        const auto validation = clc::sim::validate_runtime_caravan_cargo_delivery_result(invalid_result);
        require(!validation.ok(), "delivery result validation should reject delivered entry sum overflow");
    }

    return 0;
}
