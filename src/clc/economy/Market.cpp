#include "clc/economy/Market.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_set>
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

std::uint64_t absolute_gap(std::uint64_t lhs, std::uint64_t rhs) noexcept {
    return lhs > rhs ? lhs - rhs : rhs - lhs;
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
    prices.reserve(storage.entries().size() + market.demands().size());
    std::unordered_set<std::string> included_resource_ids;
    included_resource_ids.reserve(storage.entries().size() + market.demands().size());

    for (const auto& [resource_id, supply] : storage.entries()) {
        const auto* resource = registry.resource(resource_id);
        if (resource == nullptr) {
            continue;
        }
        prices.push_back(calculate_price(*resource, supply, market.demand(resource_id)));
        included_resource_ids.insert(resource_id);
    }

    for (const auto& [resource_id, demand] : market.demands()) {
        if (included_resource_ids.find(resource_id) != included_resource_ids.end()) {
            continue;
        }

        const auto* resource = registry.resource(resource_id);
        if (resource == nullptr) {
            continue;
        }

        prices.push_back(calculate_price(*resource, 0, demand));
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

const MarketPrice* market_price_by_resource(const MarketReport& report, std::string_view resource_id) noexcept {
    for (const auto& price : report.prices) {
        if (price.resource_id == resource_id) {
            return &price;
        }
    }
    return nullptr;
}

std::uint64_t market_price_or(const MarketReport& report, std::string_view resource_id, std::uint64_t fallback_price) noexcept {
    const auto* price = market_price_by_resource(report, resource_id);
    if (price == nullptr) {
        return fallback_price;
    }
    return price->price;
}

std::string_view market_pressure_name(MarketPressureLevel pressure) noexcept {
    switch (pressure) {
    case MarketPressureLevel::balanced:
        return "balanced";
    case MarketPressureLevel::shortage:
        return "shortage";
    case MarketPressureLevel::surplus:
        return "surplus";
    case MarketPressureLevel::depleted:
        return "depleted";
    }
    return "unknown";
}

MarketPressureLevel classify_market_pressure(std::uint64_t supply, std::uint64_t demand) noexcept {
    if (demand > 0 && supply == 0) {
        return MarketPressureLevel::depleted;
    }
    if (demand > supply) {
        return MarketPressureLevel::shortage;
    }
    if (supply > demand) {
        return MarketPressureLevel::surplus;
    }
    return MarketPressureLevel::balanced;
}

std::uint64_t market_pressure_ratio_basis_points(std::uint64_t supply, std::uint64_t demand) noexcept {
    const auto denominator = std::max<std::uint64_t>(std::max(supply, demand), 1);
    const auto gap = absolute_gap(supply, demand);
    if (gap > std::numeric_limits<std::uint64_t>::max() / 10000ULL) {
        return 10000ULL;
    }
    return std::min<std::uint64_t>((gap * 10000ULL) / denominator, 10000ULL);
}

MarketResourceSignal make_market_resource_signal(const MarketPrice& price) {
    const auto pressure = classify_market_pressure(price.supply, price.demand);
    const auto gap = absolute_gap(price.supply, price.demand);
    std::string reason = price.reason;
    if (pressure == MarketPressureLevel::depleted) {
        reason = "demand exists with no supply";
    }

    return MarketResourceSignal{
        .resource_id = price.resource_id,
        .supply = price.supply,
        .demand = price.demand,
        .unit_price = price.price,
        .base_value = price.base_value,
        .absolute_gap = gap,
        .pressure_basis_points = market_pressure_ratio_basis_points(price.supply, price.demand),
        .pressure = pressure,
        .can_fulfill_demand = price.demand == 0 || price.supply >= price.demand,
        .reason = std::move(reason),
    };
}

MarketSnapshot make_market_snapshot(const MarketReport& report) {
    MarketSnapshot snapshot;
    snapshot.total_supply = report.total_supply;
    snapshot.total_demand = report.total_demand;
    snapshot.signals.reserve(report.prices.size());

    std::uint64_t highest_pressure{};
    for (const auto& price : report.prices) {
        auto signal = make_market_resource_signal(price);
        snapshot.total_pressure_basis_points = saturating_add(snapshot.total_pressure_basis_points, signal.pressure_basis_points);

        switch (signal.pressure) {
        case MarketPressureLevel::balanced:
            ++snapshot.balanced_count;
            break;
        case MarketPressureLevel::shortage:
            ++snapshot.shortage_count;
            break;
        case MarketPressureLevel::surplus:
            ++snapshot.surplus_count;
            break;
        case MarketPressureLevel::depleted:
            ++snapshot.depleted_count;
            break;
        }

        if (signal.pressure_basis_points > highest_pressure) {
            highest_pressure = signal.pressure_basis_points;
            snapshot.highest_pressure_resource_id = signal.resource_id;
        }

        snapshot.signals.push_back(std::move(signal));
    }

    return snapshot;
}

const MarketResourceSignal* market_signal_by_resource(const MarketSnapshot& snapshot, std::string_view resource_id) noexcept {
    for (const auto& signal : snapshot.signals) {
        if (signal.resource_id == resource_id) {
            return &signal;
        }
    }
    return nullptr;
}

std::string market_snapshot_digest(const MarketSnapshot& snapshot) {
    std::ostringstream out;
    out << "market_snapshot"
        << ";resources=" << snapshot.signals.size()
        << ";supply=" << snapshot.total_supply
        << ";demand=" << snapshot.total_demand
        << ";balanced=" << snapshot.balanced_count
        << ";shortage=" << snapshot.shortage_count
        << ";surplus=" << snapshot.surplus_count
        << ";depleted=" << snapshot.depleted_count
        << ";pressure_bp=" << snapshot.total_pressure_basis_points;
    if (!snapshot.highest_pressure_resource_id.empty()) {
        out << ";highest_pressure=" << snapshot.highest_pressure_resource_id;
    }
    return out.str();
}

} // namespace clc::economy
