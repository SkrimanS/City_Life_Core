#pragma once

#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/Storage.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace clc::economy {

struct MarketPrice final {
    std::string resource_id{};
    std::uint64_t base_value{0};
    std::uint64_t supply{0};
    std::uint64_t demand{0};
    std::uint64_t price{0};
    std::string reason{};
};

struct MarketReport final {
    std::vector<MarketPrice> prices{};
    std::uint64_t total_supply{0};
    std::uint64_t total_demand{0};
    std::uint64_t average_price{0};
    std::uint64_t min_price{0};
    std::uint64_t max_price{0};
};

class MarketState final {
public:
    [[nodiscard]] data::ValidationReport set_demand(std::string resource_id, std::uint64_t demand);

    [[nodiscard]] std::uint64_t demand(std::string_view resource_id) const;
    [[nodiscard]] const std::unordered_map<std::string, std::uint64_t>& demands() const noexcept;

private:
    std::unordered_map<std::string, std::uint64_t> demands_{};
};

[[nodiscard]] MarketPrice calculate_price(
    const data::ResourceDefinition& resource,
    std::uint64_t supply,
    std::uint64_t demand
);

[[nodiscard]] std::vector<MarketPrice> calculate_market_prices(
    const data::DataRegistry& registry,
    const sim::ResourceStorage& storage,
    const MarketState& market
);

[[nodiscard]] MarketReport make_market_report(
    const data::DataRegistry& registry,
    const sim::ResourceStorage& storage,
    const MarketState& market
);

[[nodiscard]] const MarketPrice* market_price_by_resource(
    const MarketReport& report,
    std::string_view resource_id
) noexcept;

[[nodiscard]] std::uint64_t market_price_or(
    const MarketReport& report,
    std::string_view resource_id,
    std::uint64_t fallback_price = 0
) noexcept;

} // namespace clc::economy
