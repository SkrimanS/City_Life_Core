#include "clc/sim/SimulationPersistenceReplay.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace clc::sim {
namespace {

std::vector<std::string> split_lines(std::string_view content) {
    std::vector<std::string> lines;
    std::size_t start = 0;
    while (start <= content.size()) {
        const auto end = content.find('\n', start);
        auto line = end == std::string_view::npos
            ? std::string{content.substr(start)}
            : std::string{content.substr(start, end - start)};
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            lines.push_back(std::move(line));
        }
        if (end == std::string_view::npos) {
            break;
        }
        start = end + 1;
    }
    return lines;
}

std::vector<std::string_view> split_tab_line(std::string_view line) {
    std::vector<std::string_view> fields;
    std::size_t start = 0;
    while (start <= line.size()) {
        const auto tab = line.find('\t', start);
        if (tab == std::string_view::npos) {
            fields.push_back(line.substr(start));
            break;
        }
        fields.push_back(line.substr(start, tab - start));
        start = tab + 1;
    }
    return fields;
}

bool parse_uint32(std::string_view value, std::uint32_t& output) noexcept {
    if (value.empty()) {
        return false;
    }
    std::uint64_t parsed = 0;
    for (const auto character : value) {
        if (character < '0' || character > '9') {
            return false;
        }
        parsed = (parsed * 10ULL) + static_cast<std::uint64_t>(character - '0');
        if (parsed > static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max())) {
            return false;
        }
    }
    output = static_cast<std::uint32_t>(parsed);
    return true;
}

bool parse_uint64(std::string_view value, std::uint64_t& output) noexcept {
    if (value.empty()) {
        return false;
    }
    std::uint64_t parsed = 0;
    for (const auto character : value) {
        if (character < '0' || character > '9') {
            return false;
        }
        const auto digit = static_cast<std::uint64_t>(character - '0');
        if (parsed > (std::numeric_limits<std::uint64_t>::max() - digit) / 10ULL) {
            return false;
        }
        parsed = (parsed * 10ULL) + digit;
    }
    output = parsed;
    return true;
}

std::string join_lines(const std::vector<std::string>& lines) {
    std::string output;
    for (const auto& line : lines) {
        output += line;
        output.push_back('\n');
    }
    return output;
}

std::vector<std::string> canonical_world_state_lines(const SimulationWorldState& state) {
    auto lines = split_lines(serialize_simulation_world_state(state));
    std::sort(lines.begin(), lines.end());
    return lines;
}

std::uint64_t count_line_mismatches(std::vector<std::string> expected, std::vector<std::string> actual) {
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());

    std::uint64_t mismatches = 0;
    std::size_t expected_index = 0;
    std::size_t actual_index = 0;
    while (expected_index < expected.size() || actual_index < actual.size()) {
        if (expected_index >= expected.size()) {
            ++mismatches;
            ++actual_index;
        } else if (actual_index >= actual.size()) {
            ++mismatches;
            ++expected_index;
        } else if (expected[expected_index] == actual[actual_index]) {
            ++expected_index;
            ++actual_index;
        } else if (expected[expected_index] < actual[actual_index]) {
            ++mismatches;
            ++expected_index;
        } else {
            ++mismatches;
            ++actual_index;
        }
    }
    return mismatches;
}

void append_report(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::error) {
            target.add_error(message.path, message.message);
        } else {
            target.add_warning(message.path, message.message);
        }
    }
}

} // namespace

std::string_view save_format_compatibility_name(SaveFormatCompatibility compatibility) noexcept {
    switch (compatibility) {
    case SaveFormatCompatibility::current:
        return "current";
    case SaveFormatCompatibility::migratable_legacy:
        return "migratable_legacy";
    case SaveFormatCompatibility::unsupported:
        return "unsupported";
    }
    return "unknown";
}

std::string_view replay_mismatch_category_name(ReplayMismatchCategory category) noexcept {
    switch (category) {
    case ReplayMismatchCategory::none:
        return "none";
    case ReplayMismatchCategory::runtime_state:
        return "runtime_state";
    case ReplayMismatchCategory::event_log:
        return "event_log";
    }
    return "unknown";
}

SimulationSaveFormatManifest current_simulation_save_format_manifest() noexcept {
    return SimulationSaveFormatManifest{};
}

SimulationSaveFormatReview review_simulation_world_state_save_format(std::string_view content) {
    SimulationSaveFormatReview review;
    const auto lines = split_lines(content);
    review.line_count = lines.size();

    for (std::size_t index = 0; index < lines.size(); ++index) {
        const auto fields = split_tab_line(lines[index]);
        const auto path = "simulation.world_state.format.line." + std::to_string(index + 1);
        if (fields.empty()) {
            continue;
        }

        if (index == 0) {
            if (fields.size() == 2 && fields[0] == "CLC_SIM_WORLD_STATE") {
                review.header_present = true;
                review.format_id = std::string{fields[0]};
                if (!parse_uint32(fields[1], review.version)) {
                    review.validation.add_error(path, "invalid world state format version");
                }
            } else {
                review.validation.add_error(path, "missing world state header");
            }
        }

        if (fields[0] == "time") {
            review.has_time_row = true;
        } else if (fields[0] == "route" && fields.size() == 6) {
            review.has_legacy_route_rows = true;
        } else if (fields[0] == "caravan" && fields.size() == 8) {
            review.has_legacy_caravan_rows = true;
        } else if (fields[0] == "contract" && fields.size() == 10) {
            review.has_legacy_contract_rows = true;
        }
    }

    const auto manifest = current_simulation_save_format_manifest();
    if (!review.header_present) {
        review.compatibility = SaveFormatCompatibility::unsupported;
    } else if (review.version != manifest.version) {
        review.compatibility = SaveFormatCompatibility::unsupported;
        review.validation.add_error("simulation.world_state.format", "unsupported world state format version");
    } else if (review.has_time_row
        && !review.has_legacy_route_rows
        && !review.has_legacy_caravan_rows
        && !review.has_legacy_contract_rows) {
        review.compatibility = SaveFormatCompatibility::current;
    } else {
        review.compatibility = SaveFormatCompatibility::migratable_legacy;
        if (!review.has_time_row) {
            review.validation.add_warning("simulation.world_state.format.time", "legacy save is missing explicit time row");
        }
        if (review.has_legacy_route_rows || review.has_legacy_caravan_rows || review.has_legacy_contract_rows) {
            review.validation.add_warning("simulation.world_state.format.tick_rows", "legacy save uses day-based rows without explicit tick fields");
        }
    }

    return review;
}

SimulationWorldStateMigrationResult migrate_simulation_world_state_content(std::string_view content) {
    SimulationWorldStateMigrationResult result;
    result.before = review_simulation_world_state_save_format(content);
    result.migrated_content = std::string{content};
    if (!result.before.loadable()) {
        result.validation.add_error("simulation.world_state.migration", "world state content is not migratable");
        result.after = result.before;
        return result;
    }

    auto lines = split_lines(content);
    std::uint64_t day = 0;
    bool has_day = false;
    for (const auto& line : lines) {
        const auto fields = split_tab_line(line);
        if (fields.size() == 2 && fields[0] == "day" && parse_uint64(fields[1], day)) {
            has_day = true;
            break;
        }
    }

    if (!result.before.has_time_row) {
        const auto tick = has_day ? clc::days_to_ticks(day) : 0;
        auto insert_position = lines.begin();
        for (auto it = lines.begin(); it != lines.end(); ++it) {
            const auto fields = split_tab_line(*it);
            if (!fields.empty() && fields[0] == "day") {
                insert_position = it + 1;
                break;
            }
        }
        lines.insert(insert_position, "time\t" + std::to_string(tick));
        ++result.inserted_time_rows;
    }

    if (result.before.has_legacy_route_rows) {
        ++result.legacy_row_groups;
    }
    if (result.before.has_legacy_caravan_rows) {
        ++result.legacy_row_groups;
    }
    if (result.before.has_legacy_contract_rows) {
        ++result.legacy_row_groups;
    }

    result.migrated_content = join_lines(lines);
    result.after = review_simulation_world_state_save_format(result.migrated_content);
    if (!result.after.loadable()) {
        result.validation.add_error("simulation.world_state.migration", "migrated world state content is not loadable");
    }
    return result;
}

ReplayComparisonDiagnostics compare_simulation_replay_diagnostics(
    const SimulationRuntime& expected,
    const SimulationRuntime& actual
) {
    ReplayComparisonDiagnostics diagnostics;
    diagnostics.expected_tick = expected.time.current_tick();
    diagnostics.actual_tick = actual.time.current_tick();
    diagnostics.expected_day = expected.engine.current_day();
    diagnostics.actual_day = actual.engine.current_day();
    diagnostics.event_log_comparison = RuntimeEventLogChecksumComparison{
        .expected = RuntimeEventLogChecksum{},
        .actual = RuntimeEventLogChecksum{},
        .event_count_matches = true,
        .first_tick_matches = true,
        .last_tick_matches = true,
        .value_matches = true,
    };

    const auto expected_state = capture_simulation_world_state(expected);
    const auto actual_state = capture_simulation_world_state(actual);
    const auto expected_lines = canonical_world_state_lines(expected_state);
    const auto actual_lines = canonical_world_state_lines(actual_state);
    diagnostics.expected_serialized_line_count = expected_lines.size();
    diagnostics.actual_serialized_line_count = actual_lines.size();
    diagnostics.mismatched_serialized_line_count = count_line_mismatches(expected_lines, actual_lines);

    diagnostics.validation = validate_simulation_runtimes_match(expected, actual);
    if (diagnostics.expected_tick != diagnostics.actual_tick) {
        diagnostics.validation.add_error("runtime.replay.tick", "runtime replay current tick mismatch");
    }
    if (diagnostics.expected_day != diagnostics.actual_day) {
        diagnostics.validation.add_error("runtime.replay.day", "runtime replay current day mismatch");
    }
    if (diagnostics.mismatched_serialized_line_count != 0) {
        diagnostics.validation.add_error("runtime.replay.world_state", "runtime replay serialized world state mismatch");
    }
    if (!diagnostics.validation.ok()) {
        diagnostics.first_mismatch = ReplayMismatchCategory::runtime_state;
    }
    return diagnostics;
}

ReplayComparisonDiagnostics compare_simulation_replay_diagnostics(
    const SimulationRuntime& expected,
    const SimulationRuntime& actual,
    const clc::EventLog& expected_events,
    const clc::EventLog& actual_events
) {
    auto diagnostics = compare_simulation_replay_diagnostics(expected, actual);
    diagnostics.event_log_comparison = compare_runtime_event_log_checksums(
        calculate_runtime_event_log_checksum(expected_events),
        calculate_runtime_event_log_checksum(actual_events)
    );

    auto event_validation = validate_runtime_event_logs_match(expected_events, actual_events);
    if (!event_validation.ok()) {
        append_report(diagnostics.validation, event_validation);
        if (diagnostics.first_mismatch == ReplayMismatchCategory::none) {
            diagnostics.first_mismatch = ReplayMismatchCategory::event_log;
        }
    }
    return diagnostics;
}

SimulationCheckpointPlan make_simulation_checkpoint_plan(
    const SimulationRuntime& runtime,
    const clc::EventLog& events,
    clc::GameTime::Tick interval_ticks
) {
    SimulationCheckpointPlan plan{
        .current_tick = runtime.time.current_tick(),
        .interval_ticks = interval_ticks,
        .event_count = events.size(),
    };

    if (interval_ticks == 0) {
        plan.reason = "checkpoint interval is disabled";
        return plan;
    }

    plan.checkpoints_due = plan.current_tick / interval_ticks;
    plan.next_checkpoint_tick = (plan.checkpoints_due + 1) * interval_ticks;
    plan.checkpoint_due_now = plan.current_tick > 0 && plan.current_tick % interval_ticks == 0;
    plan.reason = plan.checkpoint_due_now ? "checkpoint due at current tick" : "next checkpoint scheduled";
    return plan;
}

std::string simulation_save_format_manifest_digest(const SimulationSaveFormatManifest& manifest) {
    std::ostringstream out;
    out << "save_format_manifest"
        << ";id=" << manifest.format_id
        << ";version=" << manifest.version
        << ";min=" << manifest.minimum_supported_version
        << ";time=" << (manifest.includes_tick_time ? "yes" : "no")
        << ";travel_ticks=" << (manifest.includes_tick_travel_rows ? "yes" : "no")
        << ";contract_due_ticks=" << (manifest.includes_contract_due_ticks ? "yes" : "no")
        << ";ledger=" << (manifest.includes_ledger_entries ? "yes" : "no");
    return out.str();
}

std::string simulation_save_format_review_digest(const SimulationSaveFormatReview& review) {
    std::ostringstream out;
    out << "save_format_review"
        << ";id=" << review.format_id
        << ";version=" << review.version
        << ";compatibility=" << save_format_compatibility_name(review.compatibility)
        << ";header=" << (review.header_present ? "yes" : "no")
        << ";time=" << (review.has_time_row ? "yes" : "no")
        << ";legacy_route=" << (review.has_legacy_route_rows ? "yes" : "no")
        << ";legacy_caravan=" << (review.has_legacy_caravan_rows ? "yes" : "no")
        << ";legacy_contract=" << (review.has_legacy_contract_rows ? "yes" : "no")
        << ";lines=" << review.line_count;
    return out.str();
}

std::string simulation_world_state_migration_digest(const SimulationWorldStateMigrationResult& migration) {
    std::ostringstream out;
    out << "world_state_migration"
        << ";ok=" << (migration.ok() ? "yes" : "no")
        << ";before=" << save_format_compatibility_name(migration.before.compatibility)
        << ";after=" << save_format_compatibility_name(migration.after.compatibility)
        << ";inserted_time=" << migration.inserted_time_rows
        << ";legacy_groups=" << migration.legacy_row_groups;
    return out.str();
}

std::string replay_comparison_diagnostics_digest(const ReplayComparisonDiagnostics& diagnostics) {
    std::ostringstream out;
    out << "replay_diagnostics"
        << ";matches=" << (diagnostics.matches() ? "yes" : "no")
        << ";first_mismatch=" << replay_mismatch_category_name(diagnostics.first_mismatch)
        << ";expected_tick=" << diagnostics.expected_tick
        << ";actual_tick=" << diagnostics.actual_tick
        << ";expected_day=" << diagnostics.expected_day
        << ";actual_day=" << diagnostics.actual_day
        << ";expected_lines=" << diagnostics.expected_serialized_line_count
        << ";actual_lines=" << diagnostics.actual_serialized_line_count
        << ";line_mismatches=" << diagnostics.mismatched_serialized_line_count
        << ";event_log=" << (diagnostics.event_log_comparison.matches() ? "match" : "mismatch");
    return out.str();
}

std::string simulation_checkpoint_plan_digest(const SimulationCheckpointPlan& plan) {
    std::ostringstream out;
    out << "checkpoint_plan"
        << ";current_tick=" << plan.current_tick
        << ";interval=" << plan.interval_ticks
        << ";next=" << plan.next_checkpoint_tick
        << ";events=" << plan.event_count
        << ";due_count=" << plan.checkpoints_due
        << ";due_now=" << (plan.checkpoint_due_now ? "yes" : "no");
    return out.str();
}

} // namespace clc::sim
