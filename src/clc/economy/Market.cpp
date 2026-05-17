#include "clc/economy/Market.hpp"

#include <algorithm>
#include <limits>
#include <utility>

namespace clc::economy {
namespace {

std::uint64_t saturating_add(std::uint64_t lhs, std::uint64_t rhs) {
    if (rhs > std::numeric_limits<std::uint64_t>::max() - lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs + rhs;
}

std::uint64_t saturating_multiply(std::uint64_t lhs, std::uint64_t rhs) {
    if (lhs != 0 && rhs > std::numeric_limits<std::uint64_t>::max() / lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs * rhs;
}

} // namespace

data::ValidationReport MarketState::set_demand(std::string resource_id, std::uint64_t demand_value) {
    data::ValidationReport report;
    if (resource_id.empty()) {
        report.add_error("market", "resource_id must not be empty");
        return report;
    }

    if (demand_value == 0) {
        demands_.erase(resource_id);
        return report;
    }

    demands_[std::move(resource_id)] = demand_value;
    return report;
}

std::uint64_t MarketState::demand(std::string_view resource_id) const {
    const auto it = demands_.find(std::string{resource_id});
    if (it == demands_.end()) {
        return 0;
    }
    return it->second;
}

const std::unordered_map<std::string, std::uint64_t>& MarketState::demands() const noexcept {
    return demands_;
}

MarketPrice calculate_price(const data::ResourceDefinition& resource, std::uint64_t supply, std::uint64_t demand) {
    const auto base_value = std::max<std::uint64_t>(resource.base_value, 1);
    auto price = base_value;
    std::string reason = "balanced";

    if (demand > supply) {
        const auto shortage = demand - supply;
        const auto premium = saturating_multiply(base_value, shortage) / std::max<std::uint64_t>(demand, 1);
        price = saturating_add(base_value, std::max<std::uint64_t>(premium, 1));
        reason = "demand exceeds supply";
    } else if (supply > demand && supply > 0) {
        const auto surplus = supply - demand;
        const auto discount = saturating_multiply(base_value, surplus) / supply;
        price = discount >= base_value ? 1 : std::max<std::uint64_t>(base_value - discount, 1);
        reason = "supply exceeds demand";
    }

    return MarketPrice{
        .resource_id = resource.id,
        .base_value = base_value,
        .supply = supply,
        .demand = demand,
        .price = price,
        .reason = std::move(reason),
    };
}

std::vector<MarketPrice> calculate_market_prices(const data::DataRegistry& registry, const sim::ResourceStorage& storage, const MarketState& market) {
    std::vector<MarketPrice> prices;
    prices.reserve(storage.entries().size());

    for (const auto& [resource_id, supply] : storage.entries()) {
        const auto* resource = registry.resource(resource_id);
        if (resource == nullptr) {
            continue;
        }
        prices.push_back(calculate_price(*resource, supply, market.demand(resource_id)));
    }

    std::sort(prices.begin(), prices.end(), [](const MarketPrice& lhs, const MarketPrice& rhs) {
        return lhs.resource_id < rhs.resource_id;
    });

    return prices;
}

MarketReport make_market_report(const data::DataRegistry& registry, const sim::ResourceStorage& storage, const MarketState& market) {
    MarketReport report;
    report.prices = calculate_market_prices(registry, storage, market);

    if (report.prices.empty()) {
        return report;
    }

    std::uint64_t total_price{};
    report.min_price = std::numeric_limits<std::uint64_t>::max();

    for (const auto& price : report.prices) {
        report.total_supply = saturating_add(report.total_supply, price.supply);
        report.total_demand = saturating_add(report.total_demand, price.demand);
        total_price = saturating_add(total_price, price.price);
        report.min_price = std::min(report.min_price, price.price);
        report.max_price = std::max(report.max_price, price.price);
    }

    report.average_price = total_price / static_cast<std::uint64_t>(report.prices.size());
    return report;
}

} // namespace clc::economy
