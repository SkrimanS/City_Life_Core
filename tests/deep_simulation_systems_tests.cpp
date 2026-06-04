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
    const clc::sim::PopulationNeedsState population_state{
        .settlement_id = "riverwatch",
        .population = 1000,
        .workforce = 600,
        .housing_capacity = 900,
        .food_supply = 500,
        .water_supply = 1000,
        .safety_score = 60,
        .tax_pressure = 30,
    };
    require(clc::sim::validate_population_needs_state(population_state).ok(), "population state should validate");
    const auto population = clc::sim::evaluate_population_needs(population_state);
    require(population.food_satisfaction == 50, "food satisfaction should reflect shortage");
    require(population.happiness_score < 80, "happiness should include shortages");
    require(population.migration_delta < 0, "low happiness should produce emigration");
    require(clc::sim::population_needs_digest(population).find("settlement=riverwatch") != std::string::npos, "population digest should include settlement");

    const clc::sim::WeatherClimateState weather_state{
        .scope_id = "riverwatch",
        .weather_id = "drought",
        .season_id = "summer",
        .severity = 80,
        .remaining_ticks = 120,
    };
    require(clc::sim::validate_weather_climate_state(weather_state).ok(), "weather state should validate");
    const auto weather = clc::sim::evaluate_weather_impact(weather_state);
    require(weather.crop_yield_delta < 0, "drought should reduce crop yield");
    require(weather.market_pressure_delta > 0, "drought should affect market pressure");

    const auto policy = clc::sim::evaluate_policy_impact({
        .policy_id = "grain_tax",
        .owner_faction_id = "riverwatch",
        .scope_id = "riverwatch",
        .tax_rate = 30,
        .production_modifier = 5,
        .happiness_modifier = -2,
        .remaining_ticks = 60,
    });
    require(policy.market_price_pressure > 0, "tax policy should add market pressure");
    require(policy.happiness_delta < 0, "tax policy should reduce happiness");

    const auto ecology = clc::sim::evaluate_ecology_pressure({
        .scope_id = "riverwatch",
        .fertility = 35,
        .water_level = 30,
        .regeneration_rate = 10,
        .extraction_pressure = 80,
        .pollution = 20,
    });
    require(ecology.depletion_risk > 0, "ecology pressure should produce depletion risk");
    require(!ecology.warnings.empty(), "ecology pressure should produce warnings");

    const auto crisis = clc::sim::evaluate_crisis_propagation({
        .event_id = "food_crisis_1",
        .event_type = "food_crisis",
        .scope_id = "riverwatch",
        .severity = 90,
        .remaining_ticks = 180,
    });
    require(crisis.supply_pressure == 90, "food crisis should create supply pressure");
    require(crisis.affected_systems.size() == 3, "food crisis should name affected systems");

    const auto decision = clc::sim::choose_autonomous_decision({
        .actor_id = "faction_a",
        .actor_type = "faction",
        .risk_tolerance = 40,
        .shortage_response_threshold = 50,
    }, population, crisis);
    require(decision.selected_decision == "issue_supply_contract", "actor should respond to shortage");
    require(clc::sim::autonomous_decision_digest(decision).find("selected=issue_supply_contract") != std::string::npos, "decision digest should include selected decision");

    const clc::sim::DeepSchemaVersionRegistry schemas{
        .data_pack_schema_version = 2,
        .rules_schema_version = 2,
        .effects_schema_version = 2,
        .dependency_schema_version = 2,
        .market_schema_version = 2,
        .scenario_schema_version = 2,
        .save_schema_version = 2,
        .replay_schema_version = 2,
    };
    require(clc::sim::validate_deep_schema_version_registry(schemas).ok(), "schema versions should validate");
    require(clc::sim::deep_schema_version_digest(schemas).find("replay=2") != std::string::npos, "schema digest should include replay version");

    require(clc::sim::validate_deep_simulation_scenario_preset({
        .preset_id = "drought_food_crisis",
        .display_name = "Drought Food Crisis",
        .enabled_feature_ids = {"weather", "market.depth", "population"},
        .stressor_ids = {"drought", "food_crisis"},
    }).ok(), "deep scenario preset should validate");

    return 0;
}
