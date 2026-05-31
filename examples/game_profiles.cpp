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

    return 0;
}
