#include "clc/data/DataRegistry.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Storage.hpp"

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

} // namespace

int main() {
    constexpr auto max_u64 = std::numeric_limits<std::uint64_t>::max();

    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{
        .id = "scarce",
        .display_name = "Scarce Resource",
        .category = "test",
        .base_value = max_u64,
    }).ok(), "max-value resource should register");
    require(registry.add(clc::data::ResourceDefinition{
        .id = "free_base",
        .display_name = "Free Base Resource",
        .category = "test",
        .base_value = 0,
    }).ok(), "zero-base resource should register");
    require(registry.add(clc::data::ResourceDefinition{
        .id = "bulk",
        .display_name = "Bulk Resource",
        .category = "test",
        .base_value = 2,
    }).ok(), "bulk resource should register");

    const auto* scarce = registry.resource("scarce");
    const auto* free_base = registry.resource("free_base");
    require(scarce != nullptr, "scarce resource should exist");
    require(free_base != nullptr, "free_base resource should exist");

    const auto saturated_shortage = clc::economy::calculate_price(*scarce, 0, 2);
    require(saturated_shortage.price == max_u64, "shortage premium should saturate instead of overflowing");
    require(saturated_shortage.reason == "demand exceeds supply", "saturated shortage should preserve reason");

    const auto zero_base_balanced = clc::economy::calculate_price(*free_base, 10, 10);
    require(zero_base_balanced.base_value == 1, "zero base values should normalize to one");
    require(zero_base_balanced.price == 1, "zero-base balanced price should stay positive");

    const auto zero_base_surplus = clc::economy::calculate_price(*free_base, max_u64, 0);
    require(zero_base_surplus.price == 1, "extreme surplus should not produce a zero price");
    require(zero_base_surplus.reason == "supply exceeds demand", "extreme surplus should preserve reason");

    clc::sim::ResourceStorage storage;
    require(storage.add("scarce", max_u64).ok(), "storage should accept max scarce supply");
    require(storage.add("bulk", 1).ok(), "storage should accept bulk supply");

    clc::economy::MarketState market;
    require(market.set_demand("scarce", max_u64).ok(), "market should accept max scarce demand");
    require(market.set_demand("bulk", 1).ok(), "market should accept bulk demand");
    require(market.set_demand("unknown", max_u64).ok(), "market may store unknown-resource demand");

    const auto report = clc::economy::make_market_report(registry, storage, market);
    require(report.prices.size() == 2, "report should ignore unknown-resource demand rows");
    require(report.total_supply == max_u64, "report total supply should saturate on overflow");
    require(report.total_demand == max_u64, "report total demand should saturate on overflow");
    require(report.min_price >= 1, "report min price should stay positive");
    require(report.max_price == max_u64, "report should preserve saturated max price");
    require(report.average_price >= report.min_price, "report average should stay within reported bounds");
    require(report.average_price <= report.max_price, "report average should stay within reported bounds");

    const auto* missing_price = clc::economy::market_price_by_resource(report, "unknown");
    require(missing_price == nullptr, "unknown-resource demand should not be priced without registry definition");
    require(clc::economy::market_price_or(report, "unknown", 77) == 77, "unknown-resource lookup should return fallback");

    return 0;
}
