#include "clc/economy/Ledger.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

namespace clc::economy {
namespace {

std::uint64_t saturating_add(std::uint64_t lhs, std::uint64_t rhs) {
    if (rhs > std::numeric_limits<std::uint64_t>::max() - lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs + rhs;
}

std::int64_t saturating_signed_difference(std::uint64_t positive, std::uint64_t negative) noexcept {
    constexpr auto max_signed = static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
    if (positive >= negative) {
        const auto diff = positive - negative;
        return diff > max_signed ? std::numeric_limits<std::int64_t>::max() : static_cast<std::int64_t>(diff);
    }

    const auto diff = negative - positive;
    if (diff > max_signed) {
        return std::numeric_limits<std::int64_t>::min();
    }
    return -static_cast<std::int64_t>(diff);
}

bool ledger_entry_has_valid_payload(const LedgerEntry& entry) noexcept {
    if (entry.sequence == 0 || entry.resource_id.empty() || entry.quantity == 0 || entry.total_price == 0) {
        return false;
    }

    switch (entry.type) {
    case LedgerEntryType::buy:
    case LedgerEntryType::sell:
        return entry.unit_price > 0 && entry.reference_id.empty();
    case LedgerEntryType::contract_reward:
        return entry.unit_price == 0 && !entry.reference_id.empty();
    }
    return false;
}

bool is_trade_ledger_type(LedgerEntryType type) noexcept {
    return type == LedgerEntryType::buy || type == LedgerEntryType::sell;
}

} // namespace

bool EconomyLedger::record(LedgerEntryType type, const TradeResult& result, std::string note) {
    if (!is_trade_ledger_type(type)) {
        return false;
    }

    if (!result.ok || result.resource_id.empty() || result.quantity == 0 || result.unit_price == 0 || result.total_price == 0) {
        return false;
    }

    if (next_sequence_ == std::numeric_limits<std::uint64_t>::max()) {
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

    ++next_sequence_;
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

    if (next_sequence_ == std::numeric_limits<std::uint64_t>::max()) {
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

    ++next_sequence_;
    return true;
}

data::ValidationReport EconomyLedger::restore_entries(std::vector<LedgerEntry> entries) {
    data::ValidationReport report;
    std::uint64_t expected_sequence = 1;

    for (const auto& entry : entries) {
        if (!ledger_entry_has_valid_payload(entry)) {
            report.add_error("economy.ledger.entry." + std::to_string(entry.sequence), "invalid ledger entry payload");
        }

        if (entry.sequence != expected_sequence) {
            report.add_error(
                "economy.ledger.entry." + std::to_string(entry.sequence),
                "ledger sequence must be contiguous and strictly increasing from 1"
            );
        }

        if (expected_sequence != std::numeric_limits<std::uint64_t>::max()) {
            ++expected_sequence;
        }
    }

    if (!report.ok()) {
        return report;
    }

    entries_ = std::move(entries);
    next_sequence_ = expected_sequence;
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

std::string_view ledger_entry_type_name(LedgerEntryType type) noexcept {
    switch (type) {
    case LedgerEntryType::buy:
        return "buy";
    case LedgerEntryType::sell:
        return "sell";
    case LedgerEntryType::contract_reward:
        return "contract_reward";
    }
    return "unknown";
}

LedgerSummary make_ledger_summary(const EconomyLedger& ledger) {
    LedgerSummary summary;
    std::unordered_map<std::string, std::size_t> resource_indexes;

    auto resource_summary_for = [&](const std::string& resource_id) -> LedgerResourceSummary& {
        const auto existing = resource_indexes.find(resource_id);
        if (existing != resource_indexes.end()) {
            return summary.resources[existing->second];
        }

        resource_indexes.emplace(resource_id, summary.resources.size());
        summary.resources.push_back(LedgerResourceSummary{.resource_id = resource_id});
        return summary.resources.back();
    };

    for (const auto& entry : ledger.entries()) {
        ++summary.entry_count;
        auto& resource = resource_summary_for(entry.resource_id);

        switch (entry.type) {
        case LedgerEntryType::buy:
            ++summary.buy_count;
            summary.total_buy_value = saturating_add(summary.total_buy_value, entry.total_price);
            resource.bought_quantity = saturating_add(resource.bought_quantity, entry.quantity);
            resource.buy_value = saturating_add(resource.buy_value, entry.total_price);
            break;
        case LedgerEntryType::sell:
            ++summary.sell_count;
            summary.total_sell_value = saturating_add(summary.total_sell_value, entry.total_price);
            resource.sold_quantity = saturating_add(resource.sold_quantity, entry.quantity);
            resource.sell_value = saturating_add(resource.sell_value, entry.total_price);
            break;
        case LedgerEntryType::contract_reward:
            ++summary.contract_reward_count;
            summary.total_contract_reward_value = saturating_add(summary.total_contract_reward_value, entry.total_price);
            resource.contract_reward_quantity = saturating_add(resource.contract_reward_quantity, entry.quantity);
            resource.contract_reward_value = saturating_add(resource.contract_reward_value, entry.total_price);
            break;
        }
    }

    for (auto& resource : summary.resources) {
        resource.net_quantity = saturating_signed_difference(
            saturating_add(resource.sold_quantity, resource.contract_reward_quantity),
            resource.bought_quantity
        );
        resource.net_value = saturating_signed_difference(
            saturating_add(resource.sell_value, resource.contract_reward_value),
            resource.buy_value
        );
    }

    std::sort(summary.resources.begin(), summary.resources.end(), [](const LedgerResourceSummary& lhs, const LedgerResourceSummary& rhs) {
        return lhs.resource_id < rhs.resource_id;
    });

    return summary;
}

const LedgerResourceSummary* ledger_resource_summary_by_id(const LedgerSummary& summary, std::string_view resource_id) noexcept {
    for (const auto& resource : summary.resources) {
        if (resource.resource_id == resource_id) {
            return &resource;
        }
    }
    return nullptr;
}

std::string ledger_summary_digest(const LedgerSummary& summary) {
    std::ostringstream out;
    out << "ledger_summary"
        << ";entries=" << summary.entry_count
        << ";buys=" << summary.buy_count
        << ";sells=" << summary.sell_count
        << ";contract_rewards=" << summary.contract_reward_count
        << ";buy_value=" << summary.total_buy_value
        << ";sell_value=" << summary.total_sell_value
        << ";contract_reward_value=" << summary.total_contract_reward_value
        << ";resources=" << summary.resources.size();
    return out.str();
}

} // namespace clc::economy
