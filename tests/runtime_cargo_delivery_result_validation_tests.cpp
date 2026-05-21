#include "clc/sim/SimulationRuntimeScenario.hpp"
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
    clc::sim::RuntimeCaravanCargoDeliveryResult single{};
    single.caravan_id = "caravan_a";
    single.destination_settlement_id = "hillford";
    single.delivered.push_back(clc::sim::RuntimeCargoDeliveryEntry{.resource_id = "grain", .amount = 15});
    single.delivered.push_back(clc::sim::RuntimeCargoDeliveryEntry{.resource_id = "wood", .amount = 5});
    single.total_amount = 20;

    require(clc::sim::validate_runtime_caravan_cargo_delivery_result(single).ok(), "valid single cargo delivery result should validate");

    auto bad_single_total = single;
    bad_single_total.total_amount = 21;
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result(bad_single_total).ok(), "single validator should reject total mismatch");

    auto bad_single_caravan = single;
    bad_single_caravan.caravan_id.clear();
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result(bad_single_caravan).ok(), "single validator should reject empty caravan id");

    auto bad_single_destination = single;
    bad_single_destination.destination_settlement_id.clear();
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result(bad_single_destination).ok(), "single validator should reject empty destination id");

    auto bad_single_resource = single;
    bad_single_resource.delivered[0].resource_id.clear();
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result(bad_single_resource).ok(), "single validator should reject empty resource id");

    auto bad_single_amount = single;
    bad_single_amount.delivered[0].amount = 0;
    bad_single_amount.total_amount = 5;
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result(bad_single_amount).ok(), "single validator should reject zero delivered amount");

    clc::sim::RuntimeCaravanCargoDeliveryResult empty_single{};
    empty_single.caravan_id = "caravan_empty";
    empty_single.destination_settlement_id = "hillford";
    require(clc::sim::validate_runtime_caravan_cargo_delivery_result(empty_single).ok(), "empty successful single delivery result should validate with zero total");

    auto bad_empty_single = empty_single;
    bad_empty_single.total_amount = 1;
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result(bad_empty_single).ok(), "single validator should reject empty result with non-zero total");

    clc::sim::RuntimeBulkCargoDeliveryResult bulk{};
    bulk.deliveries.push_back(single);
    clc::sim::RuntimeCaravanCargoDeliveryResult second{};
    second.caravan_id = "caravan_b";
    second.destination_settlement_id = "hillford";
    second.delivered.push_back(clc::sim::RuntimeCargoDeliveryEntry{.resource_id = "stone", .amount = 7});
    second.total_amount = 7;
    bulk.deliveries.push_back(second);
    bulk.delivered_caravans = 2;
    bulk.total_amount = 27;

    require(clc::sim::validate_runtime_bulk_cargo_delivery_result(bulk).ok(), "valid bulk cargo delivery result should validate");

    auto bad_bulk_total = bulk;
    bad_bulk_total.total_amount = 28;
    require(!clc::sim::validate_runtime_bulk_cargo_delivery_result(bad_bulk_total).ok(), "bulk validator should reject total mismatch");

    auto bad_bulk_count = bulk;
    bad_bulk_count.delivered_caravans = 3;
    require(!clc::sim::validate_runtime_bulk_cargo_delivery_result(bad_bulk_count).ok(), "bulk validator should reject delivered caravan count mismatch");

    auto bad_bulk_nested = bulk;
    bad_bulk_nested.deliveries[1].total_amount = 8;
    bad_bulk_nested.total_amount = 28;
    require(!clc::sim::validate_runtime_bulk_cargo_delivery_result(bad_bulk_nested).ok(), "bulk validator should reject invalid nested delivery result");

    clc::sim::RuntimeBulkCargoDeliveryResult empty_bulk{};
    require(clc::sim::validate_runtime_bulk_cargo_delivery_result(empty_bulk).ok(), "empty successful bulk delivery result should validate");

    auto bad_empty_bulk_total = empty_bulk;
    bad_empty_bulk_total.total_amount = 1;
    require(!clc::sim::validate_runtime_bulk_cargo_delivery_result(bad_empty_bulk_total).ok(), "bulk validator should reject empty result with non-zero total");

    auto bad_empty_bulk_count = empty_bulk;
    bad_empty_bulk_count.delivered_caravans = 1;
    require(!clc::sim::validate_runtime_bulk_cargo_delivery_result(bad_empty_bulk_count).ok(), "bulk validator should reject empty result with non-zero delivered count");

    clc::sim::RuntimeCaravanCargoDeliveryResult failed_single{};
    failed_single.validation.add_error("simulation.caravan.failed", "synthetic failure");
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result(failed_single).ok(), "single validator should propagate failed result validation");

    clc::sim::RuntimeBulkCargoDeliveryResult failed_bulk{};
    failed_bulk.validation.add_error("simulation.bulk.failed", "synthetic failure");
    require(!clc::sim::validate_runtime_bulk_cargo_delivery_result(failed_bulk).ok(), "bulk validator should propagate failed result validation");

    auto runtime_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(runtime_bootstrap.ok(), "runtime-aware validator bootstrap should succeed");
    auto& runtime = runtime_bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "runtime_validator_caravan",
        "Runtime Validator Caravan"
    );
    require(created.ok(), "runtime-aware validator caravan should create");
    require(clc::sim::load_runtime_caravan_at_origin(runtime, "runtime_validator_caravan", "grain", 15).ok(), "runtime-aware validator caravan should load");
    require(clc::sim::advance_runtime_caravan_day(runtime, "runtime_validator_caravan").ok(), "runtime-aware validator caravan first advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(runtime, "runtime_validator_caravan").ok(), "runtime-aware validator caravan second advance should succeed");

    const auto runtime_delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, "runtime_validator_caravan");
    require(runtime_delivery.ok(), "runtime-aware validator delivery should succeed");
    require(clc::sim::validate_runtime_caravan_cargo_delivery_result_for_runtime(runtime, runtime_delivery).ok(), "runtime-aware single validator should accept delivered runtime state");

    clc::sim::RuntimeBulkCargoDeliveryResult runtime_bulk{};
    runtime_bulk.deliveries.push_back(runtime_delivery);
    runtime_bulk.delivered_caravans = 1;
    runtime_bulk.total_amount = runtime_delivery.total_amount;
    require(clc::sim::validate_runtime_bulk_cargo_delivery_result_for_runtime(runtime, runtime_bulk).ok(), "runtime-aware bulk validator should accept delivered runtime state");

    auto missing_caravan_result = runtime_delivery;
    missing_caravan_result.caravan_id = "missing_caravan";
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result_for_runtime(runtime, missing_caravan_result).ok(), "runtime-aware single validator should reject missing caravan");

    auto wrong_destination_result = runtime_delivery;
    wrong_destination_result.destination_settlement_id = "riverwatch";
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result_for_runtime(runtime, wrong_destination_result).ok(), "runtime-aware single validator should reject wrong destination");

    auto low_destination_storage_runtime = runtime;
    require(low_destination_storage_runtime.engine.remove_resource_from_settlement("hillford", "grain", 15).ok(), "runtime-aware validator destination drift setup should succeed");
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result_for_runtime(low_destination_storage_runtime, runtime_delivery).ok(), "runtime-aware single validator should reject destination storage drift");

    auto cargo_drift_runtime = runtime;
    require(cargo_drift_runtime.caravans.caravans.back().cargo.add("grain", 1).ok(), "runtime-aware validator cargo drift setup should succeed");
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result_for_runtime(cargo_drift_runtime, runtime_delivery).ok(), "runtime-aware single validator should reject non-empty delivered cargo");

    auto not_arrived_runtime = runtime;
    not_arrived_runtime.caravans.caravans.back().days_remaining = 1;
    require(!clc::sim::validate_runtime_caravan_cargo_delivery_result_for_runtime(not_arrived_runtime, runtime_delivery).ok(), "runtime-aware single validator should reject not-arrived caravan state");

    auto bad_runtime_bulk = runtime_bulk;
    bad_runtime_bulk.deliveries[0].destination_settlement_id = "riverwatch";
    require(!clc::sim::validate_runtime_bulk_cargo_delivery_result_for_runtime(runtime, bad_runtime_bulk).ok(), "runtime-aware bulk validator should reject invalid nested runtime delivery");

    auto aggregate_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(aggregate_bootstrap.ok(), "aggregate runtime-aware validator bootstrap should succeed");
    auto& aggregate_runtime = aggregate_bootstrap.runtime;
    require(aggregate_runtime.engine.add_resource_to_settlement("riverwatch", "grain", 50).ok(), "aggregate runtime-aware validator origin top-up should succeed");

    auto aggregate_a = clc::sim::create_runtime_caravan_for_route(
        aggregate_runtime,
        "riverwatch_to_hillford",
        "aggregate_validator_a",
        "Aggregate Validator A"
    );
    require(aggregate_a.ok(), "aggregate validator caravan a should create");
    auto aggregate_b = clc::sim::create_runtime_caravan_for_route(
        aggregate_runtime,
        "riverwatch_to_hillford",
        "aggregate_validator_b",
        "Aggregate Validator B"
    );
    require(aggregate_b.ok(), "aggregate validator caravan b should create");
    require(clc::sim::load_runtime_caravan_at_origin(aggregate_runtime, "aggregate_validator_a", "grain", 15).ok(), "aggregate validator caravan a should load");
    require(clc::sim::load_runtime_caravan_at_origin(aggregate_runtime, "aggregate_validator_b", "grain", 20).ok(), "aggregate validator caravan b should load");
    require(clc::sim::advance_runtime_caravan_day(aggregate_runtime, "aggregate_validator_a").ok(), "aggregate validator caravan a first advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(aggregate_runtime, "aggregate_validator_a").ok(), "aggregate validator caravan a second advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(aggregate_runtime, "aggregate_validator_b").ok(), "aggregate validator caravan b first advance should succeed");
    require(clc::sim::advance_runtime_caravan_day(aggregate_runtime, "aggregate_validator_b").ok(), "aggregate validator caravan b second advance should succeed");

    const auto aggregate_delivery = clc::sim::deliver_all_runtime_arrived_caravan_cargo_to_destinations(aggregate_runtime);
    require(aggregate_delivery.ok(), "aggregate validator bulk delivery should succeed");
    require(clc::sim::validate_runtime_bulk_cargo_delivery_result_for_runtime(aggregate_runtime, aggregate_delivery).ok(), "aggregate validator should accept valid delivered runtime state");

    auto aggregate_drift_runtime = aggregate_runtime;
    require(aggregate_drift_runtime.engine.remove_resource_from_settlement("hillford", "grain", 20).ok(), "aggregate destination drift setup should succeed");
    require(!clc::sim::validate_runtime_bulk_cargo_delivery_result_for_runtime(aggregate_drift_runtime, aggregate_delivery).ok(), "runtime-aware bulk validator should reject aggregate destination storage drift");

    return 0;
}
