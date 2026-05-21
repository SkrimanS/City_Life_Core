#include "clc/sim/Contracts.hpp"

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

clc::sim::ResourceDeliveryContract make_tick_contract() {
    return clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery_3h",
        .display_name = "Grain Delivery 3h",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 10,
        .reward_coins = 25,
        .due_ticks = clc::hours_to_ticks(3),
    };
}

} // namespace

int main() {
    const auto tick_contract = make_tick_contract();
    require(clc::sim::validate_resource_delivery_contract(tick_contract).ok(), "tick-only contract should validate");
    require(clc::sim::contract_due_ticks(tick_contract) == clc::hours_to_ticks(3), "tick-only contract should expose due ticks");

    auto conflicting_contract = tick_contract;
    conflicting_contract.due_day = 1;
    require(!clc::sim::validate_resource_delivery_contract(conflicting_contract).ok(), "contract should reject conflicting day/tick deadlines");

    auto day_contract = tick_contract;
    day_contract.id = "grain_delivery_day";
    day_contract.display_name = "Grain Delivery Day";
    day_contract.due_ticks = 0;
    day_contract.due_day = 2;
    require(clc::sim::validate_resource_delivery_contract(day_contract).ok(), "day contract should still validate");
    require(clc::sim::contract_due_ticks(day_contract) == clc::days_to_ticks(2), "day contract should convert due day to due ticks");

    clc::sim::ContractCatalog catalog;
    require(clc::sim::add_contract(catalog, tick_contract).ok(), "tick contract should add");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_3h")->due_ticks == clc::hours_to_ticks(3), "tick contract should retain due ticks");

    require(clc::sim::overdue_open_contracts_at_tick(catalog, clc::hours_to_ticks(3)).empty(), "tick contract should not be overdue exactly on deadline");
    require(clc::sim::overdue_open_contracts_at_tick(catalog, clc::hours_to_ticks(3) + 1).size() == 1, "tick contract should be overdue after deadline tick");

    auto boundary_report = clc::sim::fail_overdue_open_contracts_at_tick(catalog, clc::hours_to_ticks(3));
    require(boundary_report.failed_count == 0, "tick deadline sweep should not fail on boundary");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_3h")->status == clc::sim::ContractStatus::open, "tick boundary should keep contract open");

    auto overdue_report = clc::sim::fail_overdue_open_contracts_at_tick(catalog, clc::hours_to_ticks(3) + 1);
    require(overdue_report.current_tick == clc::hours_to_ticks(3) + 1, "tick deadline report should expose current tick");
    require(overdue_report.failed_count == 1, "tick deadline sweep should fail overdue contract");
    require(overdue_report.failed_contract_ids.size() == 1, "tick deadline sweep should report failed contract id");
    require(overdue_report.failed_contract_ids[0] == "grain_delivery_3h", "tick deadline sweep should report expected contract id");
    require(clc::sim::contract_by_id(catalog, "grain_delivery_3h")->status == clc::sim::ContractStatus::failed, "tick deadline sweep should mark contract failed");

    clc::sim::ContractCatalog day_catalog;
    require(clc::sim::add_contract(day_catalog, day_contract).ok(), "day contract should add and fill due ticks");
    require(clc::sim::contract_by_id(day_catalog, "grain_delivery_day")->due_ticks == clc::days_to_ticks(2), "day contract should store converted due ticks");
    require(clc::sim::fail_overdue_open_contracts(day_catalog, 2).failed_count == 0, "day wrapper should not fail on due day");
    require(clc::sim::fail_overdue_open_contracts(day_catalog, 3).failed_count == 1, "day wrapper should still fail after due day");

    return 0;
}
