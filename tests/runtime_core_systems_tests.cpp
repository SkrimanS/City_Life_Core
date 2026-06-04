#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "basic runtime should bootstrap");

    const auto report = clc::sim::evaluate_runtime_core_systems(bootstrap.runtime, {
        .region_id = "north",
        .climate_zone_id = "temperate",
        .weather_id = "drought",
        .season_id = "summer",
        .weather_severity = 70,
        .disaster_risk = 20,
        .market_liquidity = 50,
        .market_volatility = 40,
        .market_depth = 60,
    });
    require(!report.population.settlement_id.empty(), "runtime report should include settlement id");
    require(report.weather.market_pressure_delta > 0, "drought should create market pressure");
    require(report.regional_market.region_id == "north", "regional market should use configured region");
    require(report.diagnostic_count > 0, "runtime report should count diagnostics");
    require(clc::sim::runtime_core_systems_digest(report).find("runtime_core_systems") != std::string::npos, "runtime digest should be stable");
    require(clc::sim::runtime_core_systems_markdown(report).find("Runtime Core Systems") != std::string::npos, "runtime markdown should render");

    return 0;
}
