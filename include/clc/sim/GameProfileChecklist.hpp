#pragma once

#include "clc/sim/GameProfiles.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct GameProfileChecklistItem final {
    std::string_view id{};
    std::string_view title{};
    std::string_view detail{};
    bool required{true};
};

struct GameProfileChecklist final {
    std::string profile_id{};
    std::vector<GameProfileChecklistItem> items{};
    bool found{false};
};

struct GameProfileChecklistSummary final {
    std::size_t total_items{0};
    std::size_t required_items{0};
    std::size_t optional_items{0};
};

[[nodiscard]] inline std::vector<GameProfileChecklistItem> make_game_profile_checklist_items(
    const GameIntegrationProfileDescriptor& profile
) {
    std::vector<GameProfileChecklistItem> items;
    items.push_back(GameProfileChecklistItem{
        .id = "boundary",
        .title = "Confirm integration boundary",
        .detail = profile.integration_boundary,
        .required = true,
    });

    items.push_back(GameProfileChecklistItem{
        .id = "data_validation",
        .title = "Validate game data before runtime use",
        .detail = "Use DataRegistry validation and data-pack diagnostics before mutating simulation state.",
        .required = true,
    });

    if (profile.uses_action_bridge) {
        items.push_back(GameProfileChecklistItem{
            .id = "action_bridge",
            .title = "Route external commands through the local Action Bridge",
            .detail = "Validate actions before mutation and inspect rejected-action diagnostics.",
            .required = true,
        });
    }

    if (profile.needs_c_abi) {
        items.push_back(GameProfileChecklistItem{
            .id = "c_abi_version",
            .title = "Check C ABI compatibility before use",
            .detail = "Read the native C ABI version and reject unsupported native libraries in the host layer.",
            .required = true,
        });
    }

    if (profile.server_authoritative) {
        items.push_back(GameProfileChecklistItem{
            .id = "server_boundary",
            .title = "Keep sessions, auth and networking outside the core",
            .detail = "The core owns deterministic simulation state, not accounts, transport, replication or matchmaking.",
            .required = true,
        });
        items.push_back(GameProfileChecklistItem{
            .id = "replay_persistence",
            .title = "Validate replay and persistence windows",
            .detail = "Run save/load and replay checks for authoritative simulation flows.",
            .required = true,
        });
    }

    items.push_back(GameProfileChecklistItem{
        .id = "non_goals",
        .title = "Respect core non-goals",
        .detail = "Do not push renderer, UI, networking, account or engine-specific behavior into the C++ core.",
        .required = true,
    });

    items.push_back(GameProfileChecklistItem{
        .id = "examples",
        .title = "Start from the closest SDK example",
        .detail = "Use examples/game_profiles.cpp and the profile-specific recommended systems to choose the next sample.",
        .required = false,
    });

    return items;
}

[[nodiscard]] inline GameProfileChecklist make_game_profile_checklist(
    const GameIntegrationProfileDescriptor& profile
) {
    return GameProfileChecklist{
        .profile_id = std::string{profile.id},
        .items = make_game_profile_checklist_items(profile),
        .found = true,
    };
}

[[nodiscard]] inline GameProfileChecklist make_game_profile_checklist(GameIntegrationProfile profile) {
    const auto* descriptor = game_integration_profile_descriptor(profile);
    if (descriptor == nullptr) {
        return {};
    }
    return make_game_profile_checklist(*descriptor);
}

[[nodiscard]] inline GameProfileChecklist make_game_profile_checklist(std::string_view profile_id) {
    const auto* descriptor = game_integration_profile_by_id(profile_id);
    if (descriptor == nullptr) {
        return GameProfileChecklist{.profile_id = std::string{profile_id}};
    }
    return make_game_profile_checklist(*descriptor);
}

[[nodiscard]] inline GameProfileChecklistSummary game_profile_checklist_summary(
    const GameProfileChecklist& checklist
) noexcept {
    GameProfileChecklistSummary summary;
    if (!checklist.found) {
        return summary;
    }

    for (const auto& item : checklist.items) {
        ++summary.total_items;
        if (item.required) {
            ++summary.required_items;
        } else {
            ++summary.optional_items;
        }
    }
    return summary;
}

[[nodiscard]] inline std::string game_profile_checklist_summary_digest(
    const GameProfileChecklistSummary& summary
) {
    std::string digest = "game_profile_checklist_summary items=";
    digest += std::to_string(summary.total_items);
    digest += " required=";
    digest += std::to_string(summary.required_items);
    digest += " optional=";
    digest += std::to_string(summary.optional_items);
    return digest;
}

[[nodiscard]] inline std::string game_profile_checklist_digest(const GameProfileChecklist& checklist) {
    std::string digest = "game_profile_checklist id=";
    digest += checklist.profile_id;
    digest += " found=";
    digest += checklist.found ? "yes" : "no";
    if (!checklist.found) {
        return digest;
    }

    const auto summary = game_profile_checklist_summary(checklist);
    digest += " items=";
    digest += std::to_string(summary.total_items);
    digest += " required=";
    digest += std::to_string(summary.required_items);
    digest += " optional=";
    digest += std::to_string(summary.optional_items);
    return digest;
}

} // namespace clc::sim
