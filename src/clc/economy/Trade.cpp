#include "clc/economy/Trade.hpp"

#include "clc/economy/Ledger.hpp"

#include <limits>
#include <string>
#include <utility>

namespace clc::economy {
namespace {

bool multiply_overflows(std::uint64_t lhs, std::uint64_t rhs) {
    return lhs != 0 && rhs > std::numeric_limits<std::uint64_t>::max() / lhs;
}

TradeResult failed(std::string resource_id, std::uint64_t quantity, std::uint64_t unit_price, std::string message) {
    return TradeResult{
        .ok = false,
        .resource_id = std::move(resource_id),
        .quantity = quantity,
        .unit_price = unit_price,
        .total_price = 0,
        .message = std::move(message),
    };
}

TradeResult trade_with_ledger(
    Wallet& wallet,
    sim::ResourceStorage& storage,
    const MarketPrice& price,
    std::uint64_t quantity,
    EconomyLedger& ledger,
    LedgerEntryType type,
    std::string note
) {
    auto wallet_copy = wallet;
    auto storage_copy = storage;
    auto ledger_copy = ledger;

    auto result = type == LedgerEntryType::buy
        ? buy_resource(wallet_copy, storage_copy, price, quantity)
        : sell_resource(wallet_copy, storage_copy, price, quantity);
    if (!result.ok) {
        return result;
    }

    if (!ledger_copy.record(type, result, std::move(note))) {
        return failed(result.resource_id, result.quantity, result.unit_price, "failed to record trade ledger entry");
    }

    wallet = wallet_copy;
    storage = std::move(storage_copy);
    ledger = std::move(ledger_copy);
    return result;
}

} // namespace

TradeResult buy_resource(Wallet& wallet, sim::ResourceStorage& storage, const MarketPrice& price, std::uint64_t quantity) {
    if (price.resource_id.empty()) {
        return failed(price.resource_id, quantity, price.price, "resource_id must not be empty");
    }
    if (quantity == 0) {
        return failed(price.resource_id, quantity, price.price, "quantity must be greater than zero");
    }
    if (price.price == 0) {
        return failed(price.resource_id, quantity, price.price, "unit price must be greater than zero");
    }
    if (multiply_overflows(price.price, quantity)) {
        return failed(price.resource_id, quantity, price.price, "total price overflow");
    }

    const auto total_price = price.price * quantity;
    if (wallet.coins < total_price) {
        return failed(price.resource_id, quantity, price.price, "not enough coins");
    }

    const auto add_report = storage.add(price.resource_id, quantity);
    if (!add_report.ok()) {
        return failed(price.resource_id, quantity, price.price, "failed to add purchased resource to storage");
    }

    wallet.coins -= total_price;
    return TradeResult{
        .ok = true,
        .resource_id = price.resource_id,
        .quantity = quantity,
        .unit_price = price.price,
        .total_price = total_price,
        .message = "bought resource",
    };
}

TradeResult sell_resource(Wallet& wallet, sim::ResourceStorage& storage, const MarketPrice& price, std::uint64_t quantity) {
    if (price.resource_id.empty()) {
        return failed(price.resource_id, quantity, price.price, "resource_id must not be empty");
    }
    if (quantity == 0) {
        return failed(price.resource_id, quantity, price.price, "quantity must be greater than zero");
    }
    if (price.price == 0) {
        return failed(price.resource_id, quantity, price.price, "unit price must be greater than zero");
    }
    if (multiply_overflows(price.price, quantity)) {
        return failed(price.resource_id, quantity, price.price, "total price overflow");
    }

    const auto total_price = price.price * quantity;
    if (total_price > std::numeric_limits<std::uint64_t>::max() - wallet.coins) {
        return failed(price.resource_id, quantity, price.price, "wallet overflow");
    }

    if (!storage.try_remove(price.resource_id, quantity)) {
        return failed(price.resource_id, quantity, price.price, "not enough resource in storage");
    }

    wallet.coins += total_price;
    return TradeResult{
        .ok = true,
        .resource_id = price.resource_id,
        .quantity = quantity,
        .unit_price = price.price,
        .total_price = total_price,
        .message = "sold resource",
    };
}

TradeResult buy_resource_with_ledger(
    Wallet& wallet,
    sim::ResourceStorage& storage,
    const MarketPrice& price,
    std::uint64_t quantity,
    EconomyLedger& ledger,
    std::string note
) {
    return trade_with_ledger(wallet, storage, price, quantity, ledger, LedgerEntryType::buy, std::move(note));
}

TradeResult sell_resource_with_ledger(
    Wallet& wallet,
    sim::ResourceStorage& storage,
    const MarketPrice& price,
    std::uint64_t quantity,
    EconomyLedger& ledger,
    std::string note
) {
    return trade_with_ledger(wallet, storage, price, quantity, ledger, LedgerEntryType::sell, std::move(note));
}

} // namespace clc::economy
