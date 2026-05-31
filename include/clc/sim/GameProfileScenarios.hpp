#pragma once

#include "clc/sim/GameProfiles.hpp"
#include "clc/sim/SimulationEngine.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct GameProfileScenarioRecommendation final {
    GameIntegrationProfile profile{GameIntegrationProfile::native_cpp_game};
    std::string_view profile_id{};
    SimulationScenarioPreset preset{};
    std::string_view purpose{};
};

[[nodiscard]] inline const std::vector<GameProfileScenarioRecommendation>& game_profile_scenario_recommendations() {
    static const std::vector<GameProfileScenarioRecommendation> recommendations{
        GameProfileScenarioRecommendation{
            .profile = GameIntegrationProfile::native_cpp_game,
            .profile_id = "native_cpp_game",
            .preset = SimulationScenarioPreset{.id = "native_cpp_smoke_7d", .display_name = "Native C++ smoke - 7 days", .day_count = 7},
            .purpose = "basic C++ integration smoke test",
        },
        GameProfileScenarioRecommendation{
            .profile = GameIntegrationProfile::turn_based_city,
            .profile_id = "turn_based_city",
            .preset = SimulationScenarioPreset{.id = "turn_based_city_month_30d", .display_name = "Turn-based city month - 30 days", .day_count = 30},
            .purpose = "day-step settlement simulation baseline",
        },
        GameProfileScenarioRecommendation{
            .profile = GameIntegrationProfile::realtime_economy,
            .profile_id = "realtime_economy",
            .preset = SimulationScenarioPreset{.id = "realtime_economy_week_7d", .display_name = "Real-time economy equivalent week - 7 days", .day_count = 7},
            .purpose = "economy/tick integration baseline before tick-specific adapters",
        },
        GameProfileScenarioRecommendation{
            .profile = GameIntegrationProfile::survival_colony_logistics,
            .profile_id = "survival_colony_logistics",
            .preset = SimulationScenarioPreset{.id = "colony_logistics_fortnight_14d", .display_name = "Colony logistics fortnight - 14 days", .day_count = 14},
            .purpose = "storage, food and production pressure check",
        },
        GameProfileScenarioRecommendation{
            .profile = GameIntegrationProfile::backend_service,
            .profile_id = "backend_service",
            .preset = SimulationScenarioPreset{.id = "backend_replay_window_10d", .display_name = "Backend replay window - 10 days", .day_count = 10},
            .purpose = "server-side replay and persistence validation window",
        },
        GameProfileScenarioRecommendation{
            .profile = GameIntegrationProfile::mmo_server_authoritative,
            .profile_id = "mmo_server_authoritative",
            .preset = SimulationScenarioPreset{.id = "mmo_authoritative_soak_30d", .display_name = "MMO authoritative soak - 30 days", .day_count = 30},
            .purpose = "long-running authoritative simulation smoke window",
        },
        GameProfileScenarioRecommendation{
            .profile = GameIntegrationProfile::editor_balancing_tool,
            .profile_id = "editor_balancing_tool",
            .preset = SimulationScenarioPreset{.id = "editor_balance_probe_3d", .display_name = "Editor balance probe - 3 days", .day_count = 3},
            .purpose = "short content-balancing probe for tools",
        },
    };
    return recommendations;
}

[[nodiscard]] inline std::vector<GameProfileScenarioRecommendation> game_profile_scenario_recommendations_for_profile(
    GameIntegrationProfile profile
) {
    std::vector<GameProfileScenarioRecommendation> matches;
    for (const auto& recommendation : game_profile_scenario_recommendations()) {
        if (recommendation.profile == profile) {
            matches.push_back(recommendation);
        }
    }
    return matches;
}

[[nodiscard]] inline std::vector<GameProfileScenarioRecommendation> game_profile_scenario_recommendations_for_profile_id(
    std::string_view profile_id
) {
    std::vector<GameProfileScenarioRecommendation> matches;
    for (const auto& recommendation : game_profile_scenario_recommendations()) {
        if (recommendation.profile_id == profile_id) {
            matches.push_back(recommendation);
        }
    }
    return matches;
}

[[nodiscard]] inline SimulationScenarioPresetCatalog make_game_profile_scenario_preset_catalog(
    GameIntegrationProfile profile
) {
    SimulationScenarioPresetCatalog catalog;
    for (const auto& recommendation : game_profile_scenario_recommendations_for_profile(profile)) {
        const auto report = add_scenario_preset(catalog, recommendation.preset);
        (void)report;
    }
    return catalog;
}

[[nodiscard]] inline SimulationScenarioPresetCatalog make_game_profile_scenario_preset_catalog(
    std::string_view profile_id
) {
    SimulationScenarioPresetCatalog catalog;
    for (const auto& recommendation : game_profile_scenario_recommendations_for_profile_id(profile_id)) {
        const auto report = add_scenario_preset(catalog, recommendation.preset);
        (void)report;
    }
    return catalog;
}

[[nodiscard]] inline SimulationScenarioPresetCatalog make_all_game_profile_scenario_preset_catalog() {
    SimulationScenarioPresetCatalog catalog;
    for (const auto& recommendation : game_profile_scenario_recommendations()) {
        const auto report = add_scenario_preset(catalog, recommendation.preset);
        (void)report;
    }
    return catalog;
}

[[nodiscard]] inline std::string game_profile_scenario_recommendation_digest(
    const GameProfileScenarioRecommendation& recommendation
) {
    std::string digest = "game_profile_scenario profile=";
    digest += recommendation.profile_id;
    digest += " preset=";
    digest += recommendation.preset.id;
    digest += " days=";
    digest += std::to_string(recommendation.preset.day_count);
    digest += " purpose=";
    digest += recommendation.purpose;
    return digest;
}

} // namespace clc::sim
