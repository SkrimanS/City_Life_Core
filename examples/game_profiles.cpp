#include "clc/CityLifeCore.hpp"

#include <iostream>

namespace {

void print_profile_ids(
    const char* title,
    const std::vector<const clc::sim::GameIntegrationProfileDescriptor*>& profiles
) {
    std::cout << '\n' << title << ":\n";
    for (const auto* profile : profiles) {
        if (profile != nullptr) {
            std::cout << "- " << profile->id << " (" << profile->display_name << ")\n";
        }
    }
}

void print_profile_scenarios(
    const char* title,
    const std::vector<clc::sim::GameProfileScenarioRecommendation>& recommendations
) {
    std::cout << '\n' << title << ":\n";
    for (const auto& recommendation : recommendations) {
        std::cout << "- " << clc::sim::game_profile_scenario_recommendation_digest(recommendation) << '\n';
    }
}

void print_profile_checklist(const clc::sim::GameProfileChecklist& checklist) {
    std::cout << "\nBackend adoption checklist:\n";
    std::cout << clc::sim::game_profile_checklist_digest(checklist) << '\n';
    for (const auto& item : checklist.items) {
        std::cout << "- [" << (item.required ? "required" : "optional") << "] " << item.id << ": " << item.title << '\n';
    }
}

} // namespace

int main() {
    std::cout << "City Life Core game integration profiles\n";

    for (const auto& profile : clc::sim::game_integration_profiles()) {
        std::cout << clc::sim::game_integration_profile_digest(profile) << '\n';
    }

    const auto* backend = clc::sim::game_integration_profile_by_id("backend_service");
    if (backend == nullptr) {
        std::cerr << "backend profile missing\n";
        return 1;
    }

    std::cout << "\nRecommended systems for " << backend->display_name << ":\n";
    for (const auto system : backend->required_systems) {
        std::cout << "- " << system << '\n';
    }

    std::cout << "\nCore non-goals for " << backend->display_name << ":\n";
    for (const auto non_goal : backend->non_goals) {
        std::cout << "- " << non_goal << '\n';
    }

    print_profile_ids(
        "Profiles that need the C ABI",
        clc::sim::game_integration_profiles_needing_c_abi()
    );
    print_profile_ids(
        "Profiles that use the Action Bridge",
        clc::sim::game_integration_profiles_using_action_bridge()
    );
    print_profile_ids(
        "Server-authoritative profiles",
        clc::sim::game_integration_profiles_server_authoritative()
    );
    print_profile_ids(
        "Profiles requiring persistence",
        clc::sim::game_integration_profiles_requiring_system("persistence")
    );

    std::cout << "\nBackend systems digest:\n";
    std::cout << clc::sim::game_integration_profile_systems_digest(*backend) << '\n';

    print_profile_scenarios(
        "Backend service scenario presets",
        clc::sim::game_profile_scenario_recommendations_for_profile_id("backend_service")
    );
    print_profile_scenarios(
        "MMO-like scenario presets",
        clc::sim::game_profile_scenario_recommendations_for_profile(clc::sim::GameIntegrationProfile::mmo_server_authoritative)
    );

    const auto all_scenarios = clc::sim::make_all_game_profile_scenario_preset_catalog();
    std::cout << "\nAll profile scenario presets=" << clc::sim::scenario_preset_count(all_scenarios) << '\n';

    const auto backend_report = clc::sim::make_game_profile_adoption_report("backend_service");
    std::cout << "\nBackend adoption digest:\n";
    std::cout << clc::sim::game_profile_adoption_report_digest(backend_report) << '\n';
    std::cout << "\nBackend adoption markdown:\n";
    std::cout << clc::sim::game_profile_adoption_report_markdown(backend_report) << '\n';

    print_profile_checklist(clc::sim::make_game_profile_checklist("backend_service"));

    return 0;
}
