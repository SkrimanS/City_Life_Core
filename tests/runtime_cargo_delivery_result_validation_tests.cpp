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

    return 0;
}
