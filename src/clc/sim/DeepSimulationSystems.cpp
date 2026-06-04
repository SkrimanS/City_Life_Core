#include "clc/sim/DeepSimulationSystems.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

std::uint32_t clamp_percent(std::uint64_t value) noexcept {
    return static_cast<std::uint32_t>(std::min<std::uint64_t>(100, value));
}

std::uint32_t satisfaction(std::uint64_t available, std::uint64_t required) noexcept {
    if (required == 0) {
        return 100;
    }
    return clamp_percent((available * 100) / required);
}

std::uint32_t bounded_add(std::uint32_t lhs, std::uint32_t rhs) noexcept {
    return static_cast<std::uint32_t>(std::min<std::uint64_t>(100, static_cast<std::uint64_t>(lhs) + rhs));
}

std::uint32_t bounded_severity(std::uint32_t severity) noexcept {
    return std::min<std::uint32_t>(100, severity);
}

void add_if(std::vector<std::string>& values, bool condition, std::string value) {
    if (condition) {
        values.push_back(std::move(value));
    }
}

} // namespace

data::ValidationReport validate_population_needs_state(const PopulationNeedsState& state) {
    data::ValidationReport report;
    if (state.settlement_id.empty()) {
        report.add_error("deep.population.settlement_id", "settlement_id must not be empty");
    }
    if (state.workforce > state.population) {
        report.add_error("deep.population.workforce", "workforce must not exceed population");
    }
    if (state.safety_score > 100) {
        report.add_error("deep.population.safety_score", "safety_score must be in range 0..100");
    }
    if (state.tax_pressure > 100) {
        report.add_error("deep.population.tax_pressure", "tax_pressure must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_weather_climate_state(const WeatherClimateState& state) {
    data::ValidationReport report;
    if (state.scope_id.empty()) {
        report.add_error("deep.weather.scope_id", "scope_id must not be empty");
    }
    if (state.weather_id.empty()) {
        report.add_error("deep.weather.weather_id", "weather_id must not be empty");
    }
    if (state.season_id.empty()) {
        report.add_error("deep.weather.season_id", "season_id must not be empty");
    }
    if (state.severity > 100) {
        report.add_error("deep.weather.severity", "severity must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_policy_rule_state(const PolicyRuleState& state) {
    data::ValidationReport report;
    if (state.policy_id.empty()) {
        report.add_error("deep.policy.policy_id", "policy_id must not be empty");
    }
    if (state.scope_id.empty()) {
        report.add_error("deep.policy.scope_id", "scope_id must not be empty");
    }
    if (state.tax_rate > 100) {
        report.add_error("deep.policy.tax_rate", "tax_rate must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_ecology_state(const EcologyState& state) {
    data::ValidationReport report;
    if (state.scope_id.empty()) {
        report.add_error("deep.ecology.scope_id", "scope_id must not be empty");
    }
    if (state.fertility > 100 || state.water_level > 100 || state.regeneration_rate > 100 || state.extraction_pressure > 100 || state.pollution > 100) {
        report.add_error("deep.ecology.range", "ecology scores must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_world_event_state(const WorldEventState& state) {
    data::ValidationReport report;
    if (state.event_id.empty()) {
        report.add_error("deep.event.event_id", "event_id must not be empty");
    }
    if (state.event_type.empty()) {
        report.add_error("deep.event.event_type", "event_type must not be empty");
    }
    if (state.scope_id.empty()) {
        report.add_error("deep.event.scope_id", "scope_id must not be empty");
    }
    if (state.severity > 100) {
        report.add_error("deep.event.severity", "severity must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_autonomous_decision_profile(const AutonomousDecisionProfile& profile) {
    data::ValidationReport report;
    if (profile.actor_id.empty()) {
        report.add_error("deep.actor.actor_id", "actor_id must not be empty");
    }
    if (profile.actor_type.empty()) {
        report.add_error("deep.actor.actor_type", "actor_type must not be empty");
    }
    if (profile.risk_tolerance > 100 || profile.shortage_response_threshold > 100) {
        report.add_error("deep.actor.range", "decision profile scores must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_deep_schema_version_registry(const DeepSchemaVersionRegistry& registry) {
    data::ValidationReport report;
    if (registry.data_pack_schema_version == 0 || registry.rules_schema_version == 0 || registry.effects_schema_version == 0
        || registry.dependency_schema_version == 0 || registry.market_schema_version == 0 || registry.scenario_schema_version == 0
        || registry.save_schema_version == 0 || registry.replay_schema_version == 0) {
        report.add_error("deep.schema.version", "schema versions must be greater than zero");
    }
    return report;
}

data::ValidationReport validate_deep_simulation_scenario_preset(const DeepSimulationScenarioPreset& preset) {
    data::ValidationReport report;
    if (preset.preset_id.empty()) {
        report.add_error("deep.scenario.preset_id", "preset_id must not be empty");
    }
    if (preset.display_name.empty()) {
        report.add_warning("deep.scenario.display_name", "display_name is empty");
    }
    for (const auto& feature_id : preset.enabled_feature_ids) {
        if (feature_id.empty()) {
            report.add_error("deep.scenario.enabled_feature_ids", "enabled feature ids must not be empty");
        }
    }
    for (const auto& stressor_id : preset.stressor_ids) {
        if (stressor_id.empty()) {
            report.add_error("deep.scenario.stressor_ids", "stressor ids must not be empty");
        }
    }
    return report;
}

PopulationNeedsReport evaluate_population_needs(const PopulationNeedsState& state) {
    const auto food = satisfaction(state.food_supply, state.population);
    const auto water = satisfaction(state.water_supply, state.population);
    const auto housing = satisfaction(state.housing_capacity, state.population);
    const auto baseline = (food + water + housing + bounded_severity(state.safety_score)) / 4;
    const auto tax_penalty = std::min<std::uint32_t>(baseline, state.tax_pressure / 2);
    const auto happiness = baseline - tax_penalty;
    const auto migration = happiness >= 70
        ? static_cast<std::int64_t>((state.population * (happiness - 69)) / 1000)
        : -static_cast<std::int64_t>((state.population * (70 - happiness)) / 700);
    PopulationNeedsReport report{
        .settlement_id = state.settlement_id,
        .food_satisfaction = food,
        .water_satisfaction = water,
        .housing_satisfaction = housing,
        .happiness_score = happiness,
        .migration_delta = migration,
    };
    add_if(report.reasons, food < 80, "food shortage reduces happiness");
    add_if(report.reasons, water < 80, "water shortage reduces happiness");
    add_if(report.reasons, housing < 80, "housing shortage caps growth");
    add_if(report.reasons, state.safety_score < 70, "low safety increases emigration pressure");
    add_if(report.reasons, state.tax_pressure > 50, "tax pressure reduces happiness");
    return report;
}

WeatherImpactReport evaluate_weather_impact(const WeatherClimateState& state) {
    const auto severity = bounded_severity(state.severity);
    WeatherImpactReport report{.scope_id = state.scope_id};
    if (state.weather_id == "storm" || state.weather_id == "heavy_rain" || state.weather_id == "blizzard") {
        report.route_risk_delta = severity;
        report.storage_decay_delta = severity / 3;
        report.market_pressure_delta = severity / 4;
        report.active_effects.push_back("severe weather affects logistics");
    }
    if (state.weather_id == "drought" || state.weather_id == "heatwave") {
        report.crop_yield_delta = -static_cast<std::int32_t>(severity);
        report.market_pressure_delta = bounded_add(report.market_pressure_delta, severity / 2);
        report.active_effects.push_back("dry weather affects agriculture");
    }
    if (state.weather_id == "clear" && state.season_id == "spring") {
        report.crop_yield_delta = 5;
        report.active_effects.push_back("season supports crop growth");
    }
    return report;
}

PolicyImpactReport evaluate_policy_impact(const PolicyRuleState& state) {
    PolicyImpactReport report{.policy_id = state.policy_id};
    if (!state.enabled) {
        report.side_effects.push_back("policy disabled");
        return report;
    }
    report.market_price_pressure = state.tax_rate / 2;
    report.production_delta = state.production_modifier;
    report.happiness_delta = state.happiness_modifier - static_cast<std::int32_t>(state.tax_rate / 3);
    add_if(report.side_effects, state.tax_rate > 0, "tax affects prices and happiness");
    add_if(report.side_effects, state.production_modifier != 0, "policy affects production");
    add_if(report.side_effects, state.remaining_ticks == 0, "policy has no remaining duration");
    return report;
}

EcologyReport evaluate_ecology_pressure(const EcologyState& state) {
    const auto pressure = std::min<std::uint32_t>(100, (state.extraction_pressure + state.pollution + (100 - state.fertility) + (100 - state.water_level)) / 4);
    EcologyReport report{
        .scope_id = state.scope_id,
        .yield_modifier = static_cast<std::int32_t>((state.fertility + state.water_level) / 2) - 100,
        .regeneration_delta = static_cast<std::int32_t>(state.regeneration_rate) - static_cast<std::int32_t>((state.extraction_pressure + state.pollution) / 2),
        .depletion_risk = pressure,
        .environmental_pressure = pressure,
    };
    add_if(report.warnings, state.water_level < 40, "low water level threatens agriculture");
    add_if(report.warnings, state.fertility < 40, "low fertility reduces yield");
    add_if(report.warnings, state.extraction_pressure > state.regeneration_rate, "extraction exceeds regeneration");
    add_if(report.warnings, state.pollution > 50, "pollution increases environmental pressure");
    return report;
}

CrisisPropagationReport evaluate_crisis_propagation(const WorldEventState& event) {
    const auto severity = bounded_severity(event.severity);
    CrisisPropagationReport report{.event_id = event.event_id};
    if (event.event_type == "food_crisis") {
        report.supply_pressure = severity;
        report.demand_pressure = severity / 2;
        report.stability_pressure = severity / 2;
        report.affected_systems = {"market", "population", "contracts"};
    } else if (event.event_type == "logistics_crisis") {
        report.supply_pressure = severity / 2;
        report.liquidity_pressure = severity;
        report.route_risk_pressure = severity;
        report.affected_systems = {"routes", "caravans", "market"};
    } else if (event.event_type == "security_crisis") {
        report.route_risk_pressure = severity;
        report.stability_pressure = severity;
        report.affected_systems = {"factions", "routes", "population"};
    } else {
        report.demand_pressure = severity / 3;
        report.stability_pressure = severity / 3;
        report.affected_systems = {"events"};
    }
    return report;
}

AutonomousDecisionReport choose_autonomous_decision(
    const AutonomousDecisionProfile& profile,
    const PopulationNeedsReport& population,
    const CrisisPropagationReport& crisis
) {
    AutonomousDecisionReport report{.actor_id = profile.actor_id};
    if (!profile.enabled) {
        report.selected_decision = "none";
        report.rejected_decision = "disabled";
        report.reason = "actor profile disabled";
        return report;
    }
    const auto shortage_pressure = std::max({100 - population.food_satisfaction, 100 - population.water_satisfaction, crisis.supply_pressure});
    if (shortage_pressure >= profile.shortage_response_threshold) {
        report.selected_decision = "issue_supply_contract";
        report.rejected_decision = "hold";
        report.reason = "shortage pressure exceeded response threshold";
        report.priority = shortage_pressure;
    } else if (crisis.route_risk_pressure > profile.risk_tolerance) {
        report.selected_decision = "reroute_caravan";
        report.rejected_decision = "continue_route";
        report.reason = "route risk exceeded tolerance";
        report.priority = crisis.route_risk_pressure;
    } else {
        report.selected_decision = "hold";
        report.reason = "no deterministic response threshold exceeded";
        report.priority = population.happiness_score;
    }
    return report;
}

std::string population_needs_digest(const PopulationNeedsReport& report) {
    std::ostringstream out;
    out << "population"
        << ";settlement=" << report.settlement_id
        << ";food=" << report.food_satisfaction
        << ";water=" << report.water_satisfaction
        << ";housing=" << report.housing_satisfaction
        << ";happiness=" << report.happiness_score
        << ";migration=" << report.migration_delta
        << ";reasons=" << report.reasons.size();
    return out.str();
}

std::string weather_impact_digest(const WeatherImpactReport& report) {
    std::ostringstream out;
    out << "weather"
        << ";scope=" << report.scope_id
        << ";route_risk=" << report.route_risk_delta
        << ";crop_yield=" << report.crop_yield_delta
        << ";storage_decay=" << report.storage_decay_delta
        << ";market_pressure=" << report.market_pressure_delta
        << ";effects=" << report.active_effects.size();
    return out.str();
}

std::string policy_impact_digest(const PolicyImpactReport& report) {
    std::ostringstream out;
    out << "policy"
        << ";policy=" << report.policy_id
        << ";market_pressure=" << report.market_price_pressure
        << ";production_delta=" << report.production_delta
        << ";happiness_delta=" << report.happiness_delta
        << ";side_effects=" << report.side_effects.size();
    return out.str();
}

std::string ecology_report_digest(const EcologyReport& report) {
    std::ostringstream out;
    out << "ecology"
        << ";scope=" << report.scope_id
        << ";yield_modifier=" << report.yield_modifier
        << ";regeneration_delta=" << report.regeneration_delta
        << ";depletion_risk=" << report.depletion_risk
        << ";pressure=" << report.environmental_pressure
        << ";warnings=" << report.warnings.size();
    return out.str();
}

std::string crisis_propagation_digest(const CrisisPropagationReport& report) {
    std::ostringstream out;
    out << "crisis"
        << ";event=" << report.event_id
        << ";supply=" << report.supply_pressure
        << ";demand=" << report.demand_pressure
        << ";liquidity=" << report.liquidity_pressure
        << ";route_risk=" << report.route_risk_pressure
        << ";stability=" << report.stability_pressure
        << ";systems=" << report.affected_systems.size();
    return out.str();
}

std::string autonomous_decision_digest(const AutonomousDecisionReport& report) {
    std::ostringstream out;
    out << "autonomous_decision"
        << ";actor=" << report.actor_id
        << ";selected=" << report.selected_decision
        << ";rejected=" << report.rejected_decision
        << ";priority=" << report.priority;
    return out.str();
}

std::string deep_schema_version_digest(const DeepSchemaVersionRegistry& registry) {
    std::ostringstream out;
    out << "deep_schema"
        << ";data_pack=" << registry.data_pack_schema_version
        << ";rules=" << registry.rules_schema_version
        << ";effects=" << registry.effects_schema_version
        << ";dependencies=" << registry.dependency_schema_version
        << ";market=" << registry.market_schema_version
        << ";scenario=" << registry.scenario_schema_version
        << ";save=" << registry.save_schema_version
        << ";replay=" << registry.replay_schema_version;
    return out.str();
}

} // namespace clc::sim
