#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::economy {

struct MarketOrder final {
    std::string resource_id{};
    std::uint64_t quantity{0};
    std::uint64_t limit_price{0};
};

struct TradeExecution final {
    std::string resource_id{};
    std::uint64_t quantity{0};
    std::uint64_t unit_price{0};
    std::uint64_t total_price{0};
};

struct OrderMatchResult final {
    std::vector<TradeExecution> trades{};
    std::vector<std::string> warnings{};
    std::uint64_t total_quantity{0};
    std::uint64_t total_value{0};
};

[[nodiscard]] data::ValidationReport validate_order(const MarketOrder& order, std::string_view path);
[[nodiscard]] OrderMatchResult match_orders(std::vector<MarketOrder> buy_orders, std::vector<MarketOrder> sell_orders);

} // namespace clc::economy
