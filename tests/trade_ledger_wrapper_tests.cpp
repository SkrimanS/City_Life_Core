#include "clc/economy/Ledger.hpp"
#include "clc/economy/Trade.hpp"
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

clc::economy::MarketPrice grain_price(std::uint64_t price = 10) {
    return clc::economy::MarketPrice{
        .resource_id = "grain",
        .base_value = 10,
        .supply = 0,
        .demand = 0,
        .price = price,
        .reason = "test",
    };
}

} // namespace

int main() {
    {
        clc::economy::Wallet wallet{.coins = 100};
        clc::sim::ResourceStorage storage;
        clc::economy::EconomyLedger ledger;

        const auto result = clc::economy::buy_resource_with_ledger(wallet, storage, grain_price(), 3, ledger, "sdk buy");
        require(result.ok, "buy with ledger should succeed");
        require(wallet.coins == 70, "buy with ledger should debit wallet");
        require(storage.amount("grain") == 3, "buy with ledger should credit storage");
        require(ledger.entries().size() == 1, "buy with ledger should record ledger entry");
        require(ledger.entries()[0].type == clc::economy::LedgerEntryType::buy, "ledger entry should be buy");
        require(ledger.entries()[0].note == "sdk buy", "ledger entry should preserve note");
    }

    {
        clc::economy::Wallet wallet{.coins = 0};
        clc::sim::ResourceStorage storage;
        require(storage.add("grain", 5).ok(), "storage should receive grain");
        clc::economy::EconomyLedger ledger;

        const auto result = clc::economy::sell_resource_with_ledger(wallet, storage, grain_price(), 2, ledger, "sdk sell");
        require(result.ok, "sell with ledger should succeed");
        require(wallet.coins == 20, "sell with ledger should credit wallet");
        require(storage.amount("grain") == 3, "sell with ledger should debit storage");
        require(ledger.entries().size() == 1, "sell with ledger should record ledger entry");
        require(ledger.entries()[0].type == clc::economy::LedgerEntryType::sell, "ledger entry should be sell");
        require(ledger.entries()[0].note == "sdk sell", "ledger entry should preserve note");
    }

    {
        clc::economy::Wallet wallet{.coins = 5};
        clc::sim::ResourceStorage storage;
        clc::economy::EconomyLedger ledger;

        const auto result = clc::economy::buy_resource_with_ledger(wallet, storage, grain_price(), 1, ledger, "failed buy");
        require(!result.ok, "failed buy with ledger should fail");
        require(wallet.coins == 5, "failed buy with ledger should preserve wallet");
        require(storage.amount("grain") == 0, "failed buy with ledger should preserve storage");
        require(ledger.entries().empty(), "failed buy with ledger should not record ledger entry");
    }

    {
        clc::economy::Wallet wallet{.coins = std::numeric_limits<std::uint64_t>::max() - 5};
        clc::sim::ResourceStorage storage;
        require(storage.add("grain", 1).ok(), "storage should receive grain");
        clc::economy::EconomyLedger ledger;

        const auto result = clc::economy::sell_resource_with_ledger(wallet, storage, grain_price(10), 1, ledger, "failed sell");
        require(!result.ok, "failed sell with ledger should fail on wallet overflow");
        require(wallet.coins == std::numeric_limits<std::uint64_t>::max() - 5, "failed sell with ledger should preserve wallet");
        require(storage.amount("grain") == 1, "failed sell with ledger should preserve storage");
        require(ledger.entries().empty(), "failed sell with ledger should not record ledger entry");
    }

    return 0;
}
