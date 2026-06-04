#include "clc/sim/RuntimeCoreSystems.hpp"

#include <algorithm>
#include <sstream>
#include <string>

namespace clc::sim {
namespace {

const SettlementState* first_settlement(const SimulationRuntime& runtime) noexcept {
    const auto& settlements = runtime.engine.settlements();
    return settlements.empty() ? nullptr : &settlements.front();
}

std::uint64_t total_market_supply(const SimulationRuntime& runtime, std::string_view resource_id) {
    std::uint64_t total = 0;
    for (const auto& settlement : runtime.engine.settlements()) {
        total += settlement.storage.amount(resource_id);
    }
    return total;
}

} // namespace

RuntimeCoreSystemsConfig make_default_runtime_core_systems_config() {
    return {};
}

RuntimeCoreSystemsReport evaluate_runtime_core_systems(
    const SimulationRuntime& runtime,
    const RuntimeCoreSystemsConfig& config
) {
    const auto* settlement = first_settlement(runtime);
    const auto settlement_id = settlement == nullptr ? std::string{"none"} : settlement->id;
    const auto population = settlement == nullptr ? 0 : settlement->population;
    const auto grain = settlement == nullptr ? 0 : settlement->storage.amount("grain");
    const auto water = settlement == nullptr ? 0 : settlement->storage.amount("water");
    const auto housing = population + ((population * config.default_housing_buffer) / 100);

    RuntimeCoreSystemsReport report;
    report.population = evaluate_population_needs({
        .settlement_id = settlement_id,
        .population = population,
        .workforce = population / 2,
        .housing_capacity = housing,
        .food_supply = grain,
        .water_supply = water == 0 ? grain : water,
        .safety_score = config.default_safety_score,
        .tax_pressure = config.default_tax_pressure,
    });
    report.weather = evaluate_weather_impact({
        .scope_id = settlement_id,
        .weather_id = config.weather_id,
        .season_id = config.season_id,
        .severity = config.weather_severity,
        .remaining_ticks = 0,
    });
    report.policy = evaluate_policy_impact({
        .policy_id = "default_policy",
        .scope_id = settlement_id,
        .tax_rate = config.default_tax_pressure,
        .remaining_ticks = 0,
    });
    report.ecology = evaluate_ecology_pressure({
        .scope_id = settlement_id,
        .fertility = 80,
        .water_level = water == 0 ? 50u : 80u,
        .regeneration_rate = 20,
        .extraction_pressure = 10,
        .pollution = 5,
    });
    report.crisis = evaluate_crisis_propagation({
        .event_id = "runtime_pressure",
        .event_type = grain < population ? "food_crisis" : "stable",
        .scope_id = settlement_id,
        .severity = grain < population ? 60u : 10u,
        .remaining_ticks = 0,
    });
    report.decision = choose_autonomous_decision({
        .actor_id = "runtime_actor",
        .actor_type = "settlement_processor",
        .risk_tolerance = 50,
        .shortage_response_threshold = 50,
    }, report.population, report.crisis);
    report.regional_market = evaluate_regional_market({
        .region_id = config.region_id,
        .resource_id = "grain",
        .supply = total_market_supply(runtime, "grain"),
        .demand = population,
        .liquidity = config.market_liquidity,
        .volatility = config.market_volatility,
        .market_depth = config.market_depth,
    });
    report.regional_climate = evaluate_regional_climate({
        .region_id = config.region_id,
        .climate_zone_id = config.climate_zone_id,
        .weather_id = config.weather_id,
        .severity = config.weather_severity,
        .disaster_risk = config.disaster_risk,
    });
    report.regional_migration = evaluate_regional_migration({
        .source_region_id = config.region_id,
        .target_region_id = config.region_id,
        .source_population = population,
        .target_capacity = housing,
        .source_stability = report.population.happiness_score,
        .target_stability = std::max<std::uint32_t>(report.population.happiness_score, 70),
        .health_risk = report.regional_climate.disaster_pressure,
    });
    report.maintenance = evaluate_long_running_maintenance({
        .region_count = 1,
        .market_count = 1,
        .route_count = runtime.routes.routes.size(),
        .caravan_count = runtime.caravans.caravans.size(),
        .faction_count = runtime.factions.factions.size(),
        .event_count = runtime.engine.events().size(),
        .snapshot_count = 1,
        .ticks_since_compaction = runtime.time.current_tick(),
    });
    report.diagnostic_count =
        report.population.reasons.size()
        + report.weather.active_effects.size()
        + report.policy.side_effects.size()
        + report.ecology.warnings.size()
        + report.crisis.affected_systems.size()
        + report.regional_market.causes.size()
        + report.regional_climate.affected_systems.size()
        + report.regional_migration.reasons.size()
        + report.maintenance.recommended_actions.size();
    return report;
}

std::string runtime_core_systems_digest(const RuntimeCoreSystemsReport& report) {
    std::ostringstream out;
    out << "runtime_core_systems"
        << ";settlement=" << report.population.settlement_id
        << ";happiness=" << report.population.happiness_score
        << ";decision=" << report.decision.selected_decision
        << ";regional_price_pressure=" << report.regional_market.price_pressure
        << ";maintenance_health=" << report.maintenance.world_health_score
        << ";diagnostics=" << report.diagnostic_count;
    return out.str();
}

std::string runtime_core_systems_markdown(const RuntimeCoreSystemsReport& report) {
    std::ostringstream out;
    out << "# Runtime Core Systems\n\n";
    out << "- Digest: `" << runtime_core_systems_digest(report) << "`\n";
    out << "- Population: `" << population_needs_digest(report.population) << "`\n";
    out << "- Weather: `" << weather_impact_digest(report.weather) << "`\n";
    out << "- Regional market: `" << regional_market_digest(report.regional_market) << "`\n";
    out << "- Maintenance: `" << long_running_maintenance_digest(report.maintenance) << "`\n";
    return out.str();
}

} // namespace clc::sim
