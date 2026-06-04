#pragma once

#include "clc/sim/DeepSimulationSystems.hpp"
#include "clc/sim/RegionalSimulationSystems.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>

namespace clc::sim {

struct RuntimeCoreSystemsConfig final {
    std::string region_id{"default"};
    std::string climate_zone_id{"temperate"};
    std::string weather_id{"clear"};
    std::string season_id{"spring"};
    std::uint32_t weather_severity{0};
    std::uint32_t disaster_risk{0};
    std::uint32_t default_housing_buffer{10};
    std::uint32_t default_safety_score{80};
    std::uint32_t default_tax_pressure{10};
    std::uint32_t market_liquidity{80};
    std::uint32_t market_volatility{20};
    std::uint32_t market_depth{80};
};

struct RuntimeCoreSystemsReport final {
    PopulationNeedsReport population{};
    WeatherImpactReport weather{};
    PolicyImpactReport policy{};
    EcologyReport ecology{};
    CrisisPropagationReport crisis{};
    AutonomousDecisionReport decision{};
    RegionalMarketReport regional_market{};
    RegionalClimateReport regional_climate{};
    RegionalMigrationReport regional_migration{};
    LongRunningMaintenanceReport maintenance{};
    std::uint64_t diagnostic_count{0};
};

[[nodiscard]] RuntimeCoreSystemsConfig make_default_runtime_core_systems_config();
[[nodiscard]] RuntimeCoreSystemsReport evaluate_runtime_core_systems(
    const SimulationRuntime& runtime,
    const RuntimeCoreSystemsConfig& config = make_default_runtime_core_systems_config()
);
[[nodiscard]] std::string runtime_core_systems_digest(const RuntimeCoreSystemsReport& report);
[[nodiscard]] std::string runtime_core_systems_markdown(const RuntimeCoreSystemsReport& report);

} // namespace clc::sim
