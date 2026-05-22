#include "clc/data/DataRegistry.hpp"
#include "clc/sim/SimulationEngine.hpp"
#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace {

using Clock = std::chrono::steady_clock;

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Benchmark setup failed: " << message << '\n';
        std::exit(1);
    }
}

std::uint64_t elapsed_ms(Clock::time_point start, Clock::time_point end) {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

void print_metric(std::string_view name, std::uint64_t elapsed, std::string_view detail, std::uint64_t value) {
    std::cout << name << ",elapsed_ms," << elapsed << ',' << detail << ',' << value << '\n';
}

clc::data::DataRegistry make_registry(std::size_t settlement_count) {
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

    for (std::size_t index = 0; index < settlement_count; ++index) {
        const auto id = "settlement_" + std::to_string(index);
        require(registry.add(clc::data::SettlementDefinition{
            .id = id,
            .display_name = "Settlement " + std::to_string(index),
            .starting_population = 40,
        }).ok(), "settlement should register");
    }

    require(registry.validate_references().ok(), "registry references should validate");
    return registry;
}

clc::sim::SimulationEngine make_engine(std::size_t settlement_count) {
    auto registry = make_registry(settlement_count);
    clc::sim::SimulationEngine engine{std::move(registry)};
    require(engine.market().set_demand("grain", settlement_count * 20).ok(), "market demand should set");

    for (std::size_t index = 0; index < settlement_count; ++index) {
        const auto id = "settlement_" + std::to_string(index);
        require(engine.create_settlement(id).ok(), "settlement should create");
        require(engine.add_building_to_settlement(id, clc::sim::BuildingInstance{.definition_id = "farm", .assigned_workers = 4}).ok(), "farm should add");
        require(engine.add_resource_to_settlement(id, "grain", 1000).ok(), "grain should add");
        require(engine.add_resource_to_settlement(id, "wood", 1000).ok(), "wood should add");
    }

    return engine;
}

void benchmark_engine_advance_days() {
    constexpr std::size_t settlement_count = 100;
    constexpr std::uint64_t days = 365;
    auto engine = make_engine(settlement_count);

    const auto start = Clock::now();
    for (std::uint64_t day = 0; day < days; ++day) {
        const auto report = engine.advance_day();
        require(report.settlements.size() == settlement_count, "day report settlement count should match");
    }
    const auto end = Clock::now();

    print_metric("engine_advance_day_100_settlements_365_days", elapsed_ms(start, end), "events", engine.events().size());
}

void benchmark_engine_run_scenario_reports() {
    constexpr std::size_t settlement_count = 25;
    constexpr std::uint64_t days = 90;
    auto engine = make_engine(settlement_count);

    const auto start = Clock::now();
    const auto result = engine.run_scenario(days);
    const auto end = Clock::now();

    require(result.reports.size() == days, "scenario should produce one report per day");
    print_metric("engine_run_scenario_25_settlements_90_days", elapsed_ms(start, end), "reports", result.reports.size());
}

void benchmark_world_state_serialization() {
    auto engine = make_engine(100);
    for (std::uint64_t day = 0; day < 30; ++day) {
        (void)engine.advance_day();
    }

    clc::sim::SimulationWorldState state{};
    state.engine = engine.export_state();

    const auto serialize_start = Clock::now();
    const auto serialized = clc::sim::serialize_simulation_world_state(state);
    const auto serialize_end = Clock::now();
    print_metric("world_state_serialize_100_settlements_30_days", elapsed_ms(serialize_start, serialize_end), "bytes", serialized.size());

    const auto deserialize_start = Clock::now();
    const auto loaded = clc::sim::deserialize_simulation_world_state(serialized);
    const auto deserialize_end = Clock::now();
    require(loaded.validation.ok(), "serialized world state should deserialize");
    print_metric("world_state_deserialize_100_settlements_30_days", elapsed_ms(deserialize_start, deserialize_end), "settlements", loaded.state.engine.settlements.size());
}

clc::sim::SimulationRuntime make_runtime_with_caravans(std::size_t caravan_count) {
    auto registry = make_registry(2);
    clc::sim::SimulationRuntime runtime{std::move(registry)};
    require(clc::sim::create_runtime_settlement(runtime, "settlement_0").ok(), "origin should create");
    require(clc::sim::create_runtime_settlement(runtime, "settlement_1").ok(), "destination should create");
    require(clc::sim::add_runtime_route(runtime, clc::sim::make_settlement_route_days(
        "route_0",
        "Benchmark Route",
        "settlement_0",
        "settlement_1",
        7
    )).ok(), "route should add");

    for (std::size_t index = 0; index < caravan_count; ++index) {
        auto created = clc::sim::create_runtime_caravan_for_route(
            runtime,
            "route_0",
            "caravan_" + std::to_string(index),
            "Caravan " + std::to_string(index)
        );
        require(created.ok(), "caravan should create");
    }

    return runtime;
}

void benchmark_runtime_caravans() {
    constexpr std::size_t caravan_count = 1000;
    constexpr std::uint64_t days = 30;
    auto runtime = make_runtime_with_caravans(caravan_count);

    const auto start = Clock::now();
    const auto result = clc::sim::run_runtime_days(runtime, days);
    const auto end = Clock::now();

    require(result.ok(), "runtime day run should succeed");
    print_metric("runtime_run_days_1000_caravans_30_days", elapsed_ms(start, end), "caravan_ticks", result.summary.caravan_ticks);
}

} // namespace

int main() {
    std::cout << "benchmark,metric,value,detail,value\n";
    benchmark_engine_advance_days();
    benchmark_engine_run_scenario_reports();
    benchmark_world_state_serialization();
    benchmark_runtime_caravans();
    return 0;
}
