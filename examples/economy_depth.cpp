#include "clc/CityLifeCore.hpp"

#include <iostream>
#include <string>

int main() {
    clc::data::DataRegistry registry;
    if (!registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .base_value = 4,
    }).ok()) {
        return 1;
    }

    clc::sim::ResourceStorage warehouse;
    if (!warehouse.add("grain", 80).ok()) {
        return 1;
    }

    clc::economy::MarketState market;
    if (!market.set_demand("grain", 120).ok()) {
        return 1;
    }
    const auto market_report = clc::economy::make_market_report(registry, warehouse, market);

    clc::sim::FactionCatalog factions;
    if (!clc::sim::add_faction(factions, {.id = "riverwatch", .display_name = "Riverwatch"}).ok()) {
        return 1;
    }
    if (!clc::sim::add_faction(factions, {.id = "traders", .display_name = "Traders Guild"}).ok()) {
        return 1;
    }
    if (!clc::sim::set_faction_reputation(factions, "riverwatch", "traders", 35).ok()) {
        return 1;
    }

    clc::sim::ContractCatalog contracts;
    if (!clc::sim::add_contract(contracts, clc::sim::ResourceDeliveryContract{
        .id = "grain_contract",
        .display_name = "Grain Delivery",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders",
        .resource_id = "grain",
        .quantity = 50,
        .reward_coins = 260,
        .due_ticks = clc::days_to_ticks(5),
    }).ok()) {
        return 1;
    }

    const auto assessment = clc::sim::make_contract_economy_assessment(
        contracts,
        "grain_contract",
        factions,
        warehouse,
        market_report,
        clc::days_to_ticks(2)
    );

    const auto flow = clc::sim::plan_contract_flow_from_storage(
        contracts,
        "grain_contract",
        warehouse,
        "warehouse_riverwatch"
    );

    const auto portfolio = clc::sim::make_contract_economy_portfolio_summary(
        contracts,
        factions,
        warehouse,
        market_report,
        clc::days_to_ticks(2)
    );

    std::cout << clc::economy::market_snapshot_digest(clc::economy::make_market_snapshot(market_report)) << '\n';
    std::cout << clc::sim::contract_economy_assessment_digest(assessment) << '\n';
    std::cout << clc::sim::contract_resource_flow_plan_digest(flow) << '\n';
    std::cout << clc::sim::contract_economy_portfolio_summary_digest(portfolio) << '\n';

    return 0;
}
