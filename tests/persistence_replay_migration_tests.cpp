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

std::string legacy_world_state_content() {
    return std::string{
        "CLC_SIM_WORLD_STATE\t1\n"
        "day\t2\n"
        "wallet\t100\n"
        "settlement\triverwatch\tRiverwatch\t120\n"
        "settlement_storage\triverwatch\tgrain\t40\n"
        "route\triverwatch_to_hillford\tRiverwatch to Hillford\triverwatch\thillford\t3\n"
        "caravan\tlegacy_caravan\tLegacy Caravan\triverwatch_to_hillford\triverwatch\thillford\t3\t1\n"
        "contract\tlegacy_contract\tLegacy Contract\triverwatch\ttraders\tgrain\t10\t20\t4\topen\n"
    };
}

} // namespace

int main() {
    const auto manifest = clc::sim::current_simulation_save_format_manifest();
    require(manifest.version == 1, "current save format version should be 1");
    require(clc::sim::simulation_save_format_manifest_digest(manifest).find("time=yes") != std::string::npos, "manifest digest should expose time row support");

    const auto legacy = legacy_world_state_content();
    const auto review = clc::sim::review_simulation_world_state_save_format(legacy);
    require(review.loadable(), "legacy world state should be loadable");
    require(review.compatibility == clc::sim::SaveFormatCompatibility::migratable_legacy, "legacy world state should be classified as migratable");
    require(!review.has_time_row, "legacy world state should miss time row");
    require(review.has_legacy_route_rows, "legacy route row should be detected");
    require(review.has_legacy_caravan_rows, "legacy caravan row should be detected");
    require(review.has_legacy_contract_rows, "legacy contract row should be detected");

    const auto migration = clc::sim::migrate_simulation_world_state_content(legacy);
    require(migration.ok(), "legacy world state migration should succeed");
    require(migration.inserted_time_rows == 1, "migration should insert one time row");
    require(migration.migrated_content.find("time\t172800") != std::string::npos, "migration should derive tick time from day");
    require(clc::sim::deserialize_simulation_world_state(migration.migrated_content).ok(), "migrated world state should deserialize");

    auto expected_bootstrap = clc::sim::make_basic_runtime_scenario();
    auto actual_bootstrap = clc::sim::make_basic_runtime_scenario();
    require(expected_bootstrap.ok(), "expected runtime bootstrap should succeed");
    require(actual_bootstrap.ok(), "actual runtime bootstrap should succeed");

    auto& expected = expected_bootstrap.runtime;
    auto& actual = actual_bootstrap.runtime;

    clc::EventLog expected_events;
    clc::EventLog actual_events;
    const auto expected_run = clc::sim::run_runtime_ticks(expected, clc::days_to_ticks(2), clc::days_to_ticks(1));
    const auto actual_run = clc::sim::run_runtime_ticks(actual, clc::days_to_ticks(2), clc::days_to_ticks(1));
    const auto expected_summary = clc::sim::append_runtime_tick_run_events(expected_events, expected_run);
    const auto actual_summary = clc::sim::append_runtime_tick_run_events(actual_events, actual_run);
    require(expected_summary.events_appended > 0, "expected event summary should append events");
    require(actual_summary.events_appended > 0, "actual event summary should append events");

    const auto matching = clc::sim::compare_simulation_replay_diagnostics(expected, actual, expected_events, actual_events);
    require(matching.matches(), "matching replay diagnostics should pass");
    require(matching.mismatched_serialized_line_count == 0, "matching replay diagnostics should have no line mismatches");

    auto drifted = actual;
    require(drifted.engine.add_resource_to_settlement("riverwatch", "grain", 1).ok(), "drift setup should mutate storage");
    const auto drift = clc::sim::compare_simulation_replay_diagnostics(expected, drifted, expected_events, actual_events);
    require(!drift.matches(), "drifted replay diagnostics should fail");
    require(drift.first_mismatch == clc::sim::ReplayMismatchCategory::runtime_state, "storage drift should be runtime state mismatch");
    require(drift.mismatched_serialized_line_count > 0, "storage drift should report serialized line mismatches");

    clc::EventLog event_drift;
    event_drift.append(1, "runtime.tick.completed", "elapsed=1");
    const auto event_mismatch = clc::sim::compare_simulation_replay_diagnostics(expected, actual, expected_events, event_drift);
    require(!event_mismatch.matches(), "event log drift should fail diagnostics");
    require(event_mismatch.first_mismatch == clc::sim::ReplayMismatchCategory::event_log, "event drift should be event log mismatch");

    const auto plan = clc::sim::make_simulation_checkpoint_plan(expected, expected_events, clc::days_to_ticks(1));
    require(plan.current_tick == clc::days_to_ticks(2), "checkpoint plan should use current runtime tick");
    require(plan.checkpoint_due_now, "checkpoint should be due on exact interval");
    require(plan.checkpoints_due == 2, "checkpoint plan should count due checkpoints");
    require(clc::sim::simulation_checkpoint_plan_digest(plan).find("due_now=yes") != std::string::npos, "checkpoint digest should expose due state");

    return 0;
}
