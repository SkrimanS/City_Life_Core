#pragma once

#include "clc/core/EventLog.hpp"
#include "clc/core/Time.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace clc::sim {

enum class SaveFormatCompatibility {
    current,
    migratable_legacy,
    unsupported,
};

enum class ReplayMismatchCategory {
    none,
    runtime_state,
    event_log,
};

struct SimulationSaveFormatManifest final {
    std::string format_id{"CLC_SIM_WORLD_STATE"};
    std::uint32_t version{1};
    std::uint32_t minimum_supported_version{1};
    bool includes_tick_time{true};
    bool includes_tick_travel_rows{true};
    bool includes_contract_due_ticks{true};
    bool includes_ledger_entries{true};
};

struct SimulationSaveFormatReview final {
    std::string format_id{};
    std::uint32_t version{0};
    SaveFormatCompatibility compatibility{SaveFormatCompatibility::unsupported};
    bool header_present{false};
    bool has_time_row{false};
    bool has_legacy_route_rows{false};
    bool has_legacy_caravan_rows{false};
    bool has_legacy_contract_rows{false};
    std::uint64_t line_count{0};
    data::ValidationReport validation{};

    [[nodiscard]] bool loadable() const noexcept {
        return compatibility == SaveFormatCompatibility::current
            || compatibility == SaveFormatCompatibility::migratable_legacy;
    }
};

struct SimulationWorldStateMigrationResult final {
    std::string migrated_content{};
    SimulationSaveFormatReview before{};
    SimulationSaveFormatReview after{};
    std::uint64_t inserted_time_rows{0};
    std::uint64_t legacy_row_groups{0};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok() && after.loadable();
    }
};

struct ReplayComparisonDiagnostics final {
    ReplayMismatchCategory first_mismatch{ReplayMismatchCategory::none};
    std::uint64_t expected_tick{0};
    std::uint64_t actual_tick{0};
    std::uint64_t expected_day{0};
    std::uint64_t actual_day{0};
    std::uint64_t expected_serialized_line_count{0};
    std::uint64_t actual_serialized_line_count{0};
    std::uint64_t mismatched_serialized_line_count{0};
    RuntimeEventLogChecksumComparison event_log_comparison{};
    data::ValidationReport validation{};

    [[nodiscard]] bool matches() const noexcept {
        return validation.ok() && first_mismatch == ReplayMismatchCategory::none;
    }
};

struct SimulationCheckpointPlan final {
    clc::GameTime::Tick current_tick{0};
    clc::GameTime::Tick interval_ticks{0};
    clc::GameTime::Tick next_checkpoint_tick{0};
    std::uint64_t event_count{0};
    std::uint64_t checkpoints_due{0};
    bool checkpoint_due_now{false};
    std::string reason{};
};

[[nodiscard]] std::string_view save_format_compatibility_name(SaveFormatCompatibility compatibility) noexcept;
[[nodiscard]] std::string_view replay_mismatch_category_name(ReplayMismatchCategory category) noexcept;

[[nodiscard]] SimulationSaveFormatManifest current_simulation_save_format_manifest() noexcept;
[[nodiscard]] SimulationSaveFormatReview review_simulation_world_state_save_format(std::string_view content);
[[nodiscard]] SimulationWorldStateMigrationResult migrate_simulation_world_state_content(std::string_view content);

[[nodiscard]] ReplayComparisonDiagnostics compare_simulation_replay_diagnostics(
    const SimulationRuntime& expected,
    const SimulationRuntime& actual
);

[[nodiscard]] ReplayComparisonDiagnostics compare_simulation_replay_diagnostics(
    const SimulationRuntime& expected,
    const SimulationRuntime& actual,
    const clc::EventLog& expected_events,
    const clc::EventLog& actual_events
);

[[nodiscard]] SimulationCheckpointPlan make_simulation_checkpoint_plan(
    const SimulationRuntime& runtime,
    const clc::EventLog& events,
    clc::GameTime::Tick interval_ticks
);

[[nodiscard]] std::string simulation_save_format_manifest_digest(const SimulationSaveFormatManifest& manifest);
[[nodiscard]] std::string simulation_save_format_review_digest(const SimulationSaveFormatReview& review);
[[nodiscard]] std::string simulation_world_state_migration_digest(const SimulationWorldStateMigrationResult& migration);
[[nodiscard]] std::string replay_comparison_diagnostics_digest(const ReplayComparisonDiagnostics& diagnostics);
[[nodiscard]] std::string simulation_checkpoint_plan_digest(const SimulationCheckpointPlan& plan);

} // namespace clc::sim
