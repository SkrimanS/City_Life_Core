#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct PopulationNeedsState final {
    std::string settlement_id{};
    std::uint64_t population{0};
    std::uint64_t workforce{0};
    std::uint64_t housing_capacity{0};
    std::uint64_t food_supply{0};
    std::uint64_t water_supply{0};
    std::uint32_t safety_score{100};
    std::uint32_t tax_pressure{0};
};

struct PopulationNeedsReport final {
    std::string settlement_id{};
    std::uint32_t food_satisfaction{0};
    std::uint32_t water_satisfaction{0};
    std::uint32_t housing_satisfaction{0};
    std::uint32_t happiness_score{0};
    std::int64_t migration_delta{0};
    std::vector<std::string> reasons{};
};

struct WeatherClimateState final {
    std::string scope_id{};
    std::string weather_id{"clear"};
    std::string season_id{"spring"};
    std::uint32_t severity{0};
    std::uint64_t remaining_ticks{0};
};

struct WeatherImpactReport final {
    std::string scope_id{};
    std::uint32_t route_risk_delta{0};
    std::int32_t crop_yield_delta{0};
    std::uint32_t storage_decay_delta{0};
    std::uint32_t market_pressure_delta{0};
    std::vector<std::string> active_effects{};
};

struct PolicyRuleState final {
    std::string policy_id{};
    std::string owner_faction_id{};
    std::string scope_id{};
    std::uint32_t tax_rate{0};
    std::int32_t production_modifier{0};
    std::int32_t happiness_modifier{0};
    std::uint64_t remaining_ticks{0};
    bool enabled{true};
};

struct PolicyImpactReport final {
    std::string policy_id{};
    std::uint32_t market_price_pressure{0};
    std::int32_t production_delta{0};
    std::int32_t happiness_delta{0};
    std::vector<std::string> side_effects{};
};

struct EcologyState final {
    std::string scope_id{};
    std::uint32_t fertility{100};
    std::uint32_t water_level{100};
    std::uint32_t regeneration_rate{10};
    std::uint32_t extraction_pressure{0};
    std::uint32_t pollution{0};
};

struct EcologyReport final {
    std::string scope_id{};
    std::int32_t yield_modifier{0};
    std::int32_t regeneration_delta{0};
    std::uint32_t depletion_risk{0};
    std::uint32_t environmental_pressure{0};
    std::vector<std::string> warnings{};
};

struct WorldEventState final {
    std::string event_id{};
    std::string event_type{};
    std::string scope_id{};
    std::uint32_t severity{0};
    std::uint64_t remaining_ticks{0};
    bool hidden{false};
};

struct CrisisPropagationReport final {
    std::string event_id{};
    std::uint32_t supply_pressure{0};
    std::uint32_t demand_pressure{0};
    std::uint32_t liquidity_pressure{0};
    std::uint32_t route_risk_pressure{0};
    std::uint32_t stability_pressure{0};
    std::vector<std::string> affected_systems{};
};

struct AutonomousDecisionProfile final {
    std::string actor_id{};
    std::string actor_type{};
    std::uint32_t risk_tolerance{50};
    std::uint32_t shortage_response_threshold{50};
    bool enabled{true};
};

struct AutonomousDecisionReport final {
    std::string actor_id{};
    std::string selected_decision{};
    std::string rejected_decision{};
    std::string reason{};
    std::uint32_t priority{0};
};

struct DeepSchemaVersionRegistry final {
    std::uint32_t data_pack_schema_version{1};
    std::uint32_t rules_schema_version{1};
    std::uint32_t effects_schema_version{1};
    std::uint32_t dependency_schema_version{1};
    std::uint32_t market_schema_version{1};
    std::uint32_t scenario_schema_version{1};
    std::uint32_t save_schema_version{1};
    std::uint32_t replay_schema_version{1};
};

struct DeepSimulationScenarioPreset final {
    std::string preset_id{};
    std::string display_name{};
    std::vector<std::string> enabled_feature_ids{};
    std::vector<std::string> stressor_ids{};
};

[[nodiscard]] data::ValidationReport validate_population_needs_state(const PopulationNeedsState& state);
[[nodiscard]] data::ValidationReport validate_weather_climate_state(const WeatherClimateState& state);
[[nodiscard]] data::ValidationReport validate_policy_rule_state(const PolicyRuleState& state);
[[nodiscard]] data::ValidationReport validate_ecology_state(const EcologyState& state);
[[nodiscard]] data::ValidationReport validate_world_event_state(const WorldEventState& state);
[[nodiscard]] data::ValidationReport validate_autonomous_decision_profile(const AutonomousDecisionProfile& profile);
[[nodiscard]] data::ValidationReport validate_deep_schema_version_registry(const DeepSchemaVersionRegistry& registry);
[[nodiscard]] data::ValidationReport validate_deep_simulation_scenario_preset(const DeepSimulationScenarioPreset& preset);

[[nodiscard]] PopulationNeedsReport evaluate_population_needs(const PopulationNeedsState& state);
[[nodiscard]] WeatherImpactReport evaluate_weather_impact(const WeatherClimateState& state);
[[nodiscard]] PolicyImpactReport evaluate_policy_impact(const PolicyRuleState& state);
[[nodiscard]] EcologyReport evaluate_ecology_pressure(const EcologyState& state);
[[nodiscard]] CrisisPropagationReport evaluate_crisis_propagation(const WorldEventState& event);
[[nodiscard]] AutonomousDecisionReport choose_autonomous_decision(
    const AutonomousDecisionProfile& profile,
    const PopulationNeedsReport& population,
    const CrisisPropagationReport& crisis
);

[[nodiscard]] std::string population_needs_digest(const PopulationNeedsReport& report);
[[nodiscard]] std::string weather_impact_digest(const WeatherImpactReport& report);
[[nodiscard]] std::string policy_impact_digest(const PolicyImpactReport& report);
[[nodiscard]] std::string ecology_report_digest(const EcologyReport& report);
[[nodiscard]] std::string crisis_propagation_digest(const CrisisPropagationReport& report);
[[nodiscard]] std::string autonomous_decision_digest(const AutonomousDecisionReport& report);
[[nodiscard]] std::string deep_schema_version_digest(const DeepSchemaVersionRegistry& registry);

} // namespace clc::sim
