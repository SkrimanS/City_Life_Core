#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/GameProfileAdoption.hpp"
#include "clc/sim/GameProfileChecklist.hpp"
#include "clc/sim/GameProfileScenarios.hpp"
#include "clc/sim/GameProfiles.hpp"

#include <cstddef>
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

inline void validate_game_profile_checklist(
    clc::data::ValidationReport& report,
    const GameProfileChecklist& checklist,
    std::string_view path
) {
    if (!checklist.found) {
        report.add_error(std::string{path}, "profile checklist should be generated");
        return;
    }
    if (checklist.items.empty()) {
        report.add_error(std::string{path}, "profile checklist should contain review items");
        return;
    }

    const auto summary = game_profile_checklist_summary(checklist);
    if (summary.total_items != checklist.items.size()) {
        report.add_error(std::string{path} + ".summary.total", "checklist summary total must match checklist item count");
    }
    if (summary.required_items + summary.optional_items != summary.total_items) {
        report.add_error(std::string{path} + ".summary.required_optional", "checklist required and optional counts must add up to total count");
    }

    std::vector<std::string_view> checklist_ids;
    for (const auto& item : checklist.items) {
        if (item.id.empty()) {
            report.add_error(std::string{path} + ".item", "checklist item id must not be empty");
        } else if (game_profile_validation_has_duplicate_id(checklist_ids, item.id)) {
            report.add_error(std::string{path} + "." + std::string{item.id}, "checklist item id must be unique");
        } else {
            checklist_ids.push_back(item.id);
        }

        const std::string item_path = std::string{path} + "." + std::string{item.id.empty() ? std::string_view{"<empty>"} : item.id};
        if (item.title.empty()) {
            report.add_error(item_path + ".title", "checklist item title must not be empty");
        }
        if (item.detail.empty()) {
            report.add_error(item_path + ".detail", "checklist item detail must not be empty");
        }
    }
}

inline void validate_game_profile_adoption_report_recommendations(
    clc::data::ValidationReport& report,
    const GameProfileAdoptionReport& adoption,
    const std::vector<GameProfileScenarioRecommendation>& expected_recommendations,
    std::string_view path
) {
    if (adoption.scenario_recommendations.size() != expected_recommendations.size()) {
        report.add_error(std::string{path}, "profile adoption report scenario recommendation count must match profile recommendations");
    }

    const auto comparable_count = adoption.scenario_recommendations.size() < expected_recommendations.size()
        ? adoption.scenario_recommendations.size()
        : expected_recommendations.size();

    for (std::size_t index = 0; index < comparable_count; ++index) {
        const auto& actual = adoption.scenario_recommendations[index];
        const auto& expected = expected_recommendations[index];
        const auto index_path = std::string{path} + "." + std::to_string(index);

        if (actual.profile != expected.profile) {
            report.add_error(index_path + ".profile", "profile adoption report scenario profile enum must match expected recommendation");
        }
        if (actual.profile_id != expected.profile_id) {
            report.add_error(index_path + ".profile_id", "profile adoption report scenario profile id must match expected recommendation");
        }
        if (actual.preset.id != expected.preset.id) {
            report.add_error(index_path + ".preset.id", "profile adoption report scenario preset id must match expected recommendation");
        }
        if (actual.preset.display_name != expected.preset.display_name) {
            report.add_error(index_path + ".preset.display_name", "profile adoption report scenario preset display name must match expected recommendation");
        }
        if (actual.preset.day_count != expected.preset.day_count) {
            report.add_error(index_path + ".preset.day_count", "profile adoption report scenario preset day count must match expected recommendation");
        }
        if (actual.purpose != expected.purpose) {
            report.add_error(index_path + ".purpose", "profile adoption report scenario purpose must match expected recommendation");
        }
    }
}

inline void validate_game_profile_adoption_report(
    clc::data::ValidationReport& report,
    const GameIntegrationProfileDescriptor& profile,
    const GameProfileAdoptionReport& adoption,
    std::string_view path
) {
    if (!adoption.found) {
        report.add_error(std::string{path}, "profile adoption report should be generated");
        return;
    }

    if (adoption.profile_id != profile.id) {
        report.add_error(std::string{path} + ".profile_id", "profile adoption report id must match descriptor id");
    }
    if (adoption.display_name != profile.display_name) {
        report.add_error(std::string{path} + ".display_name", "profile adoption report display name must match descriptor display name");
    }
    if (adoption.support != game_integration_profile_support_name(profile.support)) {
        report.add_error(std::string{path} + ".support", "profile adoption report support must match descriptor support");
    }
    if (adoption.integration_boundary != profile.integration_boundary) {
        report.add_error(std::string{path} + ".integration_boundary", "profile adoption report boundary must match descriptor boundary");
    }
    if (adoption.required_systems != profile.required_systems) {
        report.add_error(std::string{path} + ".required_systems", "profile adoption report required systems must match descriptor required systems");
    }
    if (adoption.optional_systems != profile.optional_systems) {
        report.add_error(std::string{path} + ".optional_systems", "profile adoption report optional systems must match descriptor optional systems");
    }
    if (adoption.non_goals != profile.non_goals) {
        report.add_error(std::string{path} + ".non_goals", "profile adoption report non-goals must match descriptor non-goals");
    }
    if (adoption.needs_c_abi != profile.needs_c_abi) {
        report.add_error(std::string{path} + ".needs_c_abi", "profile adoption report C ABI flag must match descriptor C ABI flag");
    }
    if (adoption.uses_action_bridge != profile.uses_action_bridge) {
        report.add_error(std::string{path} + ".uses_action_bridge", "profile adoption report Action Bridge flag must match descriptor Action Bridge flag");
    }
    if (adoption.server_authoritative != profile.server_authoritative) {
        report.add_error(std::string{path} + ".server_authoritative", "profile adoption report server-authoritative flag must match descriptor server-authoritative flag");
    }

    validate_game_profile_adoption_report_recommendations(
        report,
        adoption,
        game_profile_scenario_recommendations_for_profile(profile.profile),
        std::string{path} + ".scenario_recommendations"
    );

    const auto digest = game_profile_adoption_report_digest(adoption);
    if (digest.empty()) {
        report.add_error(std::string{path} + ".digest", "profile adoption report digest must not be empty");
    }

    const auto markdown = game_profile_adoption_report_markdown(adoption);
    if (markdown.empty()) {
        report.add_error(std::string{path} + ".markdown", "profile adoption report markdown must not be empty");
    } else if (markdown.find(std::string{profile.id}) == std::string::npos) {
        report.add_error(std::string{path} + ".markdown", "profile adoption report markdown must mention profile id");
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

        validate_game_profile_adoption_report(
            report,
            profile,
            make_game_profile_adoption_report(profile),
            path + ".adoption_report"
        );
        validate_game_profile_checklist(
            report,
            make_game_profile_checklist(profile),
            path + ".checklist"
        );
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
