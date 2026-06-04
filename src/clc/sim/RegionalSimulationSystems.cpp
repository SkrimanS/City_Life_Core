#include "clc/sim/RegionalSimulationSystems.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

std::uint32_t clamp100(std::uint64_t value) noexcept {
    return static_cast<std::uint32_t>(std::min<std::uint64_t>(100, value));
}

void add_if(std::vector<std::string>& values, bool condition, std::string value) {
    if (condition) {
        values.push_back(std::move(value));
    }
}

} // namespace

data::ValidationReport validate_regional_market_state(const RegionalMarketState& state) {
    data::ValidationReport report;
    if (state.region_id.empty()) {
        report.add_error("regional.market.region_id", "region_id must not be empty");
    }
    if (state.resource_id.empty()) {
        report.add_error("regional.market.resource_id", "resource_id must not be empty");
    }
    if (state.liquidity > 100 || state.volatility > 100 || state.market_depth > 100) {
        report.add_error("regional.market.range", "liquidity, volatility and market_depth must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_inter_region_route_state(const InterRegionRouteState& state) {
    data::ValidationReport report;
    if (state.route_id.empty()) {
        report.add_error("regional.route.route_id", "route_id must not be empty");
    }
    if (state.source_region_id.empty() || state.target_region_id.empty()) {
        report.add_error("regional.route.region_id", "source and target region ids must not be empty");
    }
    if (state.congestion > 100 || state.risk > 100) {
        report.add_error("regional.route.range", "congestion and risk must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_regional_climate_state(const RegionalClimateState& state) {
    data::ValidationReport report;
    if (state.region_id.empty()) {
        report.add_error("regional.climate.region_id", "region_id must not be empty");
    }
    if (state.climate_zone_id.empty()) {
        report.add_error("regional.climate.climate_zone_id", "climate_zone_id must not be empty");
    }
    if (state.weather_id.empty()) {
        report.add_error("regional.climate.weather_id", "weather_id must not be empty");
    }
    if (state.severity > 100 || state.disaster_risk > 100) {
        report.add_error("regional.climate.range", "severity and disaster_risk must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_regional_territory_state(const RegionalTerritoryState& state) {
    data::ValidationReport report;
    if (state.region_id.empty()) {
        report.add_error("regional.territory.region_id", "region_id must not be empty");
    }
    if (state.influence > 100 || state.law_security > 100 || state.conflict_pressure > 100) {
        report.add_error("regional.territory.range", "territory scores must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_regional_migration_state(const RegionalMigrationState& state) {
    data::ValidationReport report;
    if (state.source_region_id.empty() || state.target_region_id.empty()) {
        report.add_error("regional.migration.region_id", "source and target region ids must not be empty");
    }
    if (state.source_stability > 100 || state.target_stability > 100 || state.health_risk > 100) {
        report.add_error("regional.migration.range", "stability and health scores must be in range 0..100");
    }
    return report;
}

data::ValidationReport validate_regional_audit_event(const RegionalAuditEvent& event) {
    data::ValidationReport report;
    if (event.region_id.empty()) {
        report.add_error("regional.audit.region_id", "region_id must not be empty");
    }
    if (event.event_type.empty()) {
        report.add_error("regional.audit.event_type", "event_type must not be empty");
    }
    if (event.visibility_scope.empty()) {
        report.add_error("regional.audit.visibility_scope", "visibility_scope must not be empty");
    }
    return report;
}

data::ValidationReport validate_long_running_maintenance_state(const LongRunningMaintenanceState&) {
    return {};
}

RegionalMarketReport evaluate_regional_market(const RegionalMarketState& state) {
    const auto diff = static_cast<std::int64_t>(state.supply) - static_cast<std::int64_t>(state.demand);
    const auto shortage = diff < 0 ? static_cast<std::uint64_t>(-diff) : 0;
    const auto pressure_base = state.demand == 0 ? 0 : (shortage * 100) / std::max<std::uint64_t>(1, state.demand);
    RegionalMarketReport report{
        .region_id = state.region_id,
        .resource_id = state.resource_id,
        .shortage_surplus = diff,
        .price_pressure = clamp100(pressure_base + state.volatility / 2 + (100 - state.liquidity) / 3),
        .liquidity_risk = 100 - state.liquidity,
        .volatility_risk = state.volatility,
    };
    add_if(report.causes, diff < 0, "regional shortage increases price pressure");
    add_if(report.causes, diff > 0, "regional surplus reduces scarcity pressure");
    add_if(report.causes, state.liquidity < 40, "low liquidity increases manipulation risk");
    add_if(report.causes, state.volatility > 60, "high volatility increases instability");
    add_if(report.causes, state.market_depth < 40, "thin market depth increases sensitivity");
    return report;
}

InterRegionLogisticsReport evaluate_inter_region_logistics(const InterRegionRouteState& state) {
    const auto capacity_loss = state.blocked ? state.capacity : (state.capacity * state.congestion) / 100;
    InterRegionLogisticsReport report{
        .route_id = state.route_id,
        .effective_capacity = state.capacity > capacity_loss ? state.capacity - capacity_loss : 0,
        .travel_risk = clamp100(state.risk + state.congestion / 2 + (state.chokepoint ? 20 : 0) + (state.blocked ? 100 : 0)),
        .disruption_score = clamp100(state.congestion + state.risk + (state.chokepoint ? 25 : 0) + (state.blocked ? 100 : 0)),
    };
    add_if(report.bottlenecks, state.blocked, "route is blocked");
    add_if(report.bottlenecks, state.chokepoint, "route is a chokepoint");
    add_if(report.bottlenecks, state.congestion > 60, "route congestion limits capacity");
    add_if(report.bottlenecks, state.risk > 60, "route risk threatens deliveries");
    return report;
}

RegionalClimateReport evaluate_regional_climate(const RegionalClimateState& state) {
    RegionalClimateReport report{.region_id = state.region_id};
    if (state.weather_id == "drought" || state.weather_id == "heatwave") {
        report.crop_pressure = state.severity;
        report.market_pressure = state.severity / 2;
        report.affected_systems.push_back("agriculture");
        report.affected_systems.push_back("market");
    } else if (state.weather_id == "storm" || state.weather_id == "blizzard") {
        report.logistics_pressure = state.severity;
        report.market_pressure = state.severity / 3;
        report.affected_systems.push_back("logistics");
        report.affected_systems.push_back("market");
    }
    report.disaster_pressure = clamp100(state.disaster_risk + state.severity / 3);
    add_if(report.affected_systems, report.disaster_pressure > 50, "disasters");
    return report;
}

RegionalTerritoryReport evaluate_regional_territory(const RegionalTerritoryState& state) {
    RegionalTerritoryReport report{
        .region_id = state.region_id,
        .controlling_faction_id = state.controlling_faction_id,
        .route_control_pressure = clamp100(state.influence / 2 + state.conflict_pressure / 2),
        .market_control_pressure = clamp100(state.influence),
        .unrest_risk = clamp100((100 - state.law_security) + state.conflict_pressure),
    };
    add_if(report.effects, !state.controlling_faction_id.empty(), "faction controls regional systems");
    add_if(report.effects, state.conflict_pressure > 50, "conflict pressure affects routes and markets");
    add_if(report.effects, state.law_security < 50, "low security increases unrest");
    return report;
}

RegionalMigrationReport evaluate_regional_migration(const RegionalMigrationState& state) {
    const auto instability = state.source_stability >= state.target_stability ? 0 : state.target_stability - state.source_stability;
    const auto capacity = std::min<std::uint64_t>(state.target_capacity, state.source_population);
    const auto flow = (capacity * instability) / 1000;
    RegionalMigrationReport report{
        .source_region_id = state.source_region_id,
        .target_region_id = state.target_region_id,
        .migration_flow = static_cast<std::int64_t>(flow),
        .refugee_pressure = clamp100(instability + (state.source_stability < 35 ? 30 : 0)),
        .health_pressure = state.health_risk,
    };
    add_if(report.reasons, instability > 0, "target region is more stable than source");
    add_if(report.reasons, state.source_stability < 35, "source instability creates refugee pressure");
    add_if(report.reasons, state.health_risk > 50, "health risk affects migration safety");
    return report;
}

RegionalAuditStreamReport summarize_regional_audit_stream(
    const std::vector<RegionalAuditEvent>& events,
    std::string region_id
) {
    RegionalAuditStreamReport report{.region_id = std::move(region_id)};
    for (const auto& event : events) {
        if (event.region_id != report.region_id) {
            continue;
        }
        if (report.event_count == 0) {
            report.first_tick = event.tick;
        }
        report.last_tick = event.tick;
        ++report.event_count;
        if (event.visibility_scope == "private") {
            ++report.private_event_count;
        } else {
            ++report.public_event_count;
        }
    }
    return report;
}

LongRunningMaintenanceReport evaluate_long_running_maintenance(const LongRunningMaintenanceState& state) {
    const auto load_pressure = clamp100((state.region_count / 10) + (state.market_count / 50) + (state.route_count / 50) + (state.caravan_count / 100));
    const auto compaction_pressure = clamp100((state.event_count / 1000) + (state.ticks_since_compaction / 10000));
    const auto persistence_pressure = clamp100((state.snapshot_count == 0 ? 50 : 0) + (state.event_count / 2000));
    const auto total_pressure = clamp100(load_pressure / 3 + compaction_pressure / 3 + persistence_pressure / 3);
    LongRunningMaintenanceReport report{
        .world_health_score = 100 - total_pressure,
        .load_pressure = load_pressure,
        .compaction_pressure = compaction_pressure,
        .persistence_pressure = persistence_pressure,
    };
    add_if(report.recommended_actions, load_pressure > 60, "split hot regions or reduce active runtime load");
    add_if(report.recommended_actions, compaction_pressure > 60, "compact event streams");
    add_if(report.recommended_actions, persistence_pressure > 60, "write periodic snapshots");
    add_if(report.recommended_actions, report.world_health_score < 50, "schedule large-world maintenance window");
    return report;
}

std::string regional_market_digest(const RegionalMarketReport& report) {
    std::ostringstream out;
    out << "regional_market"
        << ";region=" << report.region_id
        << ";resource=" << report.resource_id
        << ";shortage_surplus=" << report.shortage_surplus
        << ";price_pressure=" << report.price_pressure
        << ";liquidity_risk=" << report.liquidity_risk
        << ";volatility_risk=" << report.volatility_risk
        << ";causes=" << report.causes.size();
    return out.str();
}

std::string inter_region_logistics_digest(const InterRegionLogisticsReport& report) {
    std::ostringstream out;
    out << "inter_region_logistics"
        << ";route=" << report.route_id
        << ";capacity=" << report.effective_capacity
        << ";risk=" << report.travel_risk
        << ";disruption=" << report.disruption_score
        << ";bottlenecks=" << report.bottlenecks.size();
    return out.str();
}

std::string regional_climate_digest(const RegionalClimateReport& report) {
    std::ostringstream out;
    out << "regional_climate"
        << ";region=" << report.region_id
        << ";crop=" << report.crop_pressure
        << ";logistics=" << report.logistics_pressure
        << ";market=" << report.market_pressure
        << ";disaster=" << report.disaster_pressure
        << ";systems=" << report.affected_systems.size();
    return out.str();
}

std::string regional_territory_digest(const RegionalTerritoryReport& report) {
    std::ostringstream out;
    out << "regional_territory"
        << ";region=" << report.region_id
        << ";faction=" << report.controlling_faction_id
        << ";route_control=" << report.route_control_pressure
        << ";market_control=" << report.market_control_pressure
        << ";unrest=" << report.unrest_risk
        << ";effects=" << report.effects.size();
    return out.str();
}

std::string regional_migration_digest(const RegionalMigrationReport& report) {
    std::ostringstream out;
    out << "regional_migration"
        << ";source=" << report.source_region_id
        << ";target=" << report.target_region_id
        << ";flow=" << report.migration_flow
        << ";refugee_pressure=" << report.refugee_pressure
        << ";health_pressure=" << report.health_pressure
        << ";reasons=" << report.reasons.size();
    return out.str();
}

std::string regional_audit_stream_digest(const RegionalAuditStreamReport& report) {
    std::ostringstream out;
    out << "regional_audit"
        << ";region=" << report.region_id
        << ";events=" << report.event_count
        << ";public=" << report.public_event_count
        << ";private=" << report.private_event_count
        << ";first_tick=" << report.first_tick
        << ";last_tick=" << report.last_tick;
    return out.str();
}

std::string long_running_maintenance_digest(const LongRunningMaintenanceReport& report) {
    std::ostringstream out;
    out << "long_running_maintenance"
        << ";health=" << report.world_health_score
        << ";load=" << report.load_pressure
        << ";compaction=" << report.compaction_pressure
        << ";persistence=" << report.persistence_pressure
        << ";actions=" << report.recommended_actions.size();
    return out.str();
}

} // namespace clc::sim
