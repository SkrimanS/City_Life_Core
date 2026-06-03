#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::SimulationRuntime make_scale_runtime(std::uint64_t settlement_count, std::uint64_t caravan_count) {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "grain should add");
    for (std::uint64_t index = 0; index < settlement_count; ++index) {
        require(registry.add(clc::data::SettlementDefinition{
            .id = "settlement_" + std::to_string(index),
            .display_name = "Settlement " + std::to_string(index),
            .starting_population = 40,
        }).ok(), "settlement definition should add");
    }

    clc::sim::SimulationRuntime runtime{std::move(registry)};
    for (std::uint64_t index = 0; index < settlement_count; ++index) {
        const auto id = "settlement_" + std::to_string(index);
        require(clc::sim::create_runtime_settlement(runtime, id).ok(), "settlement should create");
        require(runtime.engine.add_resource_to_settlement(id, "grain", 10).ok(), "settlement grain should add");
    }

    require(clc::sim::add_runtime_route(runtime, clc::sim::make_settlement_route_days(
        "route_0",
        "Route 0",
        "settlement_0",
        "settlement_1",
        2
    )).ok(), "route should add");

    for (std::uint64_t index = 0; index < caravan_count; ++index) {
        const auto id = "caravan_" + std::to_string(index);
        require(clc::sim::create_runtime_caravan_for_route(runtime, "route_0", id, "Caravan " + std::to_string(index)).ok(), "caravan should create");
    }
    return runtime;
}

} // namespace

int main() {
    auto runtime = make_scale_runtime(4, 3);
    clc::EventLog events;
    events.append(0, "runtime.tick.completed", "elapsed=1");

    const auto nominal = clc::sim::make_runtime_scale_snapshot(runtime, &events);
    require(nominal.settlements == 4, "scale snapshot should count settlements");
    require(nominal.caravans == 3, "scale snapshot should count caravans");
    require(nominal.runtime_event_log_entries == 1, "scale snapshot should count runtime event log");
    require(nominal.risk == clc::sim::ScaleRiskLevel::nominal, "default thresholds should classify small runtime as nominal");
    require(clc::sim::validate_runtime_scale_snapshot(nominal).ok(), "nominal scale snapshot should validate");

    const clc::sim::ScaleProfileThresholds strict{
        .settlement_watch = 2,
        .settlement_high = 4,
        .caravan_watch = 2,
        .caravan_high = 5,
        .contract_watch = 10,
        .contract_high = 20,
        .event_watch = 1,
        .event_high = 3,
        .serialized_line_watch = 1,
        .serialized_line_high = 1000,
    };

    const auto high = clc::sim::make_runtime_scale_snapshot(runtime, &events, strict);
    require(high.risk == clc::sim::ScaleRiskLevel::high, "strict thresholds should flag high settlement pressure");
    require(high.highest_pressure_area == "settlements", "highest pressure area should report settlements first");
    require(!clc::sim::validate_runtime_scale_snapshot(high, strict).ok(), "high scale snapshot should fail validation");
    require(clc::sim::runtime_scale_snapshot_digest(high).find("risk=high") != std::string::npos, "scale digest should expose risk");
    require(clc::sim::runtime_scale_snapshot_markdown(high).find("Runtime Scale Snapshot") != std::string::npos, "scale markdown should include title");

    return 0;
}
