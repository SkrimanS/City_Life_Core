#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    const clc::sim::RegionalMarketState market_state{
        .region_id = "north",
        .resource_id = "grain",
        .supply = 100,
        .demand = 250,
        .liquidity = 25,
        .volatility = 70,
        .market_depth = 30,
    };
    require(clc::sim::validate_regional_market_state(market_state).ok(), "regional market should validate");
    const auto market = clc::sim::evaluate_regional_market(market_state);
    require(market.shortage_surplus < 0, "regional market should detect shortage");
    require(market.price_pressure > 0, "regional market should create price pressure");
    require(!market.causes.empty(), "regional market should explain causes");

    const auto logistics = clc::sim::evaluate_inter_region_logistics({
        .route_id = "north-south",
        .source_region_id = "north",
        .target_region_id = "south",
        .distance = 120,
        .capacity = 1000,
        .congestion = 70,
        .risk = 40,
        .chokepoint = true,
    });
    require(logistics.effective_capacity < 1000, "congestion should reduce capacity");
    require(logistics.disruption_score > 0, "route should report disruption");

    const auto climate = clc::sim::evaluate_regional_climate({
        .region_id = "north",
        .climate_zone_id = "continental",
        .weather_id = "storm",
        .severity = 75,
        .disaster_risk = 30,
    });
    require(climate.logistics_pressure > 0, "storm should affect logistics");
    require(climate.disaster_pressure > 0, "regional climate should report disaster pressure");

    const auto territory = clc::sim::evaluate_regional_territory({
        .region_id = "north",
        .controlling_faction_id = "guild_a",
        .influence = 80,
        .law_security = 35,
        .conflict_pressure = 50,
    });
    require(territory.market_control_pressure == 80, "influence should affect market control");
    require(territory.unrest_risk > 0, "low security should affect unrest");

    const auto migration = clc::sim::evaluate_regional_migration({
        .source_region_id = "north",
        .target_region_id = "south",
        .source_population = 10000,
        .target_capacity = 3000,
        .source_stability = 20,
        .target_stability = 80,
        .health_risk = 45,
    });
    require(migration.migration_flow > 0, "stability gap should create migration");
    require(migration.refugee_pressure > 0, "low source stability should create refugee pressure");

    const std::vector<clc::sim::RegionalAuditEvent> events{
        {.tick = 10, .region_id = "north", .owner_id = "guild_a", .visibility_scope = "public", .event_type = "market"},
        {.tick = 20, .region_id = "north", .owner_id = "guild_a", .visibility_scope = "private", .event_type = "policy"},
        {.tick = 30, .region_id = "south", .owner_id = "guild_b", .visibility_scope = "public", .event_type = "market"},
    };
    const auto audit = clc::sim::summarize_regional_audit_stream(events, "north");
    require(audit.event_count == 2, "audit stream should filter region");
    require(audit.private_event_count == 1, "audit stream should count private events");

    const auto maintenance = clc::sim::evaluate_long_running_maintenance({
        .region_count = 200,
        .market_count = 1000,
        .route_count = 1000,
        .caravan_count = 5000,
        .faction_count = 100,
        .event_count = 90000,
        .snapshot_count = 0,
        .ticks_since_compaction = 1000000,
    });
    require(maintenance.world_health_score < 100, "maintenance pressure should reduce world health");
    require(!maintenance.recommended_actions.empty(), "maintenance should recommend actions");
    require(clc::sim::long_running_maintenance_digest(maintenance).find("health=") != std::string::npos, "maintenance digest should include health");

    return 0;
}
