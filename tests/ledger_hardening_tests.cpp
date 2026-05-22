#include "clc/economy/Ledger.hpp"
#include "clc/economy/Trade.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::economy::TradeResult successful_trade() {
    return clc::economy::TradeResult{
        .ok = true,
        .resource_id = "grain",
        .quantity = 3,
        .unit_price = 10,
        .total_price = 30,
    };
}

} // namespace

int main() {
    const auto trade = successful_trade();

    clc::economy::EconomyLedger ledger;
    require(ledger.record(clc::economy::LedgerEntryType::buy, trade, "buy"), "ledger should record buy trade");
    require(ledger.record(clc::economy::LedgerEntryType::sell, trade, "sell"), "ledger should record sell trade");
    require(!ledger.record(clc::economy::LedgerEntryType::contract_reward, trade, "invalid reward"), "ledger should reject trade result recorded as contract reward");
    require(ledger.record_contract_reward("contract_001", "grain", 3, 50, "reward"), "ledger should record contract reward through reward API");

    require(ledger.entries().size() == 3, "ledger should contain only valid entries");
    require(ledger.entries()[0].sequence == 1, "first sequence should be 1");
    require(ledger.entries()[1].sequence == 2, "second sequence should be 2");
    require(ledger.entries()[2].sequence == 3, "third sequence should be 3");
    require(ledger.next_sequence() == 4, "next sequence should advance after valid records");

    clc::economy::EconomyLedger restored;
    require(restored.restore_entries(ledger.entries()).ok(), "restore should accept contiguous strictly increasing sequences");
    require(restored.entries().size() == 3, "restored ledger should keep entries");
    require(restored.next_sequence() == 4, "restored ledger should continue after restored sequence");

    auto duplicate_sequence_entries = ledger.entries();
    duplicate_sequence_entries[1].sequence = duplicate_sequence_entries[0].sequence;
    clc::economy::EconomyLedger duplicate_sequence_ledger;
    require(!duplicate_sequence_ledger.restore_entries(duplicate_sequence_entries).ok(), "restore should reject duplicate sequence values");
    require(duplicate_sequence_ledger.entries().empty(), "failed duplicate sequence restore should not mutate ledger");
    require(duplicate_sequence_ledger.next_sequence() == 1, "failed duplicate sequence restore should keep next sequence unchanged");

    auto skipped_sequence_entries = ledger.entries();
    skipped_sequence_entries[1].sequence = 3;
    skipped_sequence_entries[2].sequence = 4;
    clc::economy::EconomyLedger skipped_sequence_ledger;
    require(!skipped_sequence_ledger.restore_entries(skipped_sequence_entries).ok(), "restore should reject skipped sequence values");
    require(skipped_sequence_ledger.entries().empty(), "failed skipped sequence restore should not mutate ledger");

    auto unordered_sequence_entries = ledger.entries();
    unordered_sequence_entries[0].sequence = 2;
    unordered_sequence_entries[1].sequence = 1;
    clc::economy::EconomyLedger unordered_sequence_ledger;
    require(!unordered_sequence_ledger.restore_entries(unordered_sequence_entries).ok(), "restore should reject non-monotonic sequence order");

    auto trade_with_reference_entries = ledger.entries();
    trade_with_reference_entries[0].reference_id = "unexpected";
    clc::economy::EconomyLedger invalid_trade_payload_ledger;
    require(!invalid_trade_payload_ledger.restore_entries(trade_with_reference_entries).ok(), "restore should reject trade entries with reference ids");

    auto reward_with_unit_price_entries = ledger.entries();
    reward_with_unit_price_entries[2].unit_price = 1;
    clc::economy::EconomyLedger invalid_reward_payload_ledger;
    require(!invalid_reward_payload_ledger.restore_entries(reward_with_unit_price_entries).ok(), "restore should reject reward entries with unit price");

    return 0;
}
