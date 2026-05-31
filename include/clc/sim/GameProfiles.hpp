#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

enum class GameIntegrationProfile {
    native_cpp_game,
    turn_based_city,
    realtime_economy,
    survival_colony_logistics,
    unity_csharp_client,
    browser_wasm,
    backend_service,
    mmo_server_authoritative,
    editor_balancing_tool,
};

enum class GameIntegrationProfileSupport {
    supported,
    partially_supported,
    initial_support,
    planned,
};

struct GameIntegrationProfileDescriptor final {
    GameIntegrationProfile profile{GameIntegrationProfile::native_cpp_game};
    std::string_view id{};
    std::string_view display_name{};
    GameIntegrationProfileSupport support{GameIntegrationProfileSupport::planned};
    std::string_view integration_boundary{};
    std::vector<std::string_view> required_systems{};
    std::vector<std::string_view> optional_systems{};
    std::vector<std::string_view> non_goals{};
    bool needs_c_abi{false};
    bool uses_action_bridge{false};
    bool server_authoritative{false};
};

[[nodiscard]] inline std::string_view game_integration_profile_support_name(GameIntegrationProfileSupport support) noexcept {
    switch (support) {
    case GameIntegrationProfileSupport::supported:
        return "supported";
    case GameIntegrationProfileSupport::partially_supported:
        return "partially_supported";
    case GameIntegrationProfileSupport::initial_support:
        return "initial_support";
    case GameIntegrationProfileSupport::planned:
        return "planned";
    }
    return "unknown";
}

[[nodiscard]] inline const std::vector<GameIntegrationProfileDescriptor>& game_integration_profiles() {
    static const std::vector<GameIntegrationProfileDescriptor> profiles{
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::native_cpp_game,
            .id = "native_cpp_game",
            .display_name = "Native C++ game",
            .support = GameIntegrationProfileSupport::supported,
            .integration_boundary = "C++ API / clc/CityLifeCore.hpp",
            .required_systems = {"data_registry", "validation", "simulation_runtime", "settlements", "resources", "events"},
            .optional_systems = {"routes", "caravans", "factions", "contracts", "economy", "action_bridge", "persistence", "replay"},
            .non_goals = {"renderer", "input", "UI framework"},
        },
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::turn_based_city,
            .id = "turn_based_city",
            .display_name = "Turn-based city / settlement simulation",
            .support = GameIntegrationProfileSupport::supported,
            .integration_boundary = "C++ API / SimulationEngine day-step flow",
            .required_systems = {"data_registry", "settlements", "resources", "storage", "day_advance", "events"},
            .optional_systems = {"routes", "caravans", "factions", "contracts", "persistence", "replay"},
            .non_goals = {"turn scheduler UI", "AI player controller", "renderer"},
        },
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::realtime_economy,
            .id = "realtime_economy",
            .display_name = "Real-time economy simulation",
            .support = GameIntegrationProfileSupport::partially_supported,
            .integration_boundary = "C++ API / tick runtime",
            .required_systems = {"game_time", "tick_runtime", "settlements", "resources", "market", "events"},
            .optional_systems = {"wallet", "ledger", "routes", "caravans", "contracts", "replay"},
            .non_goals = {"wall-clock scheduler", "thread pool", "network transport"},
        },
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::survival_colony_logistics,
            .id = "survival_colony_logistics",
            .display_name = "Survival / colony logistics",
            .support = GameIntegrationProfileSupport::supported,
            .integration_boundary = "C++ API / runtime workflow helpers",
            .required_systems = {"data_registry", "settlements", "buildings", "resources", "storage", "tick_remainders"},
            .optional_systems = {"routes", "caravans", "factions", "ownership", "contracts", "ledger"},
            .non_goals = {"colonist AI", "pathfinding", "combat simulation"},
        },
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::unity_csharp_client,
            .id = "unity_csharp_client",
            .display_name = "Unity / C# client or tool",
            .support = GameIntegrationProfileSupport::initial_support,
            .integration_boundary = "C ABI + C# P/Invoke wrapper",
            .required_systems = {"c_abi_version", "opaque_world_handle", "time_advance", "event_access", "diagnostics"},
            .optional_systems = {"native_plugin_build", "managed_safe_access", "unity_smoke_test"},
            .non_goals = {"Unity-specific C++ core", "official Unity package", "managed gameplay framework"},
            .needs_c_abi = true,
        },
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::browser_wasm,
            .id = "browser_wasm",
            .display_name = "Browser / WebAssembly game or tool",
            .support = GameIntegrationProfileSupport::planned,
            .integration_boundary = "future WASM module + JS/TS adapter",
            .required_systems = {"stable_exported_functions", "explicit_data_loading", "memory_ownership_docs", "diagnostics"},
            .optional_systems = {"action_bridge", "save_blob_io", "browser_demo"},
            .non_goals = {"browser API inside C++ core", "DOM/UI framework", "networking framework"},
            .needs_c_abi = true,
            .uses_action_bridge = true,
        },
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::backend_service,
            .id = "backend_service",
            .display_name = "Backend service simulation",
            .support = GameIntegrationProfileSupport::partially_supported,
            .integration_boundary = "C++ API + local Action Bridge + persistence/replay",
            .required_systems = {"simulation_runtime", "action_bridge", "validation", "events", "persistence", "replay"},
            .optional_systems = {"economy", "factions", "contracts", "audit_logs", "diagnostics"},
            .non_goals = {"HTTP server", "authentication", "authorization", "matchmaking"},
            .uses_action_bridge = true,
            .server_authoritative = true,
        },
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::mmo_server_authoritative,
            .id = "mmo_server_authoritative",
            .display_name = "MMO-like server-authoritative simulation",
            .support = GameIntegrationProfileSupport::planned,
            .integration_boundary = "C++ API first, local Action Bridge where useful, future shard-friendly boundaries",
            .required_systems = {"deterministic_commands", "long_running_ticks", "persistence", "replay", "diagnostics"},
            .optional_systems = {"world_partitioning", "audit_logs", "economy_depth", "faction_depth", "contract_depth"},
            .non_goals = {"network transport", "replication protocol", "accounts", "anti-cheat"},
            .uses_action_bridge = true,
            .server_authoritative = true,
        },
        GameIntegrationProfileDescriptor{
            .profile = GameIntegrationProfile::editor_balancing_tool,
            .id = "editor_balancing_tool",
            .display_name = "Editor / balancing tool",
            .support = GameIntegrationProfileSupport::partially_supported,
            .integration_boundary = "C++ API, Action Bridge, C ABI for foreign-language tools",
            .required_systems = {"data_registry", "data_pack_loader", "validation", "diagnostics", "simulation_tick_stepping"},
            .optional_systems = {"action_bridge", "event_inspection", "economy_reports", "persistence", "replay_comparison"},
            .non_goals = {"editor UI", "asset database", "project file format"},
            .needs_c_abi = true,
            .uses_action_bridge = true,
        },
    };
    return profiles;
}

[[nodiscard]] inline const GameIntegrationProfileDescriptor* game_integration_profile_by_id(std::string_view profile_id) noexcept {
    for (const auto& profile : game_integration_profiles()) {
        if (profile.id == profile_id) {
            return &profile;
        }
    }
    return nullptr;
}

[[nodiscard]] inline const GameIntegrationProfileDescriptor* game_integration_profile_descriptor(GameIntegrationProfile selected_profile) noexcept {
    for (const auto& profile : game_integration_profiles()) {
        if (profile.profile == selected_profile) {
            return &profile;
        }
    }
    return nullptr;
}

[[nodiscard]] inline std::string game_integration_profile_digest(const GameIntegrationProfileDescriptor& profile) {
    std::string digest = "game_profile id=";
    digest += profile.id;
    digest += " status=";
    digest += game_integration_profile_support_name(profile.support);
    digest += " boundary=";
    digest += profile.integration_boundary;
    digest += " required=";
    digest += std::to_string(profile.required_systems.size());
    digest += " optional=";
    digest += std::to_string(profile.optional_systems.size());
    digest += " c_abi=";
    digest += profile.needs_c_abi ? "yes" : "no";
    digest += " action_bridge=";
    digest += profile.uses_action_bridge ? "yes" : "no";
    digest += " server_authoritative=";
    digest += profile.server_authoritative ? "yes" : "no";
    return digest;
}

} // namespace clc::sim
