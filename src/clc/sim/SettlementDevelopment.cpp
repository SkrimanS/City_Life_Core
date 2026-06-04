#include "clc/sim/SettlementDevelopment.hpp"

#include "clc/sim/Production.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>

namespace clc::sim {
namespace {

constexpr std::uint64_t k_people_per_food_unit = 10;

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

std::uint64_t days_for_horizon(clc::GameTime::Tick horizon_ticks) noexcept {
    if (horizon_ticks == 0) {
        return 0;
    }
    return ((horizon_ticks - 1) / clc::ticks_per_day()) + 1;
}

std::uint64_t food_needed_for_population(std::uint64_t population) noexcept {
    if (population == 0) {
        return 0;
    }
    return ((population - 1) / k_people_per_food_unit) + 1;
}

SettlementDevelopmentPriority priority_for_missing_amount(std::uint64_t missing_amount) noexcept {
    if (missing_amount >= 50) {
        return SettlementDevelopmentPriority::critical;
    }
    if (missing_amount > 0) {
        return SettlementDevelopmentPriority::high;
    }
    return SettlementDevelopmentPriority::normal;
}

SettlementDevelopmentPriority priority_for_pressure(economy::MarketPressureLevel pressure) noexcept {
    switch (pressure) {
    case economy::MarketPressureLevel::depleted:
        return SettlementDevelopmentPriority::critical;
    case economy::MarketPressureLevel::shortage:
        return SettlementDevelopmentPriority::high;
    case economy::MarketPressureLevel::balanced:
        return SettlementDevelopmentPriority::normal;
    case economy::MarketPressureLevel::surplus:
        return SettlementDevelopmentPriority::low;
    }
    return SettlementDevelopmentPriority::normal;
}

std::uint64_t priority_rank(SettlementDevelopmentPriority priority) noexcept {
    switch (priority) {
    case SettlementDevelopmentPriority::low:
        return 0;
    case SettlementDevelopmentPriority::normal:
        return 1;
    case SettlementDevelopmentPriority::high:
        return 2;
    case SettlementDevelopmentPriority::critical:
        return 3;
    }
    return 1;
}

SettlementDevelopmentPriority max_priority(SettlementDevelopmentPriority lhs, SettlementDevelopmentPriority rhs) noexcept {
    return priority_rank(lhs) >= priority_rank(rhs) ? lhs : rhs;
}

SettlementResourceNeed& ensure_need(
    std::vector<SettlementResourceNeed>& needs,
    std::unordered_map<std::string, std::size_t>& index,
    std::string resource_id,
    const SettlementState& settlement,
    const economy::MarketSnapshot& market_snapshot,
    const economy::MarketReport& market_report
) {
    const auto existing = index.find(resource_id);
    if (existing != index.end()) {
        return needs[existing->second];
    }

    SettlementResourceNeed need{.resource_id = std::move(resource_id)};
    need.stored_amount = settlement.storage.amount(need.resource_id);

    if (const auto* signal = economy::market_signal_by_resource(market_snapshot, need.resource_id)) {
        need.market_supply = signal->supply;
        need.market_demand = signal->demand;
        need.market_pressure = signal->pressure;
    }
    need.market_price = economy::market_price_or(market_report, need.resource_id, 0);

    index.emplace(need.resource_id, needs.size());
    needs.push_back(std::move(need));
    return needs.back();
}

SettlementProductionOpportunity make_unknown_building_opportunity(const BuildingProductionSignal& building) {
    return SettlementProductionOpportunity{
        .building_definition_id = building.definition_id,
        .action = SettlementDevelopmentAction::register_definition,
        .priority = SettlementDevelopmentPriority::critical,
        .assigned_workers = building.assigned_workers,
        .reason = "building definition is missing from registry",
    };
}

void sort_plan(SettlementDevelopmentPlan& plan) {
    std::sort(plan.resource_needs.begin(), plan.resource_needs.end(), [](const SettlementResourceNeed& lhs, const SettlementResourceNeed& rhs) {
        if (priority_rank(lhs.priority) != priority_rank(rhs.priority)) {
            return priority_rank(lhs.priority) > priority_rank(rhs.priority);
        }
        return lhs.resource_id < rhs.resource_id;
    });

    std::sort(plan.opportunities.begin(), plan.opportunities.end(), [](const SettlementProductionOpportunity& lhs, const SettlementProductionOpportunity& rhs) {
        if (priority_rank(lhs.priority) != priority_rank(rhs.priority)) {
            return priority_rank(lhs.priority) > priority_rank(rhs.priority);
        }
        if (lhs.building_definition_id != rhs.building_definition_id) {
            return lhs.building_definition_id < rhs.building_definition_id;
        }
        return static_cast<int>(lhs.action) < static_cast<int>(rhs.action);
    });
}

} // namespace

std::string_view settlement_development_priority_name(SettlementDevelopmentPriority priority) noexcept {
    switch (priority) {
    case SettlementDevelopmentPriority::low:
        return "low";
    case SettlementDevelopmentPriority::normal:
        return "normal";
    case SettlementDevelopmentPriority::high:
        return "high";
    case SettlementDevelopmentPriority::critical:
        return "critical";
    }
    return "unknown";
}

std::string_view settlement_development_action_name(SettlementDevelopmentAction action) noexcept {
    switch (action) {
    case SettlementDevelopmentAction::keep_running:
        return "keep_running";
    case SettlementDevelopmentAction::assign_workers:
        return "assign_workers";
    case SettlementDevelopmentAction::import_input:
        return "import_input";
    case SettlementDevelopmentAction::expand_output:
        return "expand_output";
    case SettlementDevelopmentAction::register_definition:
        return "register_definition";
    }
    return "unknown";
}

SettlementDevelopmentPlan make_settlement_development_plan(
    const SettlementState& settlement,
    const data::DataRegistry& registry,
    const economy::MarketReport& market_report,
    clc::GameTime::Tick horizon_ticks
) {
    const auto market_snapshot = economy::make_market_snapshot(market_report);
    const auto production = make_settlement_production_snapshot_with_market(settlement, registry, market_snapshot);
    const auto horizon_days = days_for_horizon(horizon_ticks);

    SettlementDevelopmentPlan plan{
        .settlement_id = settlement.id,
        .display_name = settlement.display_name,
        .population = settlement.population,
        .horizon_ticks = horizon_ticks,
        .building_count = production.building_count,
        .active_building_count = production.active_building_count,
        .blocked_building_count = production.blocked_building_count,
        .unknown_building_count = production.unknown_building_count,
        .total_worker_slots = production.total_worker_slots,
        .assigned_workers = production.assigned_workers,
        .idle_worker_slots = production.idle_worker_slots,
    };

    std::unordered_map<std::string, std::size_t> need_index;
    auto& grain_need = ensure_need(plan.resource_needs, need_index, "grain", settlement, market_snapshot, market_report);
    grain_need.estimated_daily_need = saturating_add(grain_need.estimated_daily_need, food_needed_for_population(settlement.population));

    for (const auto& building : production.buildings) {
        if (!building.known_definition) {
            plan.opportunities.push_back(make_unknown_building_opportunity(building));
            ++plan.high_priority_opportunity_count;
            continue;
        }

        if (building.idle_worker_slots > 0) {
            plan.opportunities.push_back(SettlementProductionOpportunity{
                .building_definition_id = building.definition_id,
                .display_name = building.display_name,
                .action = SettlementDevelopmentAction::assign_workers,
                .priority = SettlementDevelopmentPriority::normal,
                .assigned_workers = building.assigned_workers,
                .worker_slots = building.worker_slots,
                .idle_worker_slots = building.idle_worker_slots,
                .reason = "building has unassigned worker slots",
            });
        }

        for (const auto& resource : building.resources) {
            if (resource.role == ProductionResourceRole::input) {
                auto& need = ensure_need(plan.resource_needs, need_index, resource.resource_id, settlement, market_snapshot, market_report);
                need.estimated_daily_need = saturating_add(need.estimated_daily_need, static_cast<std::uint64_t>(building.assigned_workers));
                if (!resource.available_locally) {
                    plan.opportunities.push_back(SettlementProductionOpportunity{
                        .building_definition_id = building.definition_id,
                        .display_name = building.display_name,
                        .action = SettlementDevelopmentAction::import_input,
                        .priority = SettlementDevelopmentPriority::high,
                        .assigned_workers = building.assigned_workers,
                        .worker_slots = building.worker_slots,
                        .missing_input_count = 1,
                        .resource_id = resource.resource_id,
                        .reason = "input resource is missing locally",
                    });
                    ++plan.high_priority_opportunity_count;
                }
                continue;
            }

            if (resource.market_shortage) {
                const auto priority = priority_for_pressure(resource.market_pressure);
                plan.opportunities.push_back(SettlementProductionOpportunity{
                    .building_definition_id = building.definition_id,
                    .display_name = building.display_name,
                    .action = SettlementDevelopmentAction::expand_output,
                    .priority = priority,
                    .assigned_workers = building.assigned_workers,
                    .worker_slots = building.worker_slots,
                    .idle_worker_slots = building.idle_worker_slots,
                    .pressured_output_count = 1,
                    .resource_id = resource.resource_id,
                    .reason = "output resource has market shortage pressure",
                });
                if (priority_rank(priority) >= priority_rank(SettlementDevelopmentPriority::high)) {
                    ++plan.high_priority_opportunity_count;
                }
            }
        }

        if (building.has_workers && building.has_required_inputs && !building.output_has_market_pressure) {
            plan.opportunities.push_back(SettlementProductionOpportunity{
                .building_definition_id = building.definition_id,
                .display_name = building.display_name,
                .action = SettlementDevelopmentAction::keep_running,
                .priority = SettlementDevelopmentPriority::low,
                .assigned_workers = building.assigned_workers,
                .worker_slots = building.worker_slots,
                .reason = "building is running without immediate shortage pressure",
            });
        }
    }

    for (auto& need : plan.resource_needs) {
        const auto required = saturating_multiply(need.estimated_daily_need, horizon_days);
        need.missing_amount = required > need.stored_amount ? required - need.stored_amount : 0;
        need.priority = max_priority(priority_for_missing_amount(need.missing_amount), priority_for_pressure(need.market_pressure));
        if (need.priority == SettlementDevelopmentPriority::critical) {
            ++plan.critical_need_count;
        }
    }

    sort_plan(plan);
    return plan;
}

data::ValidationReport validate_settlement_development_plan(const SettlementDevelopmentPlan& plan) {
    data::ValidationReport report;
    if (plan.settlement_id.empty()) {
        report.add_error("settlement_development", "settlement_id must not be empty");
    }
    if (plan.horizon_ticks == 0) {
        report.add_error("settlement_development." + plan.settlement_id, "horizon_ticks must be greater than zero");
    }
    if (plan.assigned_workers > plan.total_worker_slots) {
        report.add_error("settlement_development." + plan.settlement_id, "assigned_workers exceeds total_worker_slots");
    }

    for (const auto& need : plan.resource_needs) {
        if (need.resource_id.empty()) {
            report.add_error("settlement_development." + plan.settlement_id + ".resource", "resource_id must not be empty");
        }
        if (need.missing_amount > 0 && need.estimated_daily_need == 0) {
            report.add_warning("settlement_development." + plan.settlement_id + ".resource." + need.resource_id, "missing_amount is set without estimated_daily_need");
        }
    }

    for (const auto& opportunity : plan.opportunities) {
        if (opportunity.building_definition_id.empty()) {
            report.add_error("settlement_development." + plan.settlement_id + ".opportunity", "building_definition_id must not be empty");
        }
        if ((opportunity.action == SettlementDevelopmentAction::import_input || opportunity.action == SettlementDevelopmentAction::expand_output)
            && opportunity.resource_id.empty()) {
            report.add_error("settlement_development." + plan.settlement_id + ".opportunity." + opportunity.building_definition_id, "resource_id is required for resource actions");
        }
    }

    return report;
}

const SettlementResourceNeed* settlement_resource_need(
    const SettlementDevelopmentPlan& plan,
    std::string_view resource_id
) noexcept {
    for (const auto& need : plan.resource_needs) {
        if (need.resource_id == resource_id) {
            return &need;
        }
    }
    return nullptr;
}

const SettlementProductionOpportunity* settlement_production_opportunity(
    const SettlementDevelopmentPlan& plan,
    std::string_view building_definition_id,
    SettlementDevelopmentAction action
) noexcept {
    for (const auto& opportunity : plan.opportunities) {
        if (opportunity.building_definition_id == building_definition_id && opportunity.action == action) {
            return &opportunity;
        }
    }
    return nullptr;
}

std::string settlement_development_plan_digest(const SettlementDevelopmentPlan& plan) {
    std::ostringstream out;
    out << "settlement_development"
        << ";settlement=" << plan.settlement_id
        << ";horizon_ticks=" << plan.horizon_ticks
        << ";population=" << plan.population
        << ";buildings=" << plan.building_count
        << ";active=" << plan.active_building_count
        << ";blocked=" << plan.blocked_building_count
        << ";unknown=" << plan.unknown_building_count
        << ";worker_slots=" << plan.total_worker_slots
        << ";assigned_workers=" << plan.assigned_workers
        << ";idle_worker_slots=" << plan.idle_worker_slots
        << ";resource_needs=" << plan.resource_needs.size()
        << ";critical_needs=" << plan.critical_need_count
        << ";opportunities=" << plan.opportunities.size()
        << ";high_opportunities=" << plan.high_priority_opportunity_count;
    if (!plan.resource_needs.empty()) {
        out << ";top_need=" << plan.resource_needs.front().resource_id
            << ":" << settlement_development_priority_name(plan.resource_needs.front().priority);
    }
    if (!plan.opportunities.empty()) {
        out << ";top_action=" << settlement_development_action_name(plan.opportunities.front().action);
    }
    return out.str();
}

} // namespace clc::sim
