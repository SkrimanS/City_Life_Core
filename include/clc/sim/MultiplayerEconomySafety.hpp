#pragma once

#include "clc/data/Validation.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Contracts.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace clc::sim {

enum class EconomySafetyOperation {
    buy_resource,
    sell_resource,
    contract_reward,
};

struct EconomySafetyReview final {
    EconomySafetyOperation operation{EconomySafetyOperation::buy_resource};
    std::string actor_id{};
    std::string reference_id{};
    std::string settlement_id{};
    std::string resource_id{};
    std::uint64_t quantity{0};
    std::uint64_t unit_price{0};
    std::uint64_t total_price{0};
    std::uint64_t wallet_coins{0};
    std::uint64_t stored_amount{0};
    bool allowed{false};
    bool duplicate_reference{false};
    data::ValidationReport validation{};
};

[[nodiscard]] std::string_view economy_safety_operation_name(EconomySafetyOperation operation) noexcept;
[[nodiscard]] EconomySafetyReview review_buy_resource_safety(
    const SimulationRuntime& runtime,
    std::string actor_id,
    std::string settlement_id,
    const economy::MarketPrice& price,
    std::uint64_t quantity,
    std::string reference_id = {}
);
[[nodiscard]] EconomySafetyReview review_sell_resource_safety(
    const SimulationRuntime& runtime,
    std::string actor_id,
    std::string settlement_id,
    const economy::MarketPrice& price,
    std::uint64_t quantity,
    std::string reference_id = {}
);
[[nodiscard]] EconomySafetyReview review_contract_reward_safety(
    const SimulationRuntime& runtime,
    std::string actor_id,
    std::string contract_id,
    std::string reference_id = {}
);
[[nodiscard]] std::string economy_safety_review_digest(const EconomySafetyReview& review);

} // namespace clc::sim
