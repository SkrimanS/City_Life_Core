#include "clc/sim/DeepReplayCoverage.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {

data::ValidationReport validate_deep_replay_system_coverage(const DeepReplaySystemCoverage& coverage) {
    data::ValidationReport report;
    if (coverage.system_id.empty()) {
        report.add_error("deep_replay.system_id", "system_id must not be empty");
    }
    if (!coverage.save_load_covered) {
        report.add_warning("deep_replay." + coverage.system_id + ".save_load", "save/load coverage is missing");
    }
    if (!coverage.replay_covered) {
        report.add_warning("deep_replay." + coverage.system_id + ".replay", "replay coverage is missing");
    }
    if (!coverage.golden_fixture_available || !coverage.invalid_fixture_available) {
        report.add_warning("deep_replay." + coverage.system_id + ".fixtures", "golden or invalid fixtures are missing");
    }
    if (!coverage.diagnostics_available) {
        report.add_warning("deep_replay." + coverage.system_id + ".diagnostics", "diagnostics are missing");
    }
    return report;
}

DeepReplayCoverageReport make_deep_replay_coverage_report(std::vector<DeepReplaySystemCoverage> systems) {
    DeepReplayCoverageReport report{.system_count = systems.size(), .systems = std::move(systems)};
    for (const auto& system : report.systems) {
        const auto validation = validate_deep_replay_system_coverage(system);
        for (const auto& message : validation.messages()) {
            if (message.severity == data::ValidationSeverity::warning) {
                report.validation.add_warning(message.path, message.message);
            } else {
                report.validation.add_error(message.path, message.message);
            }
        }
        const auto system_ready = system.save_load_covered
            && system.replay_covered
            && system.migration_covered
            && system.golden_fixture_available
            && system.invalid_fixture_available
            && system.diagnostics_available;
        if (system_ready) {
            ++report.ready_system_count;
        }
        if (!system.save_load_covered) {
            ++report.missing_save_load_count;
        }
        if (!system.replay_covered) {
            ++report.missing_replay_count;
        }
        if (!system.golden_fixture_available || !system.invalid_fixture_available) {
            ++report.missing_fixture_count;
        }
    }
    report.ready = report.system_count > 0 && report.ready_system_count == report.system_count && report.validation.ok();
    return report;
}

std::vector<DeepReplaySystemCoverage> make_standard_deep_replay_coverage_baseline(bool covered) {
    const std::vector<std::string> system_ids{
        "resources", "production", "population", "weather", "logistics", "market", "policies", "factions",
        "contracts", "ecology", "events", "autonomous_actors", "content_packs", "scenario_runner", "regional",
    };
    std::vector<DeepReplaySystemCoverage> systems;
    systems.reserve(system_ids.size());
    for (const auto& system_id : system_ids) {
        systems.push_back({
            .system_id = system_id,
            .save_load_covered = covered,
            .replay_covered = covered,
            .migration_covered = covered,
            .golden_fixture_available = covered,
            .invalid_fixture_available = covered,
            .diagnostics_available = covered,
        });
    }
    return systems;
}

std::string deep_replay_coverage_digest(const DeepReplayCoverageReport& report) {
    std::ostringstream out;
    out << "deep_replay_coverage"
        << ";ready=" << (report.ready ? "yes" : "no")
        << ";systems=" << report.system_count
        << ";ready_systems=" << report.ready_system_count
        << ";missing_save_load=" << report.missing_save_load_count
        << ";missing_replay=" << report.missing_replay_count
        << ";missing_fixtures=" << report.missing_fixture_count
        << ";diagnostics=" << report.validation.messages().size();
    return out.str();
}

std::string deep_replay_coverage_markdown(const DeepReplayCoverageReport& report) {
    std::ostringstream out;
    out << "# Deep Replay Coverage\n\n";
    out << "- Ready: " << (report.ready ? "yes" : "no") << "\n";
    out << "- Systems: " << report.system_count << "\n";
    out << "- Ready systems: " << report.ready_system_count << "\n";
    out << "- Missing save/load: " << report.missing_save_load_count << "\n";
    out << "- Missing replay: " << report.missing_replay_count << "\n";
    out << "- Missing fixtures: " << report.missing_fixture_count << "\n";
    return out.str();
}

} // namespace clc::sim
