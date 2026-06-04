#include "clc/sim/SaveReplayFixtures.hpp"

#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace clc::sim {
namespace {

std::string make_golden_payload() {
    auto bootstrap = make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return "CLC_SIM_WORLD_STATE\t1\ntime\t0\nday\t0\n";
    }
    return serialize_simulation_world_state(capture_simulation_world_state(bootstrap.runtime));
}

std::string make_invalid_payload(std::string_view system_id) {
    std::string payload = "CLC_SIM_WORLD_STATE\t999\n";
    payload += "invalid\t";
    payload += system_id;
    payload += "\n";
    return payload;
}

void append_validation(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::warning) {
            target.add_warning(message.path, message.message);
        } else {
            target.add_error(message.path, message.message);
        }
    }
}

} // namespace

std::vector<SaveReplayFixture> make_standard_save_replay_fixtures() {
    const auto golden = make_golden_payload();
    const std::vector<std::pair<std::string, bool>> systems{
        {"resources", false},
        {"production", false},
        {"population", false},
        {"weather", false},
        {"logistics", false},
        {"market", false},
        {"policies", false},
        {"factions", false},
        {"contracts", false},
        {"ecology", false},
        {"events", false},
        {"autonomous_actors", false},
        {"content_packs", false},
        {"scenario_runner", false},
        {"regional", true},
        {"regional_market", true},
        {"regional_logistics", true},
        {"regional_climate", true},
        {"regional_territory", true},
        {"regional_migration", true},
        {"regional_audit", true},
        {"long_running_maintenance", true},
    };

    std::vector<SaveReplayFixture> fixtures;
    fixtures.reserve(systems.size());
    for (const auto& [system_id, regional] : systems) {
        fixtures.push_back({
            .system_id = system_id,
            .schema_id = "CLC_SIM_WORLD_STATE",
            .schema_version = current_simulation_save_format_manifest().version,
            .regional = regional,
            .golden_save_payload = golden,
            .invalid_save_payload = make_invalid_payload(system_id),
            .replay_digest = "replay_fixture;system=" + system_id + ";deterministic=yes",
        });
    }
    return fixtures;
}

data::ValidationReport validate_save_replay_fixture(const SaveReplayFixture& fixture) {
    data::ValidationReport report;
    if (fixture.system_id.empty()) {
        report.add_error("save_replay_fixture.system_id", "system_id must not be empty");
    }
    if (fixture.schema_id.empty()) {
        report.add_error("save_replay_fixture." + fixture.system_id + ".schema_id", "schema_id must not be empty");
    }
    if (fixture.schema_version == 0) {
        report.add_error("save_replay_fixture." + fixture.system_id + ".schema_version", "schema_version must be greater than zero");
    }
    if (fixture.replay_digest.empty()) {
        report.add_error("save_replay_fixture." + fixture.system_id + ".replay_digest", "replay_digest must not be empty");
    }

    const auto golden_review = review_simulation_world_state_save_format(fixture.golden_save_payload);
    if (!golden_review.loadable()) {
        report.add_error("save_replay_fixture." + fixture.system_id + ".golden", "golden save fixture must be loadable");
    }
    if (golden_review.compatibility != SaveFormatCompatibility::current) {
        report.add_warning("save_replay_fixture." + fixture.system_id + ".golden", "golden save fixture is not in the current format");
    }

    const auto invalid_review = review_simulation_world_state_save_format(fixture.invalid_save_payload);
    if (invalid_review.loadable()) {
        report.add_error("save_replay_fixture." + fixture.system_id + ".invalid", "invalid save fixture should not be loadable");
    }
    return report;
}

SaveReplayFixtureCatalogReport make_save_replay_fixture_catalog_report(std::vector<SaveReplayFixture> fixtures) {
    SaveReplayFixtureCatalogReport report{.fixture_count = fixtures.size(), .fixtures = std::move(fixtures)};
    for (const auto& fixture : report.fixtures) {
        const auto validation = validate_save_replay_fixture(fixture);
        append_validation(report.validation, validation);
        if (validation.error_count() == 0) {
            ++report.ready_fixture_count;
        }
        if (fixture.regional) {
            ++report.regional_fixture_count;
        }
        const auto invalid_review = review_simulation_world_state_save_format(fixture.invalid_save_payload);
        if (!invalid_review.loadable()) {
            ++report.invalid_fixture_count;
        }
    }
    report.ready = report.fixture_count > 0
        && report.ready_fixture_count == report.fixture_count
        && report.invalid_fixture_count == report.fixture_count
        && report.regional_fixture_count > 0
        && report.validation.error_count() == 0;
    return report;
}

std::string save_replay_fixture_catalog_digest(const SaveReplayFixtureCatalogReport& report) {
    std::ostringstream out;
    out << "save_replay_fixtures"
        << ";ready=" << (report.ready ? "yes" : "no")
        << ";fixtures=" << report.fixture_count
        << ";ready_fixtures=" << report.ready_fixture_count
        << ";regional=" << report.regional_fixture_count
        << ";invalid=" << report.invalid_fixture_count
        << ";diagnostics=" << report.validation.messages().size();
    return out.str();
}

std::string save_replay_fixture_catalog_markdown(const SaveReplayFixtureCatalogReport& report) {
    std::ostringstream out;
    out << "# Save Replay Fixtures\n\n";
    out << "- Digest: `" << save_replay_fixture_catalog_digest(report) << "`\n";
    out << "- Fixture count: " << report.fixture_count << "\n";
    out << "- Ready fixtures: " << report.ready_fixture_count << "\n";
    out << "- Regional fixtures: " << report.regional_fixture_count << "\n";
    out << "- Invalid fixtures: " << report.invalid_fixture_count << "\n";
    out << "\n## Fixtures\n\n";
    for (const auto& fixture : report.fixtures) {
        out << "- `" << fixture.system_id << "` schema `" << fixture.schema_id << "` v" << fixture.schema_version
            << (fixture.regional ? " regional" : " deep") << "\n";
    }
    return out.str();
}

} // namespace clc::sim
