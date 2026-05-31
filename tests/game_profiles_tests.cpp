#include "clc/sim/GameProfileAdoption.hpp"
#include "clc/sim/GameProfileChecklist.hpp"
#include "clc/sim/GameProfileScenarios.hpp"
#include "clc/sim/GameProfileValidation.hpp"
#include "clc/sim/GameProfiles.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

bool contains_descriptor(
    const std::vector<const clc::sim::GameIntegrationProfileDescriptor*>& values,
    std::string_view profile_id
) {
    for (const auto* candidate : values) {
        if (candidate != nullptr && candidate->id == profile_id) {
            return true;
        }
    }
    return false;
}

bool contains_recommendation(
    const std::vector<clc::sim::GameProfileScenarioRecommendation>& values,
    std::string_view preset_id
) {
    for (const auto& candidate : values) {
        if (candidate.preset.id == preset_id) {
            return true;
        }
    }
    return false;
}

bool contains_checklist_item(
    const clc::sim::GameProfileChecklist& checklist,
    std::string_view item_id
) {
    for (const auto& item : checklist.items) {
        if (item.id == item_id) {
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
    const auto catalog_report = clc::sim::validate_game_profile_catalog();
    require(catalog_report.ok(), "profile catalog validation should pass");
    require(
        clc::sim::game_profile_catalog_validation_digest(catalog_report) == "game_profile_catalog_validation ok=yes errors=0 warnings=0",
        "profile catalog validation digest should be stable"
    );

    const auto& profiles = clc::sim::game_integration_profiles();
    require(profiles.size() >= 8, "profile catalog should expose the planned v1.4 adoption profiles");

    std::unordered_set<std::string_view> ids;
    for (const auto& profile : profiles) {
        require(!profile.id.empty(), "profile id must not be empty");
        require(!profile.display_name.empty(), "profile display name must not be empty");
        require(!profile.integration_boundary.empty(), "profile boundary must not be empty");
        require(!profile.required_systems.empty(), "profile should describe required systems");
        require(ids.insert(profile.id).second, "profile ids must be unique");
        require(clc::sim::game_integration_profile_by_id(profile.id) == &profile, "lookup by id should return the catalog entry");
        require(clc::sim::game_integration_profile_descriptor(profile.profile) == &profile, "lookup by enum should return the catalog entry");
        require(!clc::sim::game_integration_profile_digest(profile).empty(), "profile digest should be non-empty");
        require(!clc::sim::game_integration_profile_systems_digest(profile).empty(), "profile systems digest should be non-empty");
    }

    const auto* native_cpp = clc::sim::game_integration_profile_by_id("native_cpp_game");
    require(native_cpp != nullptr, "native C++ profile should exist");
    require(native_cpp->support == clc::sim::GameIntegrationProfileSupport::supported, "native C++ profile should be supported");
    require(!native_cpp->needs_c_abi, "native C++ profile should not require C ABI");
    require(clc::sim::game_integration_profile_has_required_system(*native_cpp, "simulation_runtime"), "native C++ profile should require simulation runtime");
    require(clc::sim::game_integration_profile_has_optional_system(*native_cpp, "action_bridge"), "native C++ profile should list Action Bridge as optional");
    require(clc::sim::game_integration_profile_mentions_system(*native_cpp, "action_bridge"), "native C++ profile should mention Action Bridge");

    const auto* unity = clc::sim::game_integration_profile_by_id("unity_csharp_client");
    require(unity != nullptr, "Unity profile should exist");
    require(unity->support == clc::sim::GameIntegrationProfileSupport::initial_support, "Unity profile should be initial support");
    require(unity->needs_c_abi, "Unity profile should require C ABI");
    require(clc::sim::game_integration_profile_has_required_system(*unity, "opaque_world_handle"), "Unity profile should mention opaque world handles");

    const auto* backend = clc::sim::game_integration_profile_by_id("backend_service");
    require(backend != nullptr, "backend profile should exist");
    require(backend->uses_action_bridge, "backend profile should use Action Bridge");
    require(backend->server_authoritative, "backend profile should be server-authoritative");
    require(clc::sim::game_integration_profile_mentions_system(*backend, "diagnostics"), "backend profile should mention diagnostics");

    const auto* mmo = clc::sim::game_integration_profile_by_id("mmo_server_authoritative");
    require(mmo != nullptr, "MMO profile should exist");
    require(mmo->support == clc::sim::GameIntegrationProfileSupport::planned, "MMO profile should be planned");
    require(mmo->uses_action_bridge, "MMO profile should use Action Bridge where useful");
    require(mmo->server_authoritative, "MMO profile should be server-authoritative");

    const auto supported = clc::sim::game_integration_profiles_by_support(clc::sim::GameIntegrationProfileSupport::supported);
    require(contains_descriptor(supported, "native_cpp_game"), "supported profiles should include native C++");
    require(contains_descriptor(supported, "turn_based_city"), "supported profiles should include turn-based city");

    const auto c_abi_profiles = clc::sim::game_integration_profiles_needing_c_abi();
    require(contains_descriptor(c_abi_profiles, "unity_csharp_client"), "C ABI profiles should include Unity");
    require(contains_descriptor(c_abi_profiles, "browser_wasm"), "C ABI profiles should include Browser/WASM");

    const auto action_bridge_profiles = clc::sim::game_integration_profiles_using_action_bridge();
    require(contains_descriptor(action_bridge_profiles, "backend_service"), "Action Bridge profiles should include backend service");
    require(contains_descriptor(action_bridge_profiles, "editor_balancing_tool"), "Action Bridge profiles should include editor tools");

    const auto server_profiles = clc::sim::game_integration_profiles_server_authoritative();
    require(contains_descriptor(server_profiles, "backend_service"), "server-authoritative profiles should include backend service");
    require(contains_descriptor(server_profiles, "mmo_server_authoritative"), "server-authoritative profiles should include MMO");

    const auto persistence_profiles = clc::sim::game_integration_profiles_requiring_system("persistence");
    require(contains_descriptor(persistence_profiles, "backend_service"), "persistence-required profiles should include backend service");
    require(contains_descriptor(persistence_profiles, "mmo_server_authoritative"), "persistence-required profiles should include MMO");

    const auto& recommendations = clc::sim::game_profile_scenario_recommendations();
    require(!recommendations.empty(), "profile scenario recommendations should not be empty");
    for (const auto& recommendation : recommendations) {
        require(clc::sim::game_integration_profile_by_id(recommendation.profile_id) != nullptr, "scenario recommendation should reference known profile");
        require(clc::sim::validate_scenario_preset(recommendation.preset).ok(), "scenario recommendation preset should validate");
        require(!recommendation.purpose.empty(), "scenario recommendation purpose should not be empty");
        require(!clc::sim::game_profile_scenario_recommendation_digest(recommendation).empty(), "scenario recommendation digest should not be empty");
    }

    const auto backend_recommendations = clc::sim::game_profile_scenario_recommendations_for_profile_id("backend_service");
    require(contains_recommendation(backend_recommendations, "backend_replay_window_10d"), "backend scenario recommendations should include replay window preset");

    const auto mmo_recommendations = clc::sim::game_profile_scenario_recommendations_for_profile(clc::sim::GameIntegrationProfile::mmo_server_authoritative);
    require(contains_recommendation(mmo_recommendations, "mmo_authoritative_soak_30d"), "MMO scenario recommendations should include soak preset");

    const auto backend_catalog = clc::sim::make_game_profile_scenario_preset_catalog("backend_service");
    require(clc::sim::scenario_preset_count(backend_catalog) == backend_recommendations.size(), "backend scenario catalog should match backend recommendation count");
    require(clc::sim::scenario_preset_by_id(backend_catalog, "backend_replay_window_10d") != nullptr, "backend catalog should contain replay preset");

    const auto all_catalog = clc::sim::make_all_game_profile_scenario_preset_catalog();
    require(clc::sim::scenario_preset_count(all_catalog) == recommendations.size(), "all profile scenario catalog should include every recommendation");

    const auto backend_report = clc::sim::make_game_profile_adoption_report("backend_service");
    require(backend_report.found, "backend adoption report should be found");
    require(backend_report.profile_id == "backend_service", "backend adoption report should preserve id");
    require(backend_report.uses_action_bridge, "backend adoption report should preserve Action Bridge flag");
    require(backend_report.server_authoritative, "backend adoption report should preserve server-authoritative flag");
    require(!backend_report.scenario_recommendations.empty(), "backend adoption report should include scenario recommendations");
    require(!clc::sim::game_profile_adoption_report_digest(backend_report).empty(), "backend adoption report digest should be non-empty");
    require(clc::sim::game_profile_adoption_report_markdown(backend_report).find("backend_replay_window_10d") != std::string::npos, "backend markdown report should mention scenario preset");

    const auto backend_checklist = clc::sim::make_game_profile_checklist("backend_service");
    require(backend_checklist.found, "backend checklist should be found");
    require(contains_checklist_item(backend_checklist, "boundary"), "backend checklist should include boundary item");
    require(contains_checklist_item(backend_checklist, "action_bridge"), "backend checklist should include Action Bridge item");
    require(contains_checklist_item(backend_checklist, "server_boundary"), "backend checklist should include server boundary item");
    require(contains_checklist_item(backend_checklist, "replay_persistence"), "backend checklist should include replay/persistence item");
    require(clc::sim::game_profile_checklist_digest(backend_checklist).find("found=yes") != std::string::npos, "backend checklist digest should report found");

    const auto unity_checklist = clc::sim::make_game_profile_checklist("unity_csharp_client");
    require(unity_checklist.found, "Unity checklist should be found");
    require(contains_checklist_item(unity_checklist, "c_abi_version"), "Unity checklist should include C ABI version item");

    const auto missing_checklist = clc::sim::make_game_profile_checklist("missing_profile");
    require(!missing_checklist.found, "missing checklist should not be found");
    require(clc::sim::game_profile_checklist_digest(missing_checklist).find("found=no") != std::string::npos, "missing checklist digest should report not found");

    const auto missing_report = clc::sim::make_game_profile_adoption_report("missing_profile");
    require(!missing_report.found, "missing adoption report should not be found");
    require(clc::sim::game_profile_adoption_report_digest(missing_report).find("found=no") != std::string::npos, "missing adoption digest should report not found");
    require(clc::sim::game_profile_adoption_report_markdown(missing_report).find("Profile not found") != std::string::npos, "missing markdown report should explain missing profile");

    require(clc::sim::game_integration_profile_by_id("missing_profile") == nullptr, "missing profile lookup should return null");
    require(clc::sim::game_integration_profile_support_name(clc::sim::GameIntegrationProfileSupport::supported) == "supported", "support names should be stable");
    require(clc::sim::game_integration_profile_support_name(clc::sim::GameIntegrationProfileSupport::planned) == "planned", "planned support name should be stable");

    return 0;
}
