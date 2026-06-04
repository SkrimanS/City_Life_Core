#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
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
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .base_value = 4}).ok(), "grain should add");
    require(registry.add(clc::data::ResourceDefinition{.id = "flour", .display_name = "Flour", .base_value = 7}).ok(), "flour should add");
    require(registry.add(clc::data::ResourceDefinition{.id = "bread", .display_name = "Bread", .base_value = 12}).ok(), "bread should add");
    require(registry.add(clc::data::BuildingDefinition{
        .id = "mill",
        .display_name = "Mill",
        .worker_slots = 4,
        .input_resource_ids = {"grain"},
        .output_resource_ids = {"flour"},
    }).ok(), "mill should add");
    require(registry.add(clc::data::BuildingDefinition{
        .id = "bakery",
        .display_name = "Bakery",
        .worker_slots = 3,
        .input_resource_ids = {"flour"},
        .output_resource_ids = {"bread"},
    }).ok(), "bakery should add");
    return registry;
}

clc::economy::MarketReport make_market(const clc::data::DataRegistry& registry, const clc::sim::ResourceStorage& storage) {
    clc::economy::MarketState market;
    require(market.set_demand("bread", 200).ok(), "bread demand should set");
    require(market.set_demand("flour", 20).ok(), "flour demand should set");
    return clc::economy::make_market_report(registry, storage, market);
}

} // namespace

int main() {
    const auto registry = make_registry();

    clc::sim::SettlementState settlement{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .population = 120,
    };
    require(settlement.storage.add("grain", 10).ok(), "grain storage should add");
    require(clc::sim::add_building(settlement, registry, {.definition_id = "mill", .assigned_workers = 2}).ok(), "mill should add");
    require(clc::sim::add_building(settlement, registry, {.definition_id = "bakery", .assigned_workers = 1}).ok(), "bakery should add");
    settlement.buildings.push_back({.definition_id = "lost_workshop", .assigned_workers = 1});

    const auto market = make_market(registry, settlement.storage);
    const auto plan = clc::sim::make_settlement_development_plan(settlement, registry, market, clc::days_to_ticks(3));

    require(plan.settlement_id == "riverwatch", "plan should keep settlement id");
    require(plan.horizon_ticks == clc::days_to_ticks(3), "plan should keep horizon");
    require(plan.building_count == 3, "plan should count buildings");
    require(plan.unknown_building_count == 1, "plan should count unknown building");
    require(plan.idle_worker_slots == 4, "plan should count idle worker slots");
    require(plan.critical_need_count >= 1, "plan should expose critical needs");
    require(plan.high_priority_opportunity_count >= 2, "plan should count high priority opportunities");
    require(clc::sim::validate_settlement_development_plan(plan).ok(), "plan should validate");

    const auto* grain = clc::sim::settlement_resource_need(plan, "grain");
    require(grain != nullptr, "grain need should exist");
    require(grain->estimated_daily_need >= 14, "grain need should include food and mill input");
    require(grain->missing_amount > 0, "grain should be missing over three-day horizon");

    const auto* flour = clc::sim::settlement_resource_need(plan, "flour");
    require(flour != nullptr, "flour need should exist");
    require(flour->missing_amount > 0, "flour input should be missing");

    const auto* import_flour = clc::sim::settlement_production_opportunity(
        plan,
        "bakery",
        clc::sim::SettlementDevelopmentAction::import_input
    );
    require(import_flour != nullptr, "bakery should recommend flour import");
    require(import_flour->resource_id == "flour", "import opportunity should identify flour");
    require(import_flour->priority == clc::sim::SettlementDevelopmentPriority::high, "missing input should be high priority");

    const auto* expand_bakery = clc::sim::settlement_production_opportunity(
        plan,
        "bakery",
        clc::sim::SettlementDevelopmentAction::expand_output
    );
    require(expand_bakery != nullptr, "bakery should recommend output expansion");
    require(expand_bakery->resource_id == "bread", "output opportunity should identify bread");
    require(expand_bakery->priority == clc::sim::SettlementDevelopmentPriority::critical, "depleted output should be critical");

    const auto* unknown = clc::sim::settlement_production_opportunity(
        plan,
        "lost_workshop",
        clc::sim::SettlementDevelopmentAction::register_definition
    );
    require(unknown != nullptr, "unknown building should request definition registration");

    const auto digest = clc::sim::settlement_development_plan_digest(plan);
    require(digest.find("settlement_development") != std::string::npos, "digest should name domain");
    require(digest.find("settlement=riverwatch") != std::string::npos, "digest should include settlement");
    require(digest.find("top_action=") != std::string::npos, "digest should include top action");

    auto invalid = plan;
    invalid.horizon_ticks = 0;
    require(!clc::sim::validate_settlement_development_plan(invalid).ok(), "zero horizon should not validate");

    return 0;
}
