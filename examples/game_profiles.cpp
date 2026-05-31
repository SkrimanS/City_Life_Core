#include "clc/CityLifeCore.hpp"

#include <iostream>

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

    return 0;
}
