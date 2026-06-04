#pragma once

#include "clc/core/Time.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Settlement.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

enum class SettlementDevelopmentPriority {
    low,
    normal,
    high,
    critical,
};

enum class SettlementDevelopmentAction {
    keep_running,
    assign_workers,
    import_input,
    expand_output,
    register_definition,
};

struct SettlementResourceNeed final {
    std::string resource_id{};
    std::uint64_t stored_amount{0};
    std::uint64_t estimated_daily_need{0};
    std::uint64_t missing_amount{0};
    std::uint64_t market_supply{0};
    std::uint64_t market_demand{0};
    std::uint64_t market_price{0};
    economy::MarketPressureLevel market_pressure{economy::MarketPressureLevel::balanced};
    SettlementDevelopmentPriority priority{SettlementDevelopmentPriority::normal};
};

struct SettlementProductionOpportunity final {
    std::string building_definition_id{};
    std::string display_name{};
    SettlementDevelopmentAction action{SettlementDevelopmentAction::keep_running};
    SettlementDevelopmentPriority priority{SettlementDevelopmentPriority::normal};
    std::uint32_t assigned_workers{0};
    std::uint32_t worker_slots{0};
    std::uint32_t idle_worker_slots{0};
    std::uint64_t missing_input_count{0};
    std::uint64_t pressured_output_count{0};
    std::string resource_id{};
    std::string reason{};
};

struct SettlementDevelopmentPlan final {
    std::string settlement_id{};
    std::string display_name{};
    std::uint64_t population{0};
    clc::GameTime::Tick horizon_ticks{0};
    std::uint64_t building_count{0};
    std::uint64_t active_building_count{0};
    std::uint64_t blocked_building_count{0};
    std::uint64_t unknown_building_count{0};
    std::uint64_t total_worker_slots{0};
    std::uint64_t assigned_workers{0};
    std::uint64_t idle_worker_slots{0};
    std::uint64_t critical_need_count{0};
    std::uint64_t high_priority_opportunity_count{0};
    std::vector<SettlementResourceNeed> resource_needs{};
    std::vector<SettlementProductionOpportunity> opportunities{};
};

[[nodiscard]] std::string_view settlement_development_priority_name(SettlementDevelopmentPriority priority) noexcept;
[[nodiscard]] std::string_view settlement_development_action_name(SettlementDevelopmentAction action) noexcept;

[[nodiscard]] SettlementDevelopmentPlan make_settlement_development_plan(
    const SettlementState& settlement,
    const data::DataRegistry& registry,
    const economy::MarketReport& market_report,
    clc::GameTime::Tick horizon_ticks = clc::days_to_ticks(1)
);

[[nodiscard]] data::ValidationReport validate_settlement_development_plan(const SettlementDevelopmentPlan& plan);

[[nodiscard]] const SettlementResourceNeed* settlement_resource_need(
    const SettlementDevelopmentPlan& plan,
    std::string_view resource_id
) noexcept;

[[nodiscard]] const SettlementProductionOpportunity* settlement_production_opportunity(
    const SettlementDevelopmentPlan& plan,
    std::string_view building_definition_id,
    SettlementDevelopmentAction action
) noexcept;

[[nodiscard]] std::string settlement_development_plan_digest(const SettlementDevelopmentPlan& plan);

} // namespace clc::sim
