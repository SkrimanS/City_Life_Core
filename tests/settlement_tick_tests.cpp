#include "clc/data/DataRegistry.hpp"
#include "clc/sim/Settlement.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "grain should register");
    require(registry.add(clc::data::ResourceDefinition{.id = "wood", .display_name = "Wood", .category = "construction", .base_value = 6}).ok(), "wood should register");
    require(registry.add(clc::data::ProfessionDefinition{.id = "farmer", .display_name = "Farmer", .category = "production"}).ok(), "farmer should register");
    require(registry.add(clc::data::BuildingDefinition{
        .id = "farm",
        .display_name = "Farm",
        .category = "production",
        .worker_slots = 4,
        .required_profession_id = "farmer",
        .input_resource_ids = {"wood"},
        .output_resource_ids = {"grain"},
    }).ok(), "farm should register");
    return registry;
}

clc::sim::SettlementState make_settlement(const clc::data::DataRegistry& registry) {
    clc::sim::SettlementState settlement{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .population = 40,
    };
    require(settlement.storage.add("grain", 20).ok(), "settlement should receive grain");
    require(settlement.storage.add("wood", 20).ok(), "settlement should receive wood");
    require(clc::sim::add_building(settlement, registry, clc::sim::BuildingInstance{.definition_id = "farm", .assigned_workers = 4}).ok(), "settlement should add farm");
    return settlement;
}

} // namespace

int main() {
    const auto registry = make_registry();

    auto half_day_settlement = make_settlement(registry);
    const auto half_day = clc::sim::advance_settlement_ticks(half_day_settlement, registry, clc::hours_to_ticks(12));
    require(half_day.settlement_id == "riverwatch", "half-day report should expose settlement id");
    require(half_day.elapsed_ticks == clc::hours_to_ticks(12), "half-day report should expose elapsed ticks");
    require(half_day.consumed_food == 2, "half-day tick should consume half daily food");
    require(half_day.consumed_inputs == 2, "half-day tick should consume half daily inputs");
    require(half_day.produced_resources == 2, "half-day tick should produce half daily output");
    require(half_day.active_buildings == 1, "half-day tick should run active building");
    require(half_day.skipped_buildings == 0, "half-day tick should not skip active building");
    require(half_day.warnings.empty(), "half-day tick should not warn when storage is sufficient");
    require(half_day_settlement.storage.amount("grain") == 20, "half-day grain should consume 2 and produce 2");
    require(half_day_settlement.storage.amount("wood") == 18, "half-day wood should consume 2");

    const auto second_half = clc::sim::advance_settlement_ticks(half_day_settlement, registry, clc::hours_to_ticks(12));
    require(second_half.consumed_food == 2, "second half-day should consume remaining half daily food");
    require(second_half.consumed_inputs == 2, "second half-day should consume remaining half daily inputs");
    require(second_half.produced_resources == 2, "second half-day should produce remaining half daily output");
    require(half_day_settlement.storage.amount("grain") == 20, "two half-days should match full-day grain net effect");
    require(half_day_settlement.storage.amount("wood") == 16, "two half-days should match full-day wood consumption");

    auto full_day_settlement = make_settlement(registry);
    const auto full_day = clc::sim::advance_settlement_day(full_day_settlement, registry);
    require(full_day.elapsed_ticks == clc::ticks_per_day(), "day wrapper should expose one day of elapsed ticks");
    require(full_day.consumed_food == 4, "day wrapper should preserve daily food consumption");
    require(full_day.consumed_inputs == 4, "day wrapper should preserve daily input consumption");
    require(full_day.produced_resources == 4, "day wrapper should preserve daily production");
    require(full_day_settlement.storage.amount("grain") == half_day_settlement.storage.amount("grain"), "two half-days should equal one day for grain");
    require(full_day_settlement.storage.amount("wood") == half_day_settlement.storage.amount("wood"), "two half-days should equal one day for wood");

    auto hourly_settlement = make_settlement(registry);
    for (int hour = 0; hour < 5; ++hour) {
        const auto report = clc::sim::advance_settlement_ticks(hourly_settlement, registry, clc::hours_to_ticks(1));
        require(report.elapsed_ticks == clc::hours_to_ticks(1), "hourly report should expose elapsed ticks");
        require(report.consumed_food == 0, "early hourly ticks should accumulate food without whole-unit consumption");
        require(report.consumed_inputs == 0, "early hourly ticks should accumulate inputs without whole-unit consumption");
        require(report.produced_resources == 0, "early hourly ticks should accumulate output without whole-unit production");
        require(report.skipped_buildings == 0, "hourly accumulation should not skip buildings");
        require(report.warnings.empty(), "hourly accumulation should not warn while waiting for whole units");
    }
    const auto sixth_hour = clc::sim::advance_settlement_ticks(hourly_settlement, registry, clc::hours_to_ticks(1));
    require(sixth_hour.consumed_food == 1, "six hourly ticks should produce one whole food consumption unit");
    require(sixth_hour.consumed_inputs == 1, "six hourly ticks should produce one whole input consumption unit");
    require(sixth_hour.produced_resources == 1, "six hourly ticks should produce one whole output unit");
    require(sixth_hour.active_buildings == 1, "sixth hourly tick should run building when accumulated units mature");
    require(hourly_settlement.storage.amount("grain") == 20, "six hourly ticks should consume one grain and produce one grain");
    require(hourly_settlement.storage.amount("wood") == 19, "six hourly ticks should consume one wood input");
    require(!hourly_settlement.tick_remainders.empty(), "hourly settlement should keep deterministic tick remainders");

    auto zero_tick_settlement = make_settlement(registry);
    const auto zero = clc::sim::advance_settlement_ticks(zero_tick_settlement, registry, 0);
    require(zero.elapsed_ticks == 0, "zero tick report should expose zero elapsed ticks");
    require(zero.consumed_food == 0, "zero tick should not consume food");
    require(zero.consumed_inputs == 0, "zero tick should not consume inputs");
    require(zero.produced_resources == 0, "zero tick should not produce resources");

    return 0;
}
