#include "clc/sim/GameProfiles.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>
#include <unordered_set>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

bool contains(std::vector<std::string_view> values, std::string_view value) {
    for (const auto candidate : values) {
        if (candidate == value) {
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
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
    }

    const auto* native_cpp = clc::sim::game_integration_profile_by_id("native_cpp_game");
    require(native_cpp != nullptr, "native C++ profile should exist");
    require(native_cpp->support == clc::sim::GameIntegrationProfileSupport::supported, "native C++ profile should be supported");
    require(!native_cpp->needs_c_abi, "native C++ profile should not require C ABI");
    require(contains(native_cpp->required_systems, "simulation_runtime"), "native C++ profile should require simulation runtime");

    const auto* unity = clc::sim::game_integration_profile_by_id("unity_csharp_client");
    require(unity != nullptr, "Unity profile should exist");
    require(unity->support == clc::sim::GameIntegrationProfileSupport::initial_support, "Unity profile should be initial support");
    require(unity->needs_c_abi, "Unity profile should require C ABI");
    require(contains(unity->required_systems, "opaque_world_handle"), "Unity profile should mention opaque world handles");

    const auto* backend = clc::sim::game_integration_profile_by_id("backend_service");
    require(backend != nullptr, "backend profile should exist");
    require(backend->uses_action_bridge, "backend profile should use Action Bridge");
    require(backend->server_authoritative, "backend profile should be server-authoritative");
    require(contains(backend->non_goals, "HTTP server"), "backend profile should keep HTTP server outside the core");

    const auto* mmo = clc::sim::game_integration_profile_by_id("mmo_server_authoritative");
    require(mmo != nullptr, "MMO profile should exist");
    require(mmo->support == clc::sim::GameIntegrationProfileSupport::planned, "MMO profile should be planned");
    require(mmo->uses_action_bridge, "MMO profile should use Action Bridge where useful");
    require(mmo->server_authoritative, "MMO profile should be server-authoritative");

    require(clc::sim::game_integration_profile_by_id("missing_profile") == nullptr, "missing profile lookup should return null");
    require(clc::sim::game_integration_profile_support_name(clc::sim::GameIntegrationProfileSupport::supported) == "supported", "support names should be stable");
    require(clc::sim::game_integration_profile_support_name(clc::sim::GameIntegrationProfileSupport::planned) == "planned", "planned support name should be stable");

    return 0;
}
