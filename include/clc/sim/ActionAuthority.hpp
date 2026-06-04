#pragma once

#include "clc/core/Time.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/ServerAuthoritative.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

enum class ActionAuthorityRejectionReason {
    none,
    unauthorized,
    invalid_target,
    stale_target,
    insufficient_resource,
    conflict,
};

struct ActorAuthority final {
    std::string actor_id{};
    std::string player_id{};
    std::string faction_id{};
    bool host_admin{false};
    bool tool_actor{false};
    bool can_advance_time{false};
    std::vector<std::string> allowed_system_actions{};
    std::vector<std::string> allowed_resource_ids{};
    std::vector<std::string> controlled_route_ids{};
    std::vector<std::string> controlled_contract_ids{};
    std::vector<std::string> controlled_faction_ids{};
};

struct SystemPermissionRule final {
    std::string action_id{};
    bool host_admin_allowed{true};
    bool tool_actor_allowed{false};
    bool faction_actor_allowed{true};
    bool require_faction{true};
    bool require_explicit_action_grant{false};
};

struct SystemPermissionMatrix final {
    std::vector<SystemPermissionRule> rules{};
};

struct ActionAuthorityPolicy final {
    clc::GameTime::Tick max_action_age_ticks{clc::minutes_to_ticks(5)};
    bool require_actor_match{true};
    bool require_owned_resource_target{true};
    bool allow_admin_resource_mutation{true};
    SystemPermissionMatrix permission_matrix{};
};

struct ActionAuthorityReview final {
    std::uint64_t sequence{0};
    std::uint64_t submitted_tick{0};
    std::uint64_t current_tick{0};
    std::string action_id{};
    std::string action_type{};
    std::string actor_id{};
    std::string player_id{};
    std::string faction_id{};
    std::string target_id{};
    std::string secondary_target_id{};
    std::string resource_id{};
    std::string target_owner_faction_id{};
    std::string system_action_id{};
    bool authorized{false};
    ActionAuthorityRejectionReason rejection_reason{ActionAuthorityRejectionReason::none};
    data::ValidationReport validation{};
};

struct AuthorizedServerActionDispatchResult final {
    ActionAuthorityReview authority{};
    ServerRuntimeActionDispatchResult dispatch{};
};

[[nodiscard]] std::string_view action_authority_rejection_reason_name(ActionAuthorityRejectionReason reason) noexcept;
[[nodiscard]] constexpr std::string_view system_action_build() noexcept { return "build"; }
[[nodiscard]] constexpr std::string_view system_action_trade() noexcept { return "trade"; }
[[nodiscard]] constexpr std::string_view system_action_dispatch_caravan() noexcept { return "dispatch_caravan"; }
[[nodiscard]] constexpr std::string_view system_action_issue_contract() noexcept { return "issue_contract"; }
[[nodiscard]] constexpr std::string_view system_action_change_policy() noexcept { return "change_policy"; }
[[nodiscard]] constexpr std::string_view system_action_tax_market() noexcept { return "tax_market"; }
[[nodiscard]] constexpr std::string_view system_action_control_route() noexcept { return "control_route"; }
[[nodiscard]] constexpr std::string_view system_action_influence_faction() noexcept { return "influence_faction"; }
[[nodiscard]] constexpr std::string_view system_action_inject_event() noexcept { return "inject_event"; }
[[nodiscard]] constexpr std::string_view system_action_advance_time() noexcept { return "advance_time"; }

[[nodiscard]] SystemPermissionMatrix make_default_system_permission_matrix();
[[nodiscard]] data::ValidationReport validate_actor_authority(const ActorAuthority& actor);
[[nodiscard]] data::ValidationReport validate_system_permission_matrix(const SystemPermissionMatrix& matrix);
[[nodiscard]] data::ValidationReport validate_action_authority_policy(const ActionAuthorityPolicy& policy);
[[nodiscard]] const SystemPermissionRule* system_permission_rule_by_action(
    const SystemPermissionMatrix& matrix,
    std::string_view action_id
) noexcept;
[[nodiscard]] bool actor_has_system_permission(
    const ActorAuthority& actor,
    const SystemPermissionMatrix& matrix,
    std::string_view action_id
);
[[nodiscard]] ActionAuthorityReview review_system_permission(
    std::string_view action_id,
    const ActorAuthority& actor,
    const SystemPermissionMatrix& matrix = make_default_system_permission_matrix()
);

[[nodiscard]] ActionAuthorityReview review_runtime_action_authority(
    const SimulationRuntime& runtime,
    const ServerRuntimeActionEnvelope& envelope,
    const ActorAuthority& actor,
    const ActionAuthorityPolicy& policy = {}
);

[[nodiscard]] AuthorizedServerActionDispatchResult dispatch_authorized_server_runtime_action(
    SimulationRuntime& runtime,
    const ServerRuntimeActionEnvelope& envelope,
    const ActorAuthority& actor,
    const ActionAuthorityPolicy& policy = {}
);

[[nodiscard]] std::string action_authority_review_digest(const ActionAuthorityReview& review);

} // namespace clc::sim
