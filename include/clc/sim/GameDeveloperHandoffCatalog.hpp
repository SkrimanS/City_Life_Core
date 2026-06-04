#pragma once

#include "clc/sim/GameDeveloperHandoff.hpp"
#include "clc/sim/SdkHandoffManifest.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct GameDeveloperHandoffCatalogReport final {
    bool ready{false};
    std::uint64_t profile_count{0};
    std::uint64_t ready_profile_count{0};
    std::uint64_t planned_profile_count{0};
    std::uint64_t starter_example_count{0};
    std::uint64_t document_count{0};
    std::vector<GameDeveloperHandoffReport> profiles{};
    SdkHandoffManifestReport sdk_manifest{};
};

[[nodiscard]] inline GameDeveloperHandoffCatalogReport make_game_developer_handoff_catalog_report(
    SdkHandoffManifestReport sdk_manifest
) {
    GameDeveloperHandoffCatalogReport report{.sdk_manifest = std::move(sdk_manifest)};
    for (const auto& profile : game_integration_profiles()) {
        auto handoff = make_game_developer_handoff_report(profile);
        ++report.profile_count;
        report.starter_example_count += handoff.recommended_examples.size();
        report.document_count += handoff.required_documents.size();
        if (profile.support == GameIntegrationProfileSupport::planned) {
            ++report.planned_profile_count;
        } else if (handoff.ready_for_game_team && !handoff.recommended_examples.empty() && !handoff.required_documents.empty()) {
            ++report.ready_profile_count;
        }
        report.profiles.push_back(std::move(handoff));
    }
    const auto required_ready = report.profile_count - report.planned_profile_count;
    report.ready = report.sdk_manifest.ready && required_ready > 0 && report.ready_profile_count == required_ready;
    return report;
}

[[nodiscard]] inline GameDeveloperHandoffCatalogReport make_standard_game_developer_handoff_catalog_report() {
    return make_game_developer_handoff_catalog_report(make_sdk_handoff_manifest_report({
        .sdk_version = "4.0.0",
        .c_interface_version = 8,
        .artifacts = make_standard_sdk_handoff_artifacts(true),
    }));
}

[[nodiscard]] inline std::string game_developer_handoff_catalog_digest(const GameDeveloperHandoffCatalogReport& report) {
    std::string digest = "game_developer_handoff_catalog";
    digest += ";ready=";
    digest += report.ready ? "yes" : "no";
    digest += ";profiles=" + std::to_string(report.profile_count);
    digest += ";ready_profiles=" + std::to_string(report.ready_profile_count);
    digest += ";planned_profiles=" + std::to_string(report.planned_profile_count);
    digest += ";starter_examples=" + std::to_string(report.starter_example_count);
    digest += ";docs=" + std::to_string(report.document_count);
    digest += ";sdk_manifest=";
    digest += report.sdk_manifest.ready ? "ready" : "not_ready";
    return digest;
}

[[nodiscard]] inline std::string game_developer_handoff_catalog_markdown(const GameDeveloperHandoffCatalogReport& report) {
    std::string output = "# Game Developer Handoff Catalog\n\n";
    output += "- Digest: `" + game_developer_handoff_catalog_digest(report) + "`\n";
    output += "- Ready: ";
    output += report.ready ? "yes\n" : "no\n";
    output += "- Profiles: " + std::to_string(report.profile_count) + "\n";
    output += "- Ready profiles: " + std::to_string(report.ready_profile_count) + "\n";
    output += "- Planned profiles: " + std::to_string(report.planned_profile_count) + "\n";
    output += "\n## Profiles\n\n";
    for (const auto& profile : report.profiles) {
        output += "- `" + profile.profile_id + "`: ";
        output += profile.ready_for_game_team ? "ready" : "not ready";
        output += " examples=" + std::to_string(profile.recommended_examples.size());
        output += " docs=" + std::to_string(profile.required_documents.size()) + "\n";
    }
    return output;
}

} // namespace clc::sim
