#include "clc/sim/MultiplayerEconomySafety.hpp"

#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

std::uint64_t saturating_multiply(std::uint64_t lhs, std::uint64_t rhs) noexcept {
    if (lhs != 0 && rhs > std::numeric_limits<std::uint64_t>::max() / lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs * rhs;
}

bool ledger_has_reference(const economy::EconomyLedger& ledger, std::string_view reference_id) {
    if (reference_id.empty()) {
        return false;
    }
    for (const auto& entry : ledger.entries()) {
        if (entry.reference_id == reference_id || entry.note == reference_id) {
            return true;
        }
    }
    return false;
}

} // namespace

std::string_view economy_safety_operation_name(EconomySafetyOperation operation) noexcept {
    switch (operation) {
    case EconomySafetyOperation::buy_resource: return "buy_resource";
    case EconomySafetyOperation::sell_resource: return "sell_resource";
    case EconomySafetyOperation::contract_reward: return "contract_reward";
    }
    return "unknown";
}

EconomySafetyReview review_buy_resource_safety(
    const SimulationRuntime& runtime,
    std::string actor_id,
    std::string settlement_id,
    const economy::MarketPrice& price,
    std::uint64_t quantity,
    std::string reference_id
) {
    EconomySafetyReview review{
        .operation = EconomySafetyOperation::buy_resource,
        .actor_id = std::move(actor_id),
        .reference_id = std::move(reference_id),
        .settlement_id = std::move(settlement_id),
        .resource_id = price.resource_id,
        .quantity = quantity,
        .unit_price = price.price,
        .total_price = saturating_multiply(price.price, quantity),
        .wallet_coins = runtime.wallet.coins,
        .duplicate_reference = ledger_has_reference(runtime.ledger, review.reference_id),
    };
    if (review.actor_id.empty()) {
        review.validation.add_error("economy.actor_id", "actor_id must not be empty");
    }
    if (!runtime.engine.has_settlement(review.settlement_id)) {
        review.validation.add_error("economy.settlement_id", "settlement does not exist");
    }
    if (quantity == 0) {
        review.validation.add_error("economy.quantity", "quantity must be greater than zero");
    }
    if (review.wallet_coins < review.total_price) {
        review.validation.add_error("economy.wallet", "wallet has insufficient coins");
    }
    if (review.duplicate_reference) {
        review.validation.add_error("economy.reference_id", "reference_id is already present in ledger");
    }
    review.allowed = review.validation.ok();
    return review;
}

EconomySafetyReview review_sell_resource_safety(
    const SimulationRuntime& runtime,
    std::string actor_id,
    std::string settlement_id,
    const economy::MarketPrice& price,
    std::uint64_t quantity,
    std::string reference_id
) {
    EconomySafetyReview review{
        .operation = EconomySafetyOperation::sell_resource,
        .actor_id = std::move(actor_id),
        .reference_id = std::move(reference_id),
        .settlement_id = std::move(settlement_id),
        .resource_id = price.resource_id,
        .quantity = quantity,
        .unit_price = price.price,
        .total_price = saturating_multiply(price.price, quantity),
        .wallet_coins = runtime.wallet.coins,
        .stored_amount = runtime.engine.settlement_resource_amount(review.settlement_id, price.resource_id),
        .duplicate_reference = ledger_has_reference(runtime.ledger, review.reference_id),
    };
    if (review.actor_id.empty()) {
        review.validation.add_error("economy.actor_id", "actor_id must not be empty");
    }
    if (!runtime.engine.has_settlement(review.settlement_id)) {
        review.validation.add_error("economy.settlement_id", "settlement does not exist");
    }
    if (quantity == 0) {
        review.validation.add_error("economy.quantity", "quantity must be greater than zero");
    }
    if (review.stored_amount < quantity) {
        review.validation.add_error("economy.storage", "settlement has insufficient resource amount");
    }
    if (review.duplicate_reference) {
        review.validation.add_error("economy.reference_id", "reference_id is already present in ledger");
    }
    review.allowed = review.validation.ok();
    return review;
}

EconomySafetyReview review_contract_reward_safety(
    const SimulationRuntime& runtime,
    std::string actor_id,
    std::string contract_id,
    std::string reference_id
) {
    const auto* contract = contract_by_id(runtime.contracts, contract_id);
    EconomySafetyReview review{
        .operation = EconomySafetyOperation::contract_reward,
        .actor_id = std::move(actor_id),
        .reference_id = std::move(reference_id),
        .resource_id = contract == nullptr ? std::string{} : contract->resource_id,
        .quantity = contract == nullptr ? 0 : contract->quantity,
        .total_price = contract == nullptr ? 0 : contract->reward_coins,
        .wallet_coins = runtime.wallet.coins,
        .duplicate_reference = ledger_has_reference(runtime.ledger, review.reference_id),
    };
    if (review.actor_id.empty()) {
        review.validation.add_error("economy.actor_id", "actor_id must not be empty");
    }
    if (contract == nullptr) {
        review.validation.add_error("economy.contract_id", "contract does not exist");
    } else if (!contract_is_open(*contract)) {
        review.validation.add_error("economy.contract_id", "contract is not open");
    }
    if (review.duplicate_reference) {
        review.validation.add_error("economy.reference_id", "reference_id is already present in ledger");
    }
    review.allowed = review.validation.ok();
    return review;
}

std::string economy_safety_review_digest(const EconomySafetyReview& review) {
    std::ostringstream out;
    out << "economy_safety"
        << ";operation=" << economy_safety_operation_name(review.operation)
        << ";actor=" << review.actor_id
        << ";reference=" << review.reference_id
        << ";settlement=" << review.settlement_id
        << ";resource=" << review.resource_id
        << ";quantity=" << review.quantity
        << ";unit_price=" << review.unit_price
        << ";total_price=" << review.total_price
        << ";wallet=" << review.wallet_coins
        << ";stored=" << review.stored_amount
        << ";duplicate=" << (review.duplicate_reference ? "yes" : "no")
        << ";allowed=" << (review.allowed ? "yes" : "no")
        << ";diagnostics=" << review.validation.messages().size();
    return out.str();
}

} // namespace clc::sim
