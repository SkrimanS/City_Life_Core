#pragma once

#include "clc/sim/GameProfileAdoption.hpp"
#include "clc/sim/GameProfileChecklist.hpp"
#include "clc/sim/PlatformReadiness.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace clc::sim {

struct GameDeveloperHandoffReport final {
    std::string profile_id{};
    std::string display_name{};
    std::string support{};
    std::string integration_boundary{};
    GameProfileAdoptionReport adoption{};
    GameProfileChecklist checklist{};
    PlatformReadinessReport platform{};
    std::vector<std::string> recommended_examples{};
    std::vector<std::string> required_documents{};
    std::vector<std::string> handoff_warnings{};
    bool found{false};
    bool ready_for_game_team{false};
};

[[nodiscard]] inline std::vector<std::string> game_developer_handoff_examples(
    const GameIntegrationProfileDescriptor& profile
) {
    std::vector<std::string> examples{"examples/game_profiles.cpp", "examples/full_runtime_flow.cpp"};
    if (profile.uses_action_bridge) {
        examples.push_back("examples/action_bridge.cpp");
    }
    if (profile.server_authoritative) {
        examples.push_back("examples/server_authoritative.cpp");
        examples.push_back("examples/multiplayer_to_large_world.cpp");
    }
    if (game_integration_profile_mentions_system(profile, "economy") || game_integration_profile_mentions_system(profile, "market")) {
        examples.push_back("examples/economy_depth.cpp");
    }
    if (game_integration_profile_mentions_system(profile, "persistence") || game_integration_profile_mentions_system(profile, "replay")) {
        examples.push_back("examples/save_load_roundtrip.cpp");
        examples.push_back("examples/replay_persistence.cpp");
    }
    return examples;
}

[[nodiscard]] inline std::vector<std::string> game_developer_handoff_documents(
    const GameIntegrationProfileDescriptor& profile
) {
    std::vector<std::string> docs{
        "docs/README.md",
        "docs/public-api.md",
        "docs/core-concepts.md",
        "docs/sdk-structure.md",
        "docs/game-profiles.md",
        "docs/simulation-processing-doctrine.md",
        "docs/simulation-processors.md",
        "docs/deep-simulation-systems.md",
        "docs/regional-simulation-systems.md",
        "docs/platform-readiness-4.0.0.md",
    };
    if (profile.needs_c_abi) {
        docs.push_back("docs/c-abi.md");
        docs.push_back("docs/csharp-unity.md");
    }
    if (profile.uses_action_bridge) {
        docs.push_back("docs/action-bridge.md");
        docs.push_back("docs/multiplayer-action-authority.md");
    }
    if (profile.server_authoritative) {
        docs.push_back("docs/server-authoritative-mmo.md");
        docs.push_back("docs/multiplayer-persistence-replay.md");
    }
    return docs;
}

[[nodiscard]] inline PlatformReadinessInput make_game_developer_handoff_platform_input(
    const GameIntegrationProfileDescriptor& profile
) {
    return PlatformReadinessInput{
        .module_boundaries_documented = true,
        .profile_requirements_documented = true,
        .content_pipeline_requirements_documented = true,
        .editor_admin_api_requirements_documented = true,
        .diagnostics_api_requirements_documented = true,
        .cxx_api_ready = true,
        .c_abi_readonly_diagnostics_ready = true,
        .save_load_migration_ready = true,
        .replay_migration_ready = true,
        .regional_large_world_ready = true,
        .supported_profile_ids = {std::string{profile.id}},
    };
}

[[nodiscard]] inline GameDeveloperHandoffReport make_game_developer_handoff_report(
    const GameIntegrationProfileDescriptor& profile
) {
    auto platform = make_platform_readiness_report(make_game_developer_handoff_platform_input(profile));
    std::vector<std::string> warnings;
    if (profile.support == GameIntegrationProfileSupport::planned) {
        warnings.push_back("profile is planned; use the SDK foundation but expect integration work");
    }
    if (profile.needs_c_abi) {
        warnings.push_back("foreign-language integrations should check C ABI interface version at startup");
    }
    if (profile.server_authoritative) {
        warnings.push_back("networking, accounts, auth and matchmaking remain outside the core");
    }
    const auto ready = platform.ready && profile.support != GameIntegrationProfileSupport::planned;
    return GameDeveloperHandoffReport{
        .profile_id = std::string{profile.id},
        .display_name = std::string{profile.display_name},
        .support = std::string{game_integration_profile_support_name(profile.support)},
        .integration_boundary = std::string{profile.integration_boundary},
        .adoption = make_game_profile_adoption_report(profile),
        .checklist = make_game_profile_checklist(profile),
        .platform = std::move(platform),
        .recommended_examples = game_developer_handoff_examples(profile),
        .required_documents = game_developer_handoff_documents(profile),
        .handoff_warnings = std::move(warnings),
        .found = true,
        .ready_for_game_team = ready,
    };
}

[[nodiscard]] inline GameDeveloperHandoffReport make_game_developer_handoff_report(std::string_view profile_id) {
    const auto* profile = game_integration_profile_by_id(profile_id);
    if (profile == nullptr) {
        return GameDeveloperHandoffReport{.profile_id = std::string{profile_id}};
    }
    return make_game_developer_handoff_report(*profile);
}

[[nodiscard]] inline std::string game_developer_handoff_digest(const GameDeveloperHandoffReport& report) {
    std::string digest = "game_developer_handoff id=";
    digest += report.profile_id;
    digest += " found=";
    digest += report.found ? "yes" : "no";
    if (!report.found) {
        return digest;
    }
    digest += " ready=";
    digest += report.ready_for_game_team ? "yes" : "no";
    digest += " support=";
    digest += report.support;
    digest += " examples=";
    digest += std::to_string(report.recommended_examples.size());
    digest += " docs=";
    digest += std::to_string(report.required_documents.size());
    digest += " warnings=";
    digest += std::to_string(report.handoff_warnings.size());
    return digest;
}

[[nodiscard]] inline std::string game_developer_handoff_markdown(const GameDeveloperHandoffReport& report) {
    if (!report.found) {
        return "# Game Developer Handoff\n\nProfile not found: `" + report.profile_id + "`\n";
    }
    std::string output = "# Game Developer Handoff\n\n";
    output += "- profile: `" + report.profile_id + "`\n";
    output += "- display name: " + report.display_name + "\n";
    output += "- support: `" + report.support + "`\n";
    output += "- ready for game team: ";
    output += report.ready_for_game_team ? "yes\n" : "no\n";
    output += "- boundary: " + report.integration_boundary + "\n";
    output += "- digest: `" + game_developer_handoff_digest(report) + "`\n";
    output += "\n## Examples\n\n";
    for (const auto& example : report.recommended_examples) {
        output += "- `" + example + "`\n";
    }
    output += "\n## Required Docs\n\n";
    for (const auto& doc : report.required_documents) {
        output += "- `" + doc + "`\n";
    }
    if (!report.handoff_warnings.empty()) {
        output += "\n## Warnings\n\n";
        for (const auto& warning : report.handoff_warnings) {
            output += "- " + warning + "\n";
        }
    }
    return output;
}

} // namespace clc::sim
