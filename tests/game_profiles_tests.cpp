#include "clc/sim/GameProfiles.hpp"

#include <cstdlib>
#include <iostream>
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

    require(clc::sim::game_integration_profile_by_id("missing_profile") == nullptr, "missing profile lookup should return null");
    require(clc::sim::game_integration_profile_support_name(clc::sim::GameIntegrationProfileSupport::supported) == "supported", "support names should be stable");
    require(clc::sim::game_integration_profile_support_name(clc::sim::GameIntegrationProfileSupport::planned) == "planned", "planned support name should be stable");

    return 0;
}
