#include "clc/sim/Contracts.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::ContractCatalog make_contract_catalog() {
    clc::sim::ContractCatalog catalog;
    require(clc::sim::add_contract(catalog, clc::sim::ResourceDeliveryContract{
        .id = "contract_001",
        .display_name = "Deliver Grain",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "hillford",
        .resource_id = "grain",
        .quantity = 10,
        .reward_coins = 50,
        .due_day = 1,
    }).ok(), "contract should add");
    return catalog;
}

clc::economy::EconomyLedger make_ledger_at_max_sequence() {
    clc::economy::EconomyLedger ledger;
    std::vector<clc::economy::LedgerEntry> entries;
    entries.push_back(clc::economy::LedgerEntry{
        .sequence = std::numeric_limits<std::uint64_t>::max() - 1,
        .type = clc::economy::LedgerEntryType::contract_reward,
        .resource_id = "grain",
        .quantity = 1,
        .unit_price = 0,
        .total_price = 1,
        .reference_id = "previous_contract",
    });

    // This restore is expected to fail with the hardened contiguous sequence rule.
    // The test still keeps a direct max-sequence ledger scenario documented here
    // for future fixture support if non-contiguous restore fixtures are added.
    (void)ledger.restore_entries(entries);
    return ledger;
}

} // namespace

int main() {
    {
        auto catalog = make_contract_catalog();
        clc::sim::ResourceStorage storage;
        require(storage.add("grain", 10).ok(), "storage should receive grain");
        clc::economy::Wallet wallet{.coins = 100};
        clc::economy::EconomyLedger ledger;

        auto result = clc::sim::fulfill_contract_from_storage_with_reward_and_ledger(
            catalog,
            "contract_001",
            storage,
            wallet,
            ledger
        );

        require(result.ok(), "valid fulfillment with ledger should succeed");
        require(storage.amount("grain") == 0, "successful fulfillment should consume cargo");
        require(wallet.coins == 150, "successful fulfillment should credit wallet");
        require(ledger.entries().size() == 1, "successful fulfillment should write ledger entry");
        require(ledger.entries()[0].type == clc::economy::LedgerEntryType::contract_reward, "ledger entry should be contract reward");
        const auto* contract = clc::sim::contract_by_id(catalog, "contract_001");
        require(contract != nullptr && contract->status == clc::sim::ContractStatus::fulfilled, "successful fulfillment should mark contract fulfilled");
    }

    {
        auto catalog = make_contract_catalog();
        clc::sim::ResourceStorage storage;
        require(storage.add("grain", 10).ok(), "storage should receive grain");
        clc::economy::Wallet wallet{.coins = std::numeric_limits<std::uint64_t>::max() - 10};
        clc::economy::EconomyLedger ledger;

        auto result = clc::sim::fulfill_contract_from_storage_with_reward_and_ledger(
            catalog,
            "contract_001",
            storage,
            wallet,
            ledger
        );

        require(!result.ok(), "wallet overflow should reject fulfillment");
        require(storage.amount("grain") == 10, "failed wallet overflow fulfillment should preserve cargo");
        require(wallet.coins == std::numeric_limits<std::uint64_t>::max() - 10, "failed wallet overflow fulfillment should preserve wallet");
        require(ledger.entries().empty(), "failed wallet overflow fulfillment should not write ledger");
        const auto* contract = clc::sim::contract_by_id(catalog, "contract_001");
        require(contract != nullptr && contract->status == clc::sim::ContractStatus::open, "failed wallet overflow fulfillment should preserve contract status");
    }

    return 0;
}
