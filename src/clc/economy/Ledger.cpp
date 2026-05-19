#include "clc/economy/Ledger.hpp"

#include <limits>
#include <string>
#include <utility>

namespace clc::economy {
namespace {

std::uint64_t saturating_add(std::uint64_t lhs, std::uint64_t rhs) {
    if (rhs > std::numeric_limits<std::uint64_t>::max() - lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs + rhs;
}

bool ledger_entry_has_valid_payload(const LedgerEntry& entry) noexcept {
    if (entry.sequence == 0 || entry.resource_id.empty() || entry.quantity == 0 || entry.total_price == 0) {
        return false;
    }

    switch (entry.type) {
    case LedgerEntryType::buy:
    case LedgerEntryType::sell:
        return entry.unit_price > 0;
    case LedgerEntryType::contract_reward:
        return entry.unit_price == 0 && !entry.reference_id.empty();
    }
    return false;
}

} // namespace

bool EconomyLedger::record(LedgerEntryType type, const TradeResult& result, std::string note) {
    if (!result.ok || result.resource_id.empty() || result.quantity == 0 || result.unit_price == 0 || result.total_price == 0) {
        return false;
    }

    entries_.push_back(LedgerEntry{
        .sequence = next_sequence_,
        .type = type,
        .resource_id = result.resource_id,
        .quantity = result.quantity,
        .unit_price = result.unit_price,
        .total_price = result.total_price,
        .note = std::move(note),
    });

    if (next_sequence_ != std::numeric_limits<std::uint64_t>::max()) {
        ++next_sequence_;
    }

    return true;
}

bool EconomyLedger::record_contract_reward(
    std::string contract_id,
    std::string resource_id,
    std::uint64_t quantity,
    std::uint64_t reward_coins,
    std::string note
) {
    if (contract_id.empty() || resource_id.empty() || quantity == 0 || reward_coins == 0) {
        return false;
    }

    entries_.push_back(LedgerEntry{
        .sequence = next_sequence_,
        .type = LedgerEntryType::contract_reward,
        .resource_id = std::move(resource_id),
        .quantity = quantity,
        .unit_price = 0,
        .total_price = reward_coins,
        .reference_id = std::move(contract_id),
        .note = std::move(note),
    });

    if (next_sequence_ != std::numeric_limits<std::uint64_t>::max()) {
        ++next_sequence_;
    }

    return true;
}

data::ValidationReport EconomyLedger::restore_entries(std::vector<LedgerEntry> entries) {
    data::ValidationReport report;
    std::uint64_t max_sequence = 0;
    for (const auto& entry : entries) {
        if (!ledger_entry_has_valid_payload(entry)) {
            report.add_error("economy.ledger.entry." + std::to_string(entry.sequence), "invalid ledger entry payload");
        }
        if (entry.sequence > max_sequence) {
            max_sequence = entry.sequence;
        }
    }

    if (!report.ok()) {
        return report;
    }

    entries_ = std::move(entries);
    next_sequence_ = max_sequence == std::numeric_limits<std::uint64_t>::max()
        ? std::numeric_limits<std::uint64_t>::max()
        : max_sequence + 1;
    return report;
}

const std::vector<LedgerEntry>& EconomyLedger::entries() const noexcept {
    return entries_;
}

std::uint64_t EconomyLedger::next_sequence() const noexcept {
    return next_sequence_;
}

std::uint64_t EconomyLedger::total_bought(std::string_view resource_id) const {
    std::uint64_t total{};
    for (const auto& entry : entries_) {
        if (entry.type == LedgerEntryType::buy && entry.resource_id == resource_id) {
            total = saturating_add(total, entry.quantity);
        }
    }
    return total;
}

std::uint64_t EconomyLedger::total_sold(std::string_view resource_id) const {
    std::uint64_t total{};
    for (const auto& entry : entries_) {
        if (entry.type == LedgerEntryType::sell && entry.resource_id == resource_id) {
            total = saturating_add(total, entry.quantity);
        }
    }
    return total;
}

std::uint64_t EconomyLedger::total_contract_rewards(std::string_view resource_id) const {
    std::uint64_t total{};
    for (const auto& entry : entries_) {
        if (entry.type == LedgerEntryType::contract_reward && entry.resource_id == resource_id) {
            total = saturating_add(total, entry.total_price);
        }
    }
    return total;
}

} // namespace clc::economy
