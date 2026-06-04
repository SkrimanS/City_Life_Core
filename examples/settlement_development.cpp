#include "clc/CityLifeCore.hpp"

#include <iostream>

namespace {

bool ok(const clc::data::ValidationReport& report) {
    if (report.ok()) {
        return true;
    }

    for (const auto& message : report.messages()) {
        std::cerr << message.path << ": " << message.message << '\n';
    }
    return false;
}

} // namespace

int main() {
    clc::data::DataRegistry registry;
    if (!ok(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .base_value = 4}))
        || !ok(registry.add(clc::data::ResourceDefinition{.id = "flour", .display_name = "Flour", .base_value = 7}))
        || !ok(registry.add(clc::data::ResourceDefinition{.id = "bread", .display_name = "Bread", .base_value = 12}))) {
        return 1;
    }

    if (!ok(registry.add(clc::data::BuildingDefinition{
        .id = "mill",
        .display_name = "Mill",
        .worker_slots = 4,
        .input_resource_ids = {"grain"},
        .output_resource_ids = {"flour"},
    }))
        || !ok(registry.add(clc::data::BuildingDefinition{
        .id = "bakery",
        .display_name = "Bakery",
        .worker_slots = 3,
        .input_resource_ids = {"flour"},
        .output_resource_ids = {"bread"},
    }))) {
        return 1;
    }

    clc::sim::SettlementState settlement{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .population = 120,
    };
    if (!ok(settlement.storage.add("grain", 10))
        || !ok(clc::sim::add_building(settlement, registry, {.definition_id = "mill", .assigned_workers = 2}))
        || !ok(clc::sim::add_building(settlement, registry, {.definition_id = "bakery", .assigned_workers = 1}))) {
        return 1;
    }

    clc::economy::MarketState market;
    if (!ok(market.set_demand("flour", 20)) || !ok(market.set_demand("bread", 200))) {
        return 1;
    }
    const auto market_report = clc::economy::make_market_report(registry, settlement.storage, market);

    const auto plan = clc::sim::make_settlement_development_plan(
        settlement,
        registry,
        market_report,
        clc::days_to_ticks(3)
    );

    std::cout << clc::sim::settlement_development_plan_digest(plan) << '\n';

    for (const auto& need : plan.resource_needs) {
        std::cout << "need,"
                  << need.resource_id
                  << ",missing=" << need.missing_amount
                  << ",priority=" << clc::sim::settlement_development_priority_name(need.priority)
                  << '\n';
    }

    for (const auto& opportunity : plan.opportunities) {
        std::cout << "opportunity,"
                  << opportunity.building_definition_id
                  << ",action=" << clc::sim::settlement_development_action_name(opportunity.action)
                  << ",priority=" << clc::sim::settlement_development_priority_name(opportunity.priority)
                  << ",resource=" << opportunity.resource_id
                  << '\n';
    }

    return clc::sim::validate_settlement_development_plan(plan).ok() ? 0 : 1;
}
