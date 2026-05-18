#include "clc/economy/Orders.hpp"

#include <algorithm>
#include <limits>
#include <string>
#include <utility>

namespace clc::economy {
namespace {

bool multiply_overflows(std::uint64_t lhs, std::uint64_t rhs) {
    return lhs != 0 && rhs > std::numeric_limits<std::uint64_t>::max() / lhs;
}

std::uint64_t min_u64(std::uint64_t lhs, std::uint64_t rhs) {
    return lhs < rhs ? lhs : rhs;
}

} // namespace

data::ValidationReport validate_order(const MarketOrder& order, std::string_view path) {
    data::ValidationReport report;
    const auto path_string = std::string{path};

    if (order.resource_id.empty()) {
        report.add_error(path_string, "resource_id must not be empty");
    }
    if (order.quantity == 0) {
        report.add_error(path_string, "quantity must be greater than zero");
    }
    if (order.limit_price == 0) {
        report.add_error(path_string, "limit_price must be greater than zero");
    }
    if (multiply_overflows(order.quantity, order.limit_price)) {
        report.add_error(path_string, "order total value overflows");
    }

    return report;
}

OrderMatchResult match_orders(std::vector<MarketOrder> buy_orders, std::vector<MarketOrder> sell_orders) {
    OrderMatchResult result;

    buy_orders.erase(
        std::remove_if(buy_orders.begin(), buy_orders.end(), [&result](const MarketOrder& order) {
            if (!validate_order(order, "buy_order").ok()) {
                result.warnings.push_back("ignored invalid buy order");
                return true;
            }
            return false;
        }),
        buy_orders.end()
    );

    sell_orders.erase(
        std::remove_if(sell_orders.begin(), sell_orders.end(), [&result](const MarketOrder& order) {
            if (!validate_order(order, "sell_order").ok()) {
                result.warnings.push_back("ignored invalid sell order");
                return true;
            }
            return false;
        }),
        sell_orders.end()
    );

    std::sort(buy_orders.begin(), buy_orders.end(), [](const MarketOrder& lhs, const MarketOrder& rhs) {
        if (lhs.limit_price == rhs.limit_price) {
            return lhs.resource_id < rhs.resource_id;
        }
        return lhs.limit_price > rhs.limit_price;
    });

    std::sort(sell_orders.begin(), sell_orders.end(), [](const MarketOrder& lhs, const MarketOrder& rhs) {
        if (lhs.limit_price == rhs.limit_price) {
            return lhs.resource_id < rhs.resource_id;
        }
        return lhs.limit_price < rhs.limit_price;
    });

    for (auto& buy_order : buy_orders) {
        for (auto& sell_order : sell_orders) {
            if (buy_order.quantity == 0) {
                break;
            }
            if (sell_order.quantity == 0) {
                continue;
            }
            if (buy_order.resource_id != sell_order.resource_id) {
                continue;
            }
            if (buy_order.limit_price < sell_order.limit_price) {
                continue;
            }

            const auto quantity = min_u64(buy_order.quantity, sell_order.quantity);
            const auto unit_price = sell_order.limit_price;
            if (multiply_overflows(quantity, unit_price)) {
                result.warnings.push_back("skipped overflowing trade execution");
                continue;
            }

            const auto total_price = quantity * unit_price;
            result.trades.push_back(TradeExecution{
                .resource_id = buy_order.resource_id,
                .quantity = quantity,
                .unit_price = unit_price,
                .total_price = total_price,
            });

            result.total_quantity += quantity;
            result.total_value += total_price;
            buy_order.quantity -= quantity;
            sell_order.quantity -= quantity;
        }
    }

    return result;
}

} // namespace clc::economy
