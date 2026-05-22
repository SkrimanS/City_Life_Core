#pragma once

#include "clc/data/Validation.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Storage.hpp"

#include <cstdint>
#include <string>

namespace clc::economy {

class EconomyLedger;

struct Wallet final {
    std::uint64_t coins{0};
};

struct TradeResult final {
    bool ok{false};
    std::string resource_id{};
    std::uint64_t quantity{0};
    std::uint64_t unit_price{0};
    std::uint64_t total_price{0};
    std::string message{};
};

[[nodiscard]] TradeResult buy_resource(
    Wallet& wallet,
    sim::ResourceStorage& storage,
    const MarketPrice& price,
    std::uint64_t quantity
);

[[nodiscard]] TradeResult sell_resource(
    Wallet& wallet,
    sim::ResourceStorage& storage,
    const MarketPrice& price,
    std::uint64_t quantity
);

[[nodiscard]] TradeResult buy_resource_with_ledger(
    Wallet& wallet,
    sim::ResourceStorage& storage,
    const MarketPrice& price,
    std::uint64_t quantity,
    EconomyLedger& ledger,
    std::string note = {}
);

[[nodiscard]] TradeResult sell_resource_with_ledger(
    Wallet& wallet,
    sim::ResourceStorage& storage,
    const MarketPrice& price,
    std::uint64_t quantity,
    EconomyLedger& ledger,
    std::string note = {}
);

} // namespace clc::economy
