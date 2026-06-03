#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .base_value = 4,
    }).ok(), "grain resource should add");
    return registry;
}

clc::sim::FactionCatalog make_factions(std::int64_t reputation) {
    clc::sim::FactionCatalog factions;
    require(clc::sim::add_faction(factions, {.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "issuer faction should add");
    require(clc::sim::add_faction(factions, {.id = "traders", .display_name = "Traders"}).ok(), "receiver faction should add");
    require(clc::sim::set_faction_reputation(factions, "riverwatch", "traders", reputation).ok(), "reputation should set");
    return factions;
}

clc::sim::ResourceDeliveryContract make_contract(std::string id, std::uint64_t quantity = 50, std::uint64_t reward = 240) {
    return clc::sim::ResourceDeliveryContract{
        .id = std::move(id),
        .display_name = "Grain Delivery",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders",
        .resource_id = "grain",
        .quantity = quantity,
        .reward_coins = reward,
        .due_ticks = clc::days_to_ticks(5),
    };
}

clc::economy::MarketReport make_market(const clc::data::DataRegistry& registry, const clc::sim::ResourceStorage& storage) {
    clc::economy::MarketState market;
    require(market.set_demand("grain", 100).ok(), "grain demand should set");
    return clc::economy::make_market_report(registry, storage, market);
}

} // namespace

int main() {
    const auto registry = make_registry();
    auto factions = make_factions(40);

    clc::sim::ContractCatalog contracts;
    require(clc::sim::add_contract(contracts, make_contract("deliverable")).ok(), "deliverable contract should add");

    clc::sim::ResourceStorage storage;
    require(storage.add("grain", 80).ok(), "storage should accept grain");
    const auto market = make_market(registry, storage);

    const auto assessment = clc::sim::make_contract_economy_assessment(
        contracts,
        "deliverable",
        factions,
        storage,
        market,
        clc::days_to_ticks(2)
    );
    require(assessment.known_contract, "assessment should know contract");
    require(assessment.open, "assessment should see open contract");
    require(assessment.accepted_by_factions, "trusted reputation should allow contract");
    require(assessment.has_required_resources, "storage should cover quantity");
    require(assessment.deliverable, "assessment should be deliverable");
    require(assessment.market_total_value == assessment.market_unit_price * assessment.quantity, "market value should scale by quantity");
    require(clc::sim::validate_contract_economy_assessment(assessment).ok(), "deliverable assessment should validate");
    require(clc::sim::contract_economy_assessment_digest(assessment).find("deliverable=yes") != std::string::npos, "assessment digest should expose deliverable state");

    const auto storage_plan = clc::sim::plan_contract_flow_from_storage(contracts, "deliverable", storage, "warehouse_a");
    require(storage_plan.can_fulfill, "storage flow should fulfill");
    require(storage_plan.transferable_quantity == 50, "storage flow should transfer required quantity");
    require(clc::sim::contract_resource_flow_plan_digest(storage_plan).find("source=storage") != std::string::npos, "storage digest should identify source");

    auto caravan = clc::sim::create_caravan_for_route(
        clc::sim::SettlementRoute{
            .id = "riverwatch_to_traders",
            .display_name = "Riverwatch to Traders",
            .origin_settlement_id = "riverwatch",
            .destination_settlement_id = "traders",
            .travel_days = 0,
        },
        "caravan_a",
        "Caravan A"
    );
    require(caravan.cargo.add("grain", 60).ok(), "caravan cargo should accept grain");
    clc::sim::OwnershipCatalog ownership;
    require(clc::sim::set_caravan_owner(ownership, "caravan_a", "riverwatch").ok(), "caravan owner should set");

    const auto caravan_plan = clc::sim::plan_contract_flow_from_caravan(
        contracts,
        "deliverable",
        caravan,
        &ownership,
        "riverwatch"
    );
    require(caravan_plan.can_fulfill, "arrived owned caravan should fulfill");
    require(caravan_plan.source_ready, "caravan should be ready");
    require(caravan_plan.owner_matches, "caravan owner should match");

    auto blocked_factions = make_factions(-90);
    const auto blocked = clc::sim::make_contract_economy_assessment(
        contracts,
        "deliverable",
        blocked_factions,
        storage,
        market,
        clc::days_to_ticks(2)
    );
    require(!blocked.deliverable, "blocked reputation should reject delivery");
    require(!blocked.accepted_by_factions, "blocked reputation should not accept factions");
    require(!clc::sim::validate_contract_economy_assessment(blocked).ok(), "blocked assessment should fail validation");

    require(clc::sim::add_contract(contracts, make_contract("missing_resources", 200, 500)).ok(), "missing resource contract should add");
    require(clc::sim::add_contract(contracts, make_contract("overdue", 10, 50)).ok(), "overdue contract should add");

    const auto summary = clc::sim::make_contract_economy_portfolio_summary(
        contracts,
        factions,
        storage,
        market,
        clc::days_to_ticks(6)
    );
    require(summary.contract_count == 3, "portfolio should count contracts");
    require(summary.deliverable_count == 0, "overdue tick should block all open contracts");
    require(summary.overdue_count == 3, "portfolio should count overdue contracts");
    require(summary.blocked_by_resources_count == 1, "portfolio should count resource shortage");
    require(clc::sim::contract_economy_portfolio_summary_digest(summary).find("contracts=3") != std::string::npos, "portfolio digest should count contracts");

    return 0;
}
