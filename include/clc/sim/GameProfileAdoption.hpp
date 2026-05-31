#pragma once

#include "clc/sim/GameProfileScenarios.hpp"
#include "clc/sim/GameProfiles.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct GameProfileAdoptionReport final {
    std::string profile_id{};
    std::string display_name{};
    std::string support{};
    std::string integration_boundary{};
    std::vector<std::string_view> required_systems{};
    std::vector<std::string_view> optional_systems{};
    std::vector<std::string_view> non_goals{};
    std::vector<GameProfileScenarioRecommendation> scenario_recommendations{};
    bool found{false};
    bool needs_c_abi{false};
    bool uses_action_bridge{false};
    bool server_authoritative{false};
};

[[nodiscard]] inline GameProfileAdoptionReport make_game_profile_adoption_report(
    const GameIntegrationProfileDescriptor& profile
) {
    return GameProfileAdoptionReport{
        .profile_id = std::string{profile.id},
        .display_name = std::string{profile.display_name},
        .support = std::string{game_integration_profile_support_name(profile.support)},
        .integration_boundary = std::string{profile.integration_boundary},
        .required_systems = profile.required_systems,
        .optional_systems = profile.optional_systems,
        .non_goals = profile.non_goals,
        .scenario_recommendations = game_profile_scenario_recommendations_for_profile(profile.profile),
        .found = true,
        .needs_c_abi = profile.needs_c_abi,
        .uses_action_bridge = profile.uses_action_bridge,
        .server_authoritative = profile.server_authoritative,
    };
}

[[nodiscard]] inline GameProfileAdoptionReport make_game_profile_adoption_report(
    GameIntegrationProfile profile
) {
    const auto* descriptor = game_integration_profile_descriptor(profile);
    if (descriptor == nullptr) {
        return {};
    }
    return make_game_profile_adoption_report(*descriptor);
}

[[nodiscard]] inline GameProfileAdoptionReport make_game_profile_adoption_report(
    std::string_view profile_id
) {
    const auto* descriptor = game_integration_profile_by_id(profile_id);
    if (descriptor == nullptr) {
        return GameProfileAdoptionReport{.profile_id = std::string{profile_id}};
    }
    return make_game_profile_adoption_report(*descriptor);
}

[[nodiscard]] inline std::string game_profile_adoption_report_digest(const GameProfileAdoptionReport& report) {
    std::string digest = "game_profile_adoption id=";
    digest += report.profile_id;
    digest += " found=";
    digest += report.found ? "yes" : "no";
    if (!report.found) {
        return digest;
    }

    digest += " status=";
    digest += report.support;
    digest += " required=";
    digest += std::to_string(report.required_systems.size());
    digest += " optional=";
    digest += std::to_string(report.optional_systems.size());
    digest += " non_goals=";
    digest += std::to_string(report.non_goals.size());
    digest += " scenarios=";
    digest += std::to_string(report.scenario_recommendations.size());
    digest += " c_abi=";
    digest += report.needs_c_abi ? "yes" : "no";
    digest += " action_bridge=";
    digest += report.uses_action_bridge ? "yes" : "no";
    digest += " server_authoritative=";
    digest += report.server_authoritative ? "yes" : "no";
    return digest;
}

[[nodiscard]] inline std::string game_profile_adoption_report_markdown(const GameProfileAdoptionReport& report) {
    if (!report.found) {
        return "# Game Profile Adoption Report\n\nProfile not found: `" + report.profile_id + "`\n";
    }

    std::string output = "# Game Profile Adoption Report\n\n";
    output += "## ";
    output += report.display_name;
    output += "\n\n";
    output += "- id: `" + report.profile_id + "`\n";
    output += "- support: `" + report.support + "`\n";
    output += "- boundary: ";
    output += report.integration_boundary;
    output += "\n";
    output += report.needs_c_abi ? "- needs C ABI: yes\n" : "- needs C ABI: no\n";
    output += report.uses_action_bridge ? "- uses Action Bridge: yes\n" : "- uses Action Bridge: no\n";
    output += report.server_authoritative ? "- server-authoritative: yes\n" : "- server-authoritative: no\n";

    output += "\n### Required systems\n\n";
    for (const auto system : report.required_systems) {
        output += "- `";
        output += system;
        output += "`\n";
    }

    output += "\n### Optional systems\n\n";
    for (const auto system : report.optional_systems) {
        output += "- `";
        output += system;
        output += "`\n";
    }

    output += "\n### Core non-goals\n\n";
    for (const auto non_goal : report.non_goals) {
        output += "- ";
        output += non_goal;
        output += "\n";
    }

    output += "\n### Recommended scenario presets\n\n";
    for (const auto& recommendation : report.scenario_recommendations) {
        output += "- `";
        output += recommendation.preset.id;
        output += "` - ";
        output += recommendation.purpose;
        output += " (";
        output += std::to_string(recommendation.preset.day_count);
        output += " days)\n";
    }

    return output;
}

} // namespace clc::sim
