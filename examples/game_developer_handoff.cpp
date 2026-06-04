#include "clc/CityLifeCore.hpp"

#include <iostream>
#include <string_view>

namespace {

void print_handoff(std::string_view profile_id) {
    const auto report = clc::sim::make_game_developer_handoff_report(profile_id);
    std::cout << clc::sim::game_developer_handoff_digest(report) << '\n';
    if (!report.found) {
        return;
    }

    std::cout << "boundary: " << report.integration_boundary << '\n';
    std::cout << "platform: " << clc::sim::platform_readiness_digest(report.platform) << '\n';
    std::cout << "adoption: " << clc::sim::game_profile_adoption_report_digest(report.adoption) << '\n';
    std::cout << "checklist: " << clc::sim::game_profile_checklist_digest(report.checklist) << '\n';

    std::cout << "examples:\n";
    for (const auto& example : report.recommended_examples) {
        std::cout << "- " << example << '\n';
    }

    std::cout << "docs:\n";
    for (const auto& doc : report.required_documents) {
        std::cout << "- " << doc << '\n';
    }

    if (!report.handoff_warnings.empty()) {
        std::cout << "warnings:\n";
        for (const auto& warning : report.handoff_warnings) {
            std::cout << "- " << warning << '\n';
        }
    }
    std::cout << '\n';
}

} // namespace

int main() {
    std::cout << "City Life Core game developer handoff\n";
    std::cout << "version: " << clc::core_version_string() << '\n';
    std::cout << "profiles: " << clc::sim::game_integration_profiles().size() << "\n\n";

    print_handoff("native_cpp_game");
    print_handoff("turn_based_city");
    print_handoff("backend_service");
    print_handoff("unity_csharp_client");
    print_handoff("mmo_server_authoritative");

    return 0;
}
