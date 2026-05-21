#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view caravan_id = "cargo_delivery_persistence_caravan";
constexpr std::string_view bulk_caravan_a_id = "bulk_cargo_delivery_persistence_caravan_a";
constexpr std::string_view bulk_caravan_b_id = "bulk_cargo_delivery_persistence_caravan_b";
constexpr std::string_view route_id = "riverwatch_to_hillford";
constexpr std::string_view faction_id = "riverwatch";
constexpr std::string_view resource_id = "grain";
constexpr std::string_view origin_settlement_id = "riverwatch";
constexpr std::string_view destination_settlement_id = "hillford";

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void prepare_delivered_runtime(clc::sim::SimulationRuntime& runtime) {
    const auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        route_id,
        std::string{caravan_id},
        "Cargo Delivery Persistence Caravan"
    );
    require(created.ok(), "cargo delivery persistence caravan creation failed");

    require(
        clc::sim::set_runtime_caravan_owner(runtime, std::string{caravan_id}, std::string{faction_id}).ok(),
        "cargo delivery persistence caravan ownership setup failed"
    );

    require(
        clc::sim::load_runtime_caravan_at_origin(runtime, caravan_id, resource_id, 40).ok(),
        "cargo delivery persistence caravan load failed"
    );

    require(
        clc::sim::advance_runtime_caravan_day(runtime, caravan_id).ok(),
        "cargo delivery persistence caravan first advance failed"
    );
    require(
        clc::sim::advance_runtime_caravan_day(runtime, caravan_id).ok(),
        "cargo delivery persistence caravan second advance failed"
    );

    const auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, caravan_id);
    require(delivery.ok(), "cargo delivery persistence delivery failed");
    require(delivery.total_amount == 40, "cargo delivery persistence delivered unexpected amount");
    require(runtime.engine.settlement_resource_amount(destination_settlement_id, resource_id) == 40, "cargo delivery persistence destination storage missing delivered cargo");
    require(runtime.caravans.caravans[0].cargo.empty(), "cargo delivery persistence caravan cargo should be empty after delivery");
}

void prepare_bulk_delivered_runtime(clc::sim::SimulationRuntime& runtime) {
    require(
        runtime.engine.add_resource_to_settlement(std::string{origin_settlement_id}, std::string{resource_id}, 100).ok(),
        "bulk cargo delivery persistence origin top-up failed"
    );

    const auto created_a = clc::sim::create_runtime_caravan_for_route(
        runtime,
        route_id,
        std::string{bulk_caravan_a_id},
        "Bulk Cargo Delivery Persistence Caravan A"
    );
    require(created_a.ok(), "bulk cargo delivery persistence caravan a creation failed");

    const auto created_b = clc::sim::create_runtime_caravan_for_route(
        runtime,
        route_id,
        std::string{bulk_caravan_b_id},
        "Bulk Cargo Delivery Persistence Caravan B"
    );
    require(created_b.ok(), "bulk cargo delivery persistence caravan b creation failed");

    require(
        clc::sim::set_runtime_caravan_owner(runtime, std::string{bulk_caravan_a_id}, std::string{faction_id}).ok(),
        "bulk cargo delivery persistence caravan a ownership setup failed"
    );
    require(
        clc::sim::set_runtime_caravan_owner(runtime, std::string{bulk_caravan_b_id}, std::string{faction_id}).ok(),
        "bulk cargo delivery persistence caravan b ownership setup failed"
    );

    require(
        clc::sim::load_runtime_caravan_at_origin(runtime, bulk_caravan_a_id, resource_id, 15).ok(),
        "bulk cargo delivery persistence caravan a load failed"
    );
    require(
        clc::sim::load_runtime_caravan_at_origin(runtime, bulk_caravan_b_id, resource_id, 20).ok(),
        "bulk cargo delivery persistence caravan b load failed"
    );

    require(
        clc::sim::advance_runtime_caravan_day(runtime, bulk_caravan_a_id).ok(),
        "bulk cargo delivery persistence caravan a first advance failed"
    );
    require(
        clc::sim::advance_runtime_caravan_day(runtime, bulk_caravan_a_id).ok(),
        "bulk cargo delivery persistence caravan a second advance failed"
    );
    require(
        clc::sim::advance_runtime_caravan_day(runtime, bulk_caravan_b_id).ok(),
        "bulk cargo delivery persistence caravan b first advance failed"
    );
    require(
        clc::sim::advance_runtime_caravan_day(runtime, bulk_caravan_b_id).ok(),
        "bulk cargo delivery persistence caravan b second advance failed"
    );

    const auto delivery = clc::sim::deliver_all_runtime_arrived_caravan_cargo_to_destinations(runtime);
    require(delivery.ok(), "bulk cargo delivery persistence bulk delivery failed");
    require(delivery.delivered_caravans == 2, "bulk cargo delivery persistence delivered unexpected caravan count");
    require(delivery.total_amount == 35, "bulk cargo delivery persistence delivered unexpected total amount");
    require(delivery.deliveries.size() == 2, "bulk cargo delivery persistence should report two deliveries");
    require(delivery.deliveries[0].caravan_id == bulk_caravan_a_id, "bulk cargo delivery persistence should preserve delivery order for caravan a");
    require(delivery.deliveries[1].caravan_id == bulk_caravan_b_id, "bulk cargo delivery persistence should preserve delivery order for caravan b");
    require(runtime.engine.settlement_resource_amount(destination_settlement_id, resource_id) == 35, "bulk cargo delivery persistence destination storage missing delivered cargo");
    require(runtime.caravans.caravans[0].cargo.empty(), "bulk cargo delivery persistence caravan a cargo should be empty after delivery");
    require(runtime.caravans.caravans[1].cargo.empty(), "bulk cargo delivery persistence caravan b cargo should be empty after delivery");
}

void expect_runtime_drift_detected(
    const clc::sim::SimulationRuntime& expected,
    const clc::sim::SimulationRuntime& drifted,
    std::string_view message
) {
    const auto match = clc::sim::validate_simulation_runtimes_match(expected, drifted);
    require(!match.ok(), message);
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "cargo delivery persistence bootstrap failed");

    auto& runtime = bootstrap.runtime;
    prepare_delivered_runtime(runtime);

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_cargo_delivery_persistence_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto path = directory / "runtime_cargo_delivery.clcs";

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(runtime, loaded, path);
    require(validation.ok(), "cargo delivery persistence roundtrip validation failed");

    const auto match = clc::sim::validate_simulation_runtimes_match(runtime, loaded);
    require(match.ok(), "cargo delivery persistence runtime match failed");
    require(loaded.engine.settlement_resource_amount(destination_settlement_id, resource_id) == 40, "cargo delivery persistence loaded destination storage mismatch");
    require(!loaded.caravans.caravans.empty(), "cargo delivery persistence loaded caravan missing");
    require(loaded.caravans.caravans[0].cargo.empty(), "cargo delivery persistence loaded caravan cargo should remain empty");

    auto storage_drifted = loaded;
    require(storage_drifted.engine.add_resource_to_settlement(std::string{destination_settlement_id}, std::string{resource_id}, 1).ok(), "cargo delivery persistence storage drift setup failed");
    expect_runtime_drift_detected(runtime, storage_drifted, "runtime match unexpectedly accepted delivered destination storage drift");

    auto cargo_drifted = loaded;
    require(cargo_drifted.caravans.caravans[0].cargo.add(std::string{resource_id}, 1).ok(), "cargo delivery persistence cargo drift setup failed");
    expect_runtime_drift_detected(runtime, cargo_drifted, "runtime match unexpectedly accepted redelivered caravan cargo drift");

    auto bulk_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bulk_bootstrap.ok(), "bulk cargo delivery persistence bootstrap failed");

    auto& bulk_runtime = bulk_bootstrap.runtime;
    prepare_bulk_delivered_runtime(bulk_runtime);

    const auto bulk_path = directory / "runtime_bulk_cargo_delivery.clcs";
    clc::sim::SimulationRuntime bulk_loaded{clc::sim::make_basic_runtime_scenario_registry()};
    const auto bulk_validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(bulk_runtime, bulk_loaded, bulk_path);
    require(bulk_validation.ok(), "bulk cargo delivery persistence roundtrip validation failed");

    const auto bulk_match = clc::sim::validate_simulation_runtimes_match(bulk_runtime, bulk_loaded);
    require(bulk_match.ok(), "bulk cargo delivery persistence runtime match failed");
    require(bulk_loaded.engine.settlement_resource_amount(destination_settlement_id, resource_id) == 35, "bulk cargo delivery persistence loaded destination storage mismatch");
    require(bulk_loaded.caravans.caravans.size() == 2, "bulk cargo delivery persistence loaded caravan count mismatch");
    require(bulk_loaded.caravans.caravans[0].cargo.empty(), "bulk cargo delivery persistence loaded caravan a cargo should remain empty");
    require(bulk_loaded.caravans.caravans[1].cargo.empty(), "bulk cargo delivery persistence loaded caravan b cargo should remain empty");

    auto bulk_storage_drifted = bulk_loaded;
    require(bulk_storage_drifted.engine.add_resource_to_settlement(std::string{destination_settlement_id}, std::string{resource_id}, 1).ok(), "bulk cargo delivery persistence storage drift setup failed");
    expect_runtime_drift_detected(bulk_runtime, bulk_storage_drifted, "runtime match unexpectedly accepted bulk delivered destination storage drift");

    auto bulk_cargo_drifted = bulk_loaded;
    require(bulk_cargo_drifted.caravans.caravans[1].cargo.add(std::string{resource_id}, 1).ok(), "bulk cargo delivery persistence cargo drift setup failed");
    expect_runtime_drift_detected(bulk_runtime, bulk_cargo_drifted, "runtime match unexpectedly accepted bulk redelivered caravan cargo drift");

    std::filesystem::remove_all(directory);
    return 0;
}
