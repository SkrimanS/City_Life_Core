#include "clc/data/DataRegistry.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Storage.hpp"

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
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    }).ok(), "grain resource should register");
    require(registry.add(clc::data::ResourceDefinition{
        .id = "wood",
        .display_name = "Wood",
        .category = "construction",
        .base_value = 6,
    }).ok(), "wood resource should register");

    const auto* grain = registry.resource("grain");
    const auto* wood = registry.resource("wood");
    require(grain != nullptr, "grain should exist");
    require(wood != nullptr, "wood should exist");

    const auto shortage = clc::economy::calculate_price(*grain, 16, 32);
    require(shortage.price > shortage.base_value, "shortage should raise price above base");
    require(shortage.reason == "demand exceeds supply", "shortage should explain reason");

    const auto surplus = clc::economy::calculate_price(*wood, 24, 6);
    require(surplus.price < surplus.base_value, "surplus should lower price below base");
    require(surplus.price >= 1, "surplus price should stay positive");

    const auto balanced = clc::economy::calculate_price(*grain, 10, 10);
    require(balanced.price == balanced.base_value, "balanced market should keep base price");
    require(balanced.reason == "balanced", "balanced market should explain reason");

    clc::economy::MarketState market;
    require(market.set_demand("grain", 32).ok(), "market should accept grain demand");
    require(market.set_demand("wood", 6).ok(), "market should accept wood demand");
    require(market.demand("grain") == 32, "market should report grain demand");
    require(market.set_demand("wood", 0).ok(), "zero demand should clear demand entry");
    require(market.demand("wood") == 0, "cleared demand should be zero");
    require(market.set_demand("wood", 6).ok(), "market should restore wood demand");

    clc::sim::ResourceStorage storage;
    require(storage.add("grain", 16).ok(), "storage should accept grain supply");
    require(storage.add("wood", 24).ok(), "storage should accept wood supply");

    const auto prices = clc::economy::calculate_market_prices(registry, storage, market);
    require(prices.size() == 2, "market should price registered resources present in storage");
    require(prices[0].resource_id == "grain", "market prices should be sorted by resource id");
    require(prices[1].resource_id == "wood", "market prices should include wood");

    const auto report = clc::economy::make_market_report(registry, storage, market);
    require(report.prices.size() == 2, "market report should include price rows");
    require(report.total_supply == 40, "market report should aggregate total supply");
    require(report.total_demand == 38, "market report should aggregate total demand");
    require(report.min_price >= 1, "market report min price should stay positive");
    require(report.max_price >= report.min_price, "market report max price should be >= min price");
    require(report.average_price >= report.min_price, "market report average price should be >= min price");
    require(report.average_price <= report.max_price, "market report average price should be <= max price");

    clc::sim::ResourceStorage empty_storage;
    const auto empty_report = clc::economy::make_market_report(registry, empty_storage, market);
    require(empty_report.prices.empty(), "empty market report should have no price rows");
    require(empty_report.total_supply == 0, "empty market report should have zero supply");
    require(empty_report.total_demand == 0, "empty market report should have zero demand");
    require(empty_report.average_price == 0, "empty market report should have zero average price");
    require(empty_report.min_price == 0, "empty market report should have zero min price");
    require(empty_report.max_price == 0, "empty market report should have zero max price");

    return 0;
}
