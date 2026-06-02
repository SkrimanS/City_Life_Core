#pragma once

#include "clc/data/Validation.hpp"
#include "clc/economy/Trade.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::economy {

enum class LedgerEntryType {
    buy,
    sell,
    contract_reward,
};

struct LedgerEntry final {
    std::uint64_t sequence{0};
    LedgerEntryType type{LedgerEntryType::buy};
    std::string resource_id{};
    std::uint64_t quantity{0};
    std::uint64_t unit_price{0};
    std::uint64_t total_price{0};
    std::string reference_id{};
    std::string note{};
};

struct LedgerResourceSummary final {
    std::string resource_id{};
    std::uint64_t bought_quantity{0};
    std::uint64_t sold_quantity{0};
    std::uint64_t contract_reward_quantity{0};
    std::uint64_t buy_value{0};
    std::uint64_t sell_value{0};
    std::uint64_t contract_reward_value{0};
    std::int64_t net_quantity{0};
    std::int64_t net_value{0};
};

struct LedgerSummary final {
    std::uint64_t entry_count{0};
    std::uint64_t buy_count{0};
    std::uint64_t sell_count{0};
    std::uint64_t contract_reward_count{0};
    std::uint64_t total_buy_value{0};
    std::uint64_t total_sell_value{0};
    std::uint64_t total_contract_reward_value{0};
    std::vector<LedgerResourceSummary> resources{};
};

class EconomyLedger final {
public:
    [[nodiscard]] bool record(LedgerEntryType type, const TradeResult& result, std::string note = {});
    [[nodiscard]] bool record_contract_reward(
        std::string contract_id,
        std::string resource_id,
        std::uint64_t quantity,
        std::uint64_t reward_coins,
        std::string note = {}
    );
    [[nodiscard]] data::ValidationReport restore_entries(std::vector<LedgerEntry> entries);

    [[nodiscard]] const std::vector<LedgerEntry>& entries() const noexcept;
    [[nodiscard]] std::uint64_t next_sequence() const noexcept;
    [[nodiscard]] std::uint64_t total_bought(std::string_view resource_id) const;
    [[nodiscard]] std::uint64_t total_sold(std::string_view resource_id) const;
    [[nodiscard]] std::uint64_t total_contract_rewards(std::string_view resource_id) const;

private:
    std::vector<LedgerEntry> entries_{};
    std::uint64_t next_sequence_{1};
};

[[nodiscard]] std::string_view ledger_entry_type_name(LedgerEntryType type) noexcept;
[[nodiscard]] LedgerSummary make_ledger_summary(const EconomyLedger& ledger);
[[nodiscard]] const LedgerResourceSummary* ledger_resource_summary_by_id(
    const LedgerSummary& summary,
    std::string_view resource_id
) noexcept;
[[nodiscard]] std::string ledger_summary_digest(const LedgerSummary& summary);

} // namespace clc::economy
