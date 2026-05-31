#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/GameProfileChecklist.hpp"
#include "clc/sim/GameProfileScenarios.hpp"
#include "clc/sim/GameProfiles.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

[[nodiscard]] inline bool game_profile_validation_vector_contains(
    const std::vector<std::string_view>& values,
    std::string_view value
) noexcept {
    for (const auto candidate : values) {
        if (candidate == value) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] inline bool game_profile_validation_has_duplicate_id(
    const std::vector<std::string_view>& ids,
    std::string_view id
) noexcept {
    return game_profile_validation_vector_contains(ids, id);
}

[[nodiscard]] inline clc::data::ValidationReport validate_game_profile_catalog() {
    clc::data::ValidationReport report;

    const auto& profiles = game_integration_profiles();
    if (profiles.empty()) {
        report.add_error("game_profiles", "profile catalog must not be empty");
    }

    std::vector<std::string_view> profile_ids;
    for (const auto& profile : profiles) {
        const std::string path = "game_profiles." + std::string{profile.id.empty() ? "<empty>" : profile.id};

        if (profile.id.empty()) {
            report.add_error(path + ".id", "profile id must not be empty");
        } else if (game_profile_validation_has_duplicate_id(profile_ids, profile.id)) {
            report.add_error(path + ".id", "profile id must be unique");
        } else {
            profile_ids.push_back(profile.id);
        }

        if (profile.display_name.empty()) {
            report.add_error(path + ".display_name", "profile display name must not be empty");
        }
        if (profile.integration_boundary.empty()) {
            report.add_error(path + ".integration_boundary", "profile integration boundary must not be empty");
        }
        if (profile.required_systems.empty()) {
            report.add_error(path + ".required_systems", "profile should describe required systems");
        }
        if (game_integration_profile_by_id(profile.id) != &profile) {
            report.add_error(path + ".lookup", "profile lookup by id should resolve to the catalog entry");
        }
        if (game_integration_profile_descriptor(profile.profile) != &profile) {
            report.add_error(path + ".lookup", "profile lookup by enum should resolve to the catalog entry");
        }

        const auto checklist = make_game_profile_checklist(profile);
        if (!checklist.found) {
            report.add_error(path + ".checklist", "profile checklist should be generated");
        }
        if (checklist.items.empty()) {
            report.add_error(path + ".checklist", "profile checklist should contain review items");
        }
    }

    std::vector<std::string_view> scenario_ids;
    for (const auto& recommendation : game_profile_scenario_recommendations()) {
        const std::string path = "game_profile_scenarios." + std::string{recommendation.preset.id.empty() ? "<empty>" : recommendation.preset.id};

        if (recommendation.profile_id.empty()) {
            report.add_error(path + ".profile_id", "scenario recommendation profile id must not be empty");
        }
        const auto* profile = game_integration_profile_by_id(recommendation.profile_id);
        if (profile == nullptr) {
            report.add_error(path + ".profile_id", "scenario recommendation must reference a known profile");
        } else if (profile->profile != recommendation.profile) {
            report.add_error(path + ".profile", "scenario recommendation profile enum must match profile id");
        }

        if (recommendation.preset.id.empty()) {
            report.add_error(path + ".preset.id", "scenario preset id must not be empty");
        } else if (game_profile_validation_has_duplicate_id(scenario_ids, recommendation.preset.id)) {
            report.add_error(path + ".preset.id", "scenario preset id must be unique");
        } else {
            scenario_ids.push_back(recommendation.preset.id);
        }

        if (recommendation.purpose.empty()) {
            report.add_error(path + ".purpose", "scenario recommendation purpose must not be empty");
        }

        const auto preset_report = validate_scenario_preset(recommendation.preset);
        if (!preset_report.ok()) {
            report.add_error(path + ".preset", "scenario preset must validate");
        }
    }

    return report;
}

[[nodiscard]] inline std::string game_profile_catalog_validation_digest(const clc::data::ValidationReport& report) {
    std::string digest = "game_profile_catalog_validation ok=";
    digest += report.ok() ? "yes" : "no";
    digest += " errors=";
    digest += std::to_string(report.error_count());
    digest += " warnings=";
    digest += std::to_string(report.warning_count());
    return digest;
}

} // namespace clc::sim
