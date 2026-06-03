#include "clc/CityLifeCore.hpp"

#include <iostream>
#include <string>

int main() {
    const std::string legacy_save =
        "CLC_SIM_WORLD_STATE\t1\n"
        "day\t3\n"
        "wallet\t75\n"
        "settlement\triverwatch\tRiverwatch\t100\n";

    const auto review = clc::sim::review_simulation_world_state_save_format(legacy_save);
    const auto migration = clc::sim::migrate_simulation_world_state_content(legacy_save);

    auto expected_bootstrap = clc::sim::make_basic_runtime_scenario();
    auto actual_bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!expected_bootstrap.ok() || !actual_bootstrap.ok()) {
        return 1;
    }

    auto& expected = expected_bootstrap.runtime;
    auto& actual = actual_bootstrap.runtime;

    clc::EventLog expected_events;
    clc::EventLog actual_events;
    const auto expected_run = clc::sim::run_runtime_ticks(expected, clc::days_to_ticks(1), clc::hours_to_ticks(12));
    const auto actual_run = clc::sim::run_runtime_ticks(actual, clc::days_to_ticks(1), clc::hours_to_ticks(12));
    const auto expected_summary = clc::sim::append_runtime_tick_run_events(expected_events, expected_run);
    const auto actual_summary = clc::sim::append_runtime_tick_run_events(actual_events, actual_run);
    if (expected_summary.events_appended == 0 || actual_summary.events_appended == 0) {
        return 1;
    }

    const auto replay = clc::sim::compare_simulation_replay_diagnostics(
        expected,
        actual,
        expected_events,
        actual_events
    );

    const auto checkpoint = clc::sim::make_simulation_checkpoint_plan(
        expected,
        expected_events,
        clc::hours_to_ticks(12)
    );

    std::cout << clc::sim::simulation_save_format_manifest_digest(clc::sim::current_simulation_save_format_manifest()) << '\n';
    std::cout << clc::sim::simulation_save_format_review_digest(review) << '\n';
    std::cout << clc::sim::simulation_world_state_migration_digest(migration) << '\n';
    std::cout << clc::sim::replay_comparison_diagnostics_digest(replay) << '\n';
    std::cout << clc::sim::simulation_checkpoint_plan_digest(checkpoint) << '\n';

    return replay.matches() && migration.ok() ? 0 : 1;
}
