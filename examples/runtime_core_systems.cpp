#include "clc/CityLifeCore.hpp"

#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        std::cerr << "failed to bootstrap runtime\n";
        return 1;
    }

    const auto report = clc::sim::evaluate_runtime_core_systems(bootstrap.runtime, {
        .region_id = "demo_region",
        .weather_id = "drought",
        .season_id = "summer",
        .weather_severity = 50,
        .market_liquidity = 60,
        .market_volatility = 30,
    });

    std::cout << clc::sim::runtime_core_systems_digest(report) << '\n';
    std::cout << clc::sim::runtime_core_systems_markdown(report) << '\n';
    return 0;
}
