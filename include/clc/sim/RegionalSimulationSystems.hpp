#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct RegionalMarketState final {
    std::string region_id{};
    std::string resource_id{};
    std::uint64_t supply{0};
    std::uint64_t demand{0};
    std::uint32_t liquidity{100};
    std::uint32_t volatility{0};
    std::uint32_t market_depth{100};
};

struct RegionalMarketReport final {
    std::string region_id{};
    std::string resource_id{};
    std::int64_t shortage_surplus{0};
    std::uint32_t price_pressure{0};
    std::uint32_t liquidity_risk{0};
    std::uint32_t volatility_risk{0};
    std::vector<std::string> causes{};
};

struct InterRegionRouteState final {
    std::string route_id{};
    std::string source_region_id{};
    std::string target_region_id{};
    std::uint32_t distance{0};
    std::uint32_t capacity{0};
    std::uint32_t congestion{0};
    std::uint32_t risk{0};
    bool chokepoint{false};
    bool blocked{false};
};

struct InterRegionLogisticsReport final {
    std::string route_id{};
    std::uint32_t effective_capacity{0};
    std::uint32_t travel_risk{0};
    std::uint32_t disruption_score{0};
    std::vector<std::string> bottlenecks{};
};

struct RegionalClimateState final {
    std::string region_id{};
    std::string climate_zone_id{};
    std::string weather_id{"clear"};
    std::uint32_t severity{0};
    std::uint32_t disaster_risk{0};
};

struct RegionalClimateReport final {
    std::string region_id{};
    std::uint32_t crop_pressure{0};
    std::uint32_t logistics_pressure{0};
    std::uint32_t market_pressure{0};
    std::uint32_t disaster_pressure{0};
    std::vector<std::string> affected_systems{};
};

struct RegionalTerritoryState final {
    std::string region_id{};
    std::string controlling_faction_id{};
    std::uint32_t influence{0};
    std::uint32_t law_security{100};
    std::uint32_t conflict_pressure{0};
};

struct RegionalTerritoryReport final {
    std::string region_id{};
    std::string controlling_faction_id{};
    std::uint32_t route_control_pressure{0};
    std::uint32_t market_control_pressure{0};
    std::uint32_t unrest_risk{0};
    std::vector<std::string> effects{};
};

struct RegionalMigrationState final {
    std::string source_region_id{};
    std::string target_region_id{};
    std::uint64_t source_population{0};
    std::uint64_t target_capacity{0};
    std::uint32_t source_stability{100};
    std::uint32_t target_stability{100};
    std::uint32_t health_risk{0};
};

struct RegionalMigrationReport final {
    std::string source_region_id{};
    std::string target_region_id{};
    std::int64_t migration_flow{0};
    std::uint32_t refugee_pressure{0};
    std::uint32_t health_pressure{0};
    std::vector<std::string> reasons{};
};

struct RegionalAuditEvent final {
    std::uint64_t tick{0};
    std::string region_id{};
    std::string owner_id{};
    std::string visibility_scope{"public"};
    std::string event_type{};
};

struct RegionalAuditStreamReport final {
    std::string region_id{};
    std::uint64_t event_count{0};
    std::uint64_t public_event_count{0};
    std::uint64_t private_event_count{0};
    std::uint64_t first_tick{0};
    std::uint64_t last_tick{0};
};

struct LongRunningMaintenanceState final {
    std::uint64_t region_count{0};
    std::uint64_t market_count{0};
    std::uint64_t route_count{0};
    std::uint64_t caravan_count{0};
    std::uint64_t faction_count{0};
    std::uint64_t event_count{0};
    std::uint64_t snapshot_count{0};
    std::uint64_t ticks_since_compaction{0};
};

struct LongRunningMaintenanceReport final {
    std::uint32_t world_health_score{100};
    std::uint32_t load_pressure{0};
    std::uint32_t compaction_pressure{0};
    std::uint32_t persistence_pressure{0};
    std::vector<std::string> recommended_actions{};
};

[[nodiscard]] data::ValidationReport validate_regional_market_state(const RegionalMarketState& state);
[[nodiscard]] data::ValidationReport validate_inter_region_route_state(const InterRegionRouteState& state);
[[nodiscard]] data::ValidationReport validate_regional_climate_state(const RegionalClimateState& state);
[[nodiscard]] data::ValidationReport validate_regional_territory_state(const RegionalTerritoryState& state);
[[nodiscard]] data::ValidationReport validate_regional_migration_state(const RegionalMigrationState& state);
[[nodiscard]] data::ValidationReport validate_regional_audit_event(const RegionalAuditEvent& event);
[[nodiscard]] data::ValidationReport validate_long_running_maintenance_state(const LongRunningMaintenanceState& state);

[[nodiscard]] RegionalMarketReport evaluate_regional_market(const RegionalMarketState& state);
[[nodiscard]] InterRegionLogisticsReport evaluate_inter_region_logistics(const InterRegionRouteState& state);
[[nodiscard]] RegionalClimateReport evaluate_regional_climate(const RegionalClimateState& state);
[[nodiscard]] RegionalTerritoryReport evaluate_regional_territory(const RegionalTerritoryState& state);
[[nodiscard]] RegionalMigrationReport evaluate_regional_migration(const RegionalMigrationState& state);
[[nodiscard]] RegionalAuditStreamReport summarize_regional_audit_stream(
    const std::vector<RegionalAuditEvent>& events,
    std::string region_id
);
[[nodiscard]] LongRunningMaintenanceReport evaluate_long_running_maintenance(const LongRunningMaintenanceState& state);

[[nodiscard]] std::string regional_market_digest(const RegionalMarketReport& report);
[[nodiscard]] std::string inter_region_logistics_digest(const InterRegionLogisticsReport& report);
[[nodiscard]] std::string regional_climate_digest(const RegionalClimateReport& report);
[[nodiscard]] std::string regional_territory_digest(const RegionalTerritoryReport& report);
[[nodiscard]] std::string regional_migration_digest(const RegionalMigrationReport& report);
[[nodiscard]] std::string regional_audit_stream_digest(const RegionalAuditStreamReport& report);
[[nodiscard]] std::string long_running_maintenance_digest(const LongRunningMaintenanceReport& report);

} // namespace clc::sim
