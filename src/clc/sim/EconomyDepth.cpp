#include "clc/sim/EconomyDepth.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

std::uint64_t saturating_add(std::uint64_t lhs, std::uint64_t rhs) noexcept {
    if (rhs > std::numeric_limits<std::uint64_t>::max() - lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs + rhs;
}

std::uint64_t saturating_multiply(std::uint64_t lhs, std::uint64_t rhs) noexcept {
    if (lhs != 0 && rhs > std::numeric_limits<std::uint64_t>::max() / lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs * rhs;
}

std::int64_t signed_margin(std::uint64_t reward, std::uint64_t value) noexcept {
    const auto max_i64 = static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max());
    if (reward >= value) {
        return static_cast<std::int64_t>(std::min(reward - value, max_i64));
    }
    return -static_cast<std::int64_t>(std::min(value - reward, max_i64));
}

std::string blocked_reason(const ContractEconomyAssessment& assessment) {
    if (!assessment.known_contract) {
        return "unknown contract";
    }
    if (!assessment.open) {
        return "contract is not open";
    }
    if (assessment.overdue) {
        return "contract is overdue";
    }
    if (!assessment.accepted_by_factions) {
        return "faction access blocks contract";
    }
    if (!assessment.has_required_resources) {
        return "insufficient resources";
    }
    return "deliverable";
}

ContractResourceFlowPlan unknown_contract_flow_plan(
    std::string_view contract_id,
    ContractResourceFlowSource source,
    std::string source_id
) {
    return ContractResourceFlowPlan{
        .contract_id = std::string{contract_id},
        .source_id = std::move(source_id),
        .source = source,
        .source_ready = false,
        .owner_matches = false,
        .reason = "unknown contract",
    };
}

} // namespace

std::string_view contract_resource_flow_source_name(ContractResourceFlowSource source) noexcept {
    switch (source) {
    case ContractResourceFlowSource::storage:
        return "storage";
    case ContractResourceFlowSource::caravan:
        return "caravan";
    }
    return "unknown";
}

ContractEconomyAssessment make_contract_economy_assessment(
    const ContractCatalog& catalog,
    std::string_view contract_id,
    const FactionCatalog& factions,
    const ResourceStorage& available_resources,
    const economy::MarketReport& market,
    clc::GameTime::Tick current_tick
) {
    ContractEconomyAssessment assessment{
        .contract_id = std::string{contract_id},
        .current_tick = current_tick,
    };

    const auto* contract = contract_by_id(catalog, contract_id);
    if (contract == nullptr) {
        assessment.reason = "unknown contract";
        return assessment;
    }

    assessment.known_contract = true;
    assessment.resource_id = contract->resource_id;
    assessment.issuer_faction_id = contract->issuer_faction_id;
    assessment.receiver_faction_id = contract->receiver_faction_id;
    assessment.quantity = contract->quantity;
    assessment.reward_coins = contract->reward_coins;
    assessment.due_tick = contract_due_ticks(*contract);
    assessment.open = contract_is_open(*contract);
    assessment.overdue = assessment.open && current_tick > 0 && current_tick > assessment.due_tick;
    assessment.ticks_until_due = assessment.overdue || current_tick >= assessment.due_tick ? 0 : assessment.due_tick - current_tick;

    assessment.available_quantity = available_resources.amount(contract->resource_id);
    assessment.has_required_resources = assessment.available_quantity >= contract->quantity;
    assessment.missing_quantity = assessment.has_required_resources ? 0 : contract->quantity - assessment.available_quantity;

    assessment.market_unit_price = economy::market_price_or(market, contract->resource_id, 0);
    assessment.market_total_value = saturating_multiply(assessment.market_unit_price, contract->quantity);
    assessment.reward_margin = signed_margin(contract->reward_coins, assessment.market_total_value);
    assessment.reward_covers_market_value = contract->reward_coins >= assessment.market_total_value;

    assessment.access = make_faction_access_report(factions, contract->issuer_faction_id, contract->receiver_faction_id);
    assessment.accepted_by_factions = assessment.access.can_issue_contract && assessment.access.can_receive_contract;
    assessment.deliverable = assessment.open && !assessment.overdue && assessment.accepted_by_factions && assessment.has_required_resources;
    assessment.reason = blocked_reason(assessment);
    return assessment;
}

ContractResourceFlowPlan plan_contract_flow_from_storage(
    const ContractCatalog& catalog,
    std::string_view contract_id,
    const ResourceStorage& storage,
    std::string source_id
) {
    const auto* contract = contract_by_id(catalog, contract_id);
    if (contract == nullptr) {
        return unknown_contract_flow_plan(contract_id, ContractResourceFlowSource::storage, std::move(source_id));
    }

    const auto available = storage.amount(contract->resource_id);
    const auto transferable = std::min(available, contract->quantity);
    const auto missing = contract->quantity - transferable;
    const auto open = contract_is_open(*contract);
    return ContractResourceFlowPlan{
        .contract_id = contract->id,
        .resource_id = contract->resource_id,
        .source_id = std::move(source_id),
        .source = ContractResourceFlowSource::storage,
        .required_quantity = contract->quantity,
        .available_quantity = available,
        .transferable_quantity = transferable,
        .missing_quantity = missing,
        .source_ready = true,
        .owner_matches = true,
        .can_fulfill = open && missing == 0,
        .reason = !open ? "contract is not open" : (missing == 0 ? "ready" : "insufficient resources"),
    };
}

ContractResourceFlowPlan plan_contract_flow_from_caravan(
    const ContractCatalog& catalog,
    std::string_view contract_id,
    const CaravanState& caravan,
    const OwnershipCatalog* ownership,
    std::string_view expected_owner_faction_id
) {
    const auto* contract = contract_by_id(catalog, contract_id);
    if (contract == nullptr) {
        return unknown_contract_flow_plan(contract_id, ContractResourceFlowSource::caravan, caravan.id);
    }

    const auto available = caravan.cargo.amount(contract->resource_id);
    const auto transferable = std::min(available, contract->quantity);
    const auto missing = contract->quantity - transferable;
    const auto ready = caravan_arrived(caravan);
    bool owner_matches = true;
    if (ownership != nullptr && !expected_owner_faction_id.empty()) {
        owner_matches = caravan_owner(*ownership, caravan.id) == expected_owner_faction_id;
    }

    const auto open = contract_is_open(*contract);
    std::string reason = "ready";
    if (!open) {
        reason = "contract is not open";
    } else if (!ready) {
        reason = "caravan has not arrived";
    } else if (!owner_matches) {
        reason = "caravan owner does not match expected faction";
    } else if (missing != 0) {
        reason = "insufficient resources";
    }

    return ContractResourceFlowPlan{
        .contract_id = contract->id,
        .resource_id = contract->resource_id,
        .source_id = caravan.id,
        .source = ContractResourceFlowSource::caravan,
        .required_quantity = contract->quantity,
        .available_quantity = available,
        .transferable_quantity = transferable,
        .missing_quantity = missing,
        .source_ready = ready,
        .owner_matches = owner_matches,
        .can_fulfill = open && ready && owner_matches && missing == 0,
        .reason = std::move(reason),
    };
}

ContractEconomyPortfolioSummary make_contract_economy_portfolio_summary(
    const ContractCatalog& catalog,
    const FactionCatalog& factions,
    const ResourceStorage& available_resources,
    const economy::MarketReport& market,
    clc::GameTime::Tick current_tick
) {
    ContractEconomyPortfolioSummary summary;
    summary.contract_count = catalog.contracts.size();

    for (const auto& contract : catalog.contracts) {
        const auto assessment = make_contract_economy_assessment(
            catalog,
            contract.id,
            factions,
            available_resources,
            market,
            current_tick
        );

        summary.total_reward_coins = saturating_add(summary.total_reward_coins, assessment.reward_coins);
        summary.total_market_value = saturating_add(summary.total_market_value, assessment.market_total_value);
        summary.total_reward_margin = signed_margin(summary.total_reward_coins, summary.total_market_value);

        if (assessment.deliverable) {
            ++summary.deliverable_count;
            summary.deliverable_contract_ids.push_back(contract.id);
            continue;
        }

        summary.blocked_contract_ids.push_back(contract.id);
        if (!assessment.open) {
            ++summary.blocked_by_status_count;
        }
        if (!assessment.accepted_by_factions) {
            ++summary.blocked_by_reputation_count;
        }
        if (!assessment.has_required_resources) {
            ++summary.blocked_by_resources_count;
        }
        if (assessment.overdue) {
            ++summary.overdue_count;
        }
    }

    return summary;
}

data::ValidationReport validate_contract_economy_assessment(
    const ContractEconomyAssessment& assessment
) {
    data::ValidationReport report;
    if (!assessment.known_contract) {
        report.add_error("simulation.contract." + assessment.contract_id, "unknown contract");
        return report;
    }
    if (!assessment.open) {
        report.add_error("simulation.contract." + assessment.contract_id, "contract is not open");
    }
    if (assessment.overdue) {
        report.add_error("simulation.contract." + assessment.contract_id, "contract is overdue");
    }
    if (!assessment.accepted_by_factions) {
        report.add_error("simulation.contract." + assessment.contract_id + ".factions", "faction access blocks contract");
    }
    if (!assessment.has_required_resources) {
        report.add_error("simulation.contract." + assessment.contract_id + ".resources", "insufficient resources");
    }
    if (!assessment.reward_covers_market_value) {
        report.add_warning("simulation.contract." + assessment.contract_id + ".reward", "reward is below current market value");
    }
    return report;
}

std::string contract_economy_assessment_digest(const ContractEconomyAssessment& assessment) {
    std::ostringstream out;
    out << "contract_economy"
        << ";id=" << assessment.contract_id
        << ";resource=" << assessment.resource_id
        << ";known=" << (assessment.known_contract ? "yes" : "no")
        << ";open=" << (assessment.open ? "yes" : "no")
        << ";deliverable=" << (assessment.deliverable ? "yes" : "no")
        << ";overdue=" << (assessment.overdue ? "yes" : "no")
        << ";available=" << assessment.available_quantity
        << ";missing=" << assessment.missing_quantity
        << ";reward=" << assessment.reward_coins
        << ";market_value=" << assessment.market_total_value
        << ";margin=" << assessment.reward_margin
        << ";access=" << faction_access_level_name(assessment.access.access);
    return out.str();
}

std::string contract_resource_flow_plan_digest(const ContractResourceFlowPlan& plan) {
    std::ostringstream out;
    out << "contract_flow"
        << ";id=" << plan.contract_id
        << ";resource=" << plan.resource_id
        << ";source=" << contract_resource_flow_source_name(plan.source)
        << ";source_id=" << plan.source_id
        << ";ready=" << (plan.source_ready ? "yes" : "no")
        << ";owner=" << (plan.owner_matches ? "yes" : "no")
        << ";required=" << plan.required_quantity
        << ";available=" << plan.available_quantity
        << ";transferable=" << plan.transferable_quantity
        << ";missing=" << plan.missing_quantity
        << ";can_fulfill=" << (plan.can_fulfill ? "yes" : "no");
    return out.str();
}

std::string contract_economy_portfolio_summary_digest(const ContractEconomyPortfolioSummary& summary) {
    std::ostringstream out;
    out << "contract_economy_portfolio"
        << ";contracts=" << summary.contract_count
        << ";deliverable=" << summary.deliverable_count
        << ";blocked_status=" << summary.blocked_by_status_count
        << ";blocked_reputation=" << summary.blocked_by_reputation_count
        << ";blocked_resources=" << summary.blocked_by_resources_count
        << ";overdue=" << summary.overdue_count
        << ";reward=" << summary.total_reward_coins
        << ";market_value=" << summary.total_market_value
        << ";margin=" << summary.total_reward_margin;
    return out.str();
}

} // namespace clc::sim
