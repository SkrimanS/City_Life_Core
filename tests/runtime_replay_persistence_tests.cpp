#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view route_id = "riverwatch_to_hillford";
constexpr std::string_view caravan_id = "replay_caravan";
constexpr std::string_view origin_faction_id = "riverwatch";
constexpr std::string_view contract_id = "grain_delivery_runtime";
constexpr std::string_view resource_id = "grain";
constexpr std::string_view destination_settlement_id = "hillford";

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void print_report(const clc::data::ValidationReport& report) {
    for (const auto& item : report.messages()) {
        std::cerr << item.path << ": " << item.message << '\n';
    }
}

void require_runtimes_match(
    const clc::sim::SimulationRuntime& expected,
    const clc::sim::SimulationRuntime& actual,
    std::string_view message
) {
    const auto match = clc::sim::validate_simulation_runtimes_match(expected, actual);
    if (!match.ok()) {
        print_report(match);
    }
    require(match.ok(), message);
}

void expect_runtime_drift_detected(
    const clc::sim::SimulationRuntime& expected,
    const clc::sim::SimulationRuntime& drifted,
    std::string_view message
) {
    const auto match = clc::sim::validate_simulation_runtimes_match(expected, drifted);
    require(!match.ok(), message);
}

void apply_pre_save_replay_sequence(clc::sim::SimulationRuntime& runtime) {
    const auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        route_id,
        std::string{caravan_id},
        "Replay Caravan"
    );
    require(created.ok(), "replay caravan creation failed");

    require(
        clc::sim::set_runtime_caravan_owner(runtime, std::string{caravan_id}, std::string{origin_faction_id}).ok(),
        "replay caravan ownership setup failed"
    );

    require(
        clc::sim::load_runtime_caravan_at_origin(runtime, caravan_id, resource_id, 40).ok(),
        "replay caravan cargo load failed"
    );

    const auto advanced = clc::sim::advance_runtime_caravan_day(runtime, caravan_id);
    require(advanced.ok(), "replay caravan midpoint advance failed");

    const auto run = clc::sim::run_runtime_days(runtime, 1);
    require(run.reports.size() == 1, "replay runtime midpoint advance failed");
}

void apply_post_load_replay_sequence(clc::sim::SimulationRuntime& runtime) {
    const auto advanced = clc::sim::advance_runtime_caravan_day(runtime, caravan_id);
    require(advanced.ok(), "replay caravan post-load advance failed");

    const auto fulfilled = clc::sim::fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        contract_id,
        caravan_id,
        origin_faction_id
    );
    require(fulfilled.ok(), "replay contract fulfillment failed");

    const auto delivery = clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, caravan_id);
    require(delivery.ok(), "replay cargo delivery failed");
    require(delivery.total_amount == 10, "replay cargo delivery moved unexpected total amount");
    require(delivery.delivered.size() == 1, "replay cargo delivery should report one resource entry");
    require(delivery.delivered[0].resource_id == resource_id, "replay cargo delivery should report grain entry");
    require(delivery.delivered[0].amount == 10, "replay cargo delivery should report remaining grain amount");
    require(runtime.engine.settlement_resource_amount(destination_settlement_id, resource_id) == 10, "replay cargo delivery should credit destination storage");

    const auto run = clc::sim::run_runtime_days(runtime, 2);
    require(run.reports.size() == 2, "replay runtime post-load advance failed");
}

} // namespace

int main() {
    auto source_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(source_bootstrap.ok(), "source replay bootstrap failed");

    auto control_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(control_bootstrap.ok(), "control replay bootstrap failed");

    auto& source = source_bootstrap.runtime;
    auto& control = control_bootstrap.runtime;

    apply_pre_save_replay_sequence(source);
    apply_pre_save_replay_sequence(control);

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_replay_persistence_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    const auto path = directory / "runtime_replay.clcs";
    const auto save_report = clc::sim::save_simulation_runtime_to_file(source, path);
    if (!save_report.ok()) {
        print_report(save_report);
    }
    require(save_report.ok(), "replay midpoint save failed");

    clc::sim::SimulationRuntime replayed{clc::sim::make_basic_runtime_scenario_registry()};
    const auto load = clc::sim::load_simulation_runtime_from_file(path, replayed);
    if (!load.ok()) {
        print_report(load.validation);
    }
    require(load.ok(), "replay midpoint load failed");

    require_runtimes_match(control, replayed, "replay midpoint runtime mismatch");

    apply_post_load_replay_sequence(control);
    apply_post_load_replay_sequence(replayed);

    require_runtimes_match(control, replayed, "post-load replay runtime mismatch after cargo delivery");

    auto drifted = replayed;
    const auto drift_run = clc::sim::run_runtime_days(drifted, 1);
    require(drift_run.reports.size() == 1, "replay negative drift setup failed");
    expect_runtime_drift_detected(control, drifted, "runtime match unexpectedly accepted replay drift");

    auto storage_drifted = replayed;
    require(storage_drifted.engine.add_resource_to_settlement(std::string{destination_settlement_id}, std::string{resource_id}, 1).ok(), "replay destination storage drift setup failed");
    expect_runtime_drift_detected(control, storage_drifted, "runtime match unexpectedly accepted delivered storage drift");

    std::filesystem::remove_all(directory);
    return 0;
}
