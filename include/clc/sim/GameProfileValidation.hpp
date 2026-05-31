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

[[nodiscard]] inline bool game_profile_validation_profile_vector_contains(
    const std::vector<GameIntegrationProfile>& values,
    GameIntegrationProfile value
) noexcept {
    for (const auto candidate : values) {
        if (candidate == value) {
            return true;
        }
    }
    return false;
}

inline void validate_game_profile_system_list(
    clc::data::ValidationReport& report,
    const std::vector<std::string_view>& systems,
    std::string_view path
) {
    std::vector<std::string_view> seen;
    for (const auto system : systems) {
        if (system.empty()) {
            report.add_error(std::string{path}, "profile system id must not be empty");
        } else if (game_profile_validation_vector_contains(seen, system)) {
            report.add_error(std::string{path} + "." + std::string{system}, "profile system id must be unique in its list");
        } else {
            seen.push_back(system);
        }
    }
}

inline void validate_game_profile_system_overlap(
    clc::data::ValidationReport& report,
    const GameIntegrationProfileDescriptor& profile,
    std::string_view path
) {
    for (const auto required_system : profile.required_systems) {
        if (game_integration_profile_has_optional_system(profile, required_system)) {
            report.add_error(std::string{path} + "." + std::string{required_system}, "profile system must not be both required and optional");
        }
    }
}

[[nodiscard]] inline clc::data::ValidationReport validate_game_profile_catalog() {
    clc::data::ValidationReport report;

    const auto& profiles = game_integration_profiles();
    if (profiles.empty()) {
        report.add_error("game_profiles", "profile catalog must not be empty");
    }

    const auto summary = game_integration_profile_catalog_summary();
    if (summary.total_profiles != profiles.size()) {
        report.add_error("game_profiles.summary.total", "profile summary total must match profile catalog size");
    }
    if (
        summary.supported_profiles
            + summary.partially_supported_profiles
            + summary.initial_support_profiles
            + summary.planned_profiles
        != summary.total_profiles
    ) {
        report.add_error("game_profiles.summary.support", "profile support counts must add up to the total profile count");
    }
    if (summary.c_abi_profiles != game_integration_profiles_needing_c_abi().size()) {
        report.add_error("game_profiles.summary.c_abi", "C ABI summary count must match C ABI profile query count");
    }
    if (summary.action_bridge_profiles != game_integration_profiles_using_action_bridge().size()) {
        report.add_error("game_profiles.summary.action_bridge", "Action Bridge summary count must match Action Bridge profile query count");
    }
    if (summary.server_authoritative_profiles != game_integration_profiles_server_authoritative().size()) {
        report.add_error("game_profiles.summary.server_authoritative", "server-authoritative summary count must match server-authoritative profile query count");
    }

    std::vector<std::string_view> profile_ids;
    std::vector<GameIntegrationProfile> profile_enums;
    for (const auto& profile : profiles) {
        const std::string profile_id_for_path = profile.id.empty() ? std::string{"<empty>"} : std::string{profile.id};
        const std::string path = "game_profiles." + profile_id_for_path;

        if (profile.id.empty()) {
            report.add_error(path + ".id", "profile id must not be empty");
        } else if (game_profile_validation_has_duplicate_id(profile_ids, profile.id)) {
            report.add_error(path + ".id", "profile id must be unique");
        } else {
            profile_ids.push_back(profile.id);
        }

        if (game_profile_validation_profile_vector_contains(profile_enums, profile.profile)) {
            report.add_error(path + ".profile", "profile enum value must be unique");
        } else {
            profile_enums.push_back(profile.profile);
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
        validate_game_profile_system_list(report, profile.required_systems, path + ".required_systems");
        validate_game_profile_system_list(report, profile.optional_systems, path + ".optional_systems");
        validate_game_profile_system_overlap(report, profile, path + ".systems");

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
        const std::string preset_id_for_path = recommendation.preset.id.empty() ? std::string{"<empty>"} : recommendation.preset.id;
        const std::string path = "game_profile_scenarios." + preset_id_for_path;

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
