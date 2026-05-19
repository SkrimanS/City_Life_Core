#pragma once

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

    [[nodiscard]] const std::vector<LedgerEntry>& entries() const noexcept;
    [[nodiscard]] std::uint64_t next_sequence() const noexcept;
    [[nodiscard]] std::uint64_t total_bought(std::string_view resource_id) const;
    [[nodiscard]] std::uint64_t total_sold(std::string_view resource_id) const;
    [[nodiscard]] std::uint64_t total_contract_rewards(std::string_view resource_id) const;

private:
    std::vector<LedgerEntry> entries_{};
    std::uint64_t next_sequence_{1};
};

} // namespace clc::economy
