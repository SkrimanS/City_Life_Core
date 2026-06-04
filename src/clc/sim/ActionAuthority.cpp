#include "clc/sim/ActionAuthority.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

void append_validation(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::warning) {
            target.add_warning(message.path, message.message);
        } else {
            target.add_error(message.path, message.message);
        }
    }
}

bool contains_string(const std::vector<std::string>& values, std::string_view value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

bool resource_allowed(const ActorAuthority& actor, std::string_view resource_id) {
    return actor.allowed_resource_ids.empty() || contains_string(actor.allowed_resource_ids, resource_id);
}

bool system_action_allowed(const ActorAuthority& actor, std::string_view action_id) {
    return actor.allowed_system_actions.empty() || contains_string(actor.allowed_system_actions, action_id);
}

bool is_resource_action(std::string_view action_type) {
    return action_type == runtime_action_type_add_resource
        || action_type == runtime_action_type_remove_resource
        || action_type == runtime_action_type_transfer_resource;
}

void reject(ActionAuthorityReview& review, ActionAuthorityRejectionReason reason, std::string path, std::string message) {
    if (review.rejection_reason == ActionAuthorityRejectionReason::none) {
        review.rejection_reason = reason;
    }
    review.validation.add_error(std::move(path), std::move(message));
    review.authorized = false;
}

bool actor_can_mutate_resource_targets(const ActorAuthority& actor, const ActionAuthorityPolicy& policy) {
    return actor.host_admin && policy.allow_admin_resource_mutation;
}

bool target_owned_by_actor(
    const OwnershipCatalog& ownership,
    std::string_view settlement_id,
    const ActorAuthority& actor
) {
    const auto owner = settlement_owner(ownership, settlement_id);
    return !owner.empty() && owner == actor.faction_id;
}

RuntimeActionResult authority_rejected_action_result(const RuntimeAction& action, const ActionAuthorityReview& review) {
    return RuntimeActionResult{
        .action_id = action.action_id,
        .type = action.type,
        .accepted = false,
        .validation_status = std::string{runtime_action_status_rejected},
        .error_code = std::string{action_authority_rejection_reason_name(review.rejection_reason)},
        .message = review.validation.messages().empty() ? std::string{"action authority rejected"} : review.validation.messages().front().message,
        .validation = review.validation,
    };
}

ServerRuntimeActionAuditRecord make_authority_rejected_audit(
    const SimulationRuntime& runtime,
    const ServerRuntimeActionEnvelope& envelope,
    const RuntimeActionResult& result
) {
    return ServerRuntimeActionAuditRecord{
        .sequence = envelope.sequence,
        .submitted_tick = envelope.submitted_tick,
        .resulting_day = runtime.engine.current_day(),
        .shard_id = envelope.shard_id,
        .action_id = envelope.action.action_id,
        .action_type = envelope.action.type,
        .actor_id = envelope.action.actor_id,
        .player_id = envelope.session.player_id,
        .session_id = envelope.session.session_id,
        .accepted = false,
        .validation_status = result.validation_status,
        .error_code = result.error_code,
        .event_count = 0,
        .diagnostic_count = result.validation.messages().size(),
    };
}

} // namespace

std::string_view action_authority_rejection_reason_name(ActionAuthorityRejectionReason reason) noexcept {
    switch (reason) {
    case ActionAuthorityRejectionReason::none:
        return "none";
    case ActionAuthorityRejectionReason::unauthorized:
        return "unauthorized";
    case ActionAuthorityRejectionReason::invalid_target:
        return "invalid_target";
    case ActionAuthorityRejectionReason::stale_target:
        return "stale_target";
    case ActionAuthorityRejectionReason::insufficient_resource:
        return "insufficient_resource";
    case ActionAuthorityRejectionReason::conflict:
        return "conflict";
    }
    return "conflict";
}

SystemPermissionMatrix make_default_system_permission_matrix() {
    return SystemPermissionMatrix{.rules = {
        {.action_id = std::string{system_action_build()}, .require_explicit_action_grant = true},
        {.action_id = std::string{system_action_trade()}},
        {.action_id = std::string{system_action_dispatch_caravan()}, .require_explicit_action_grant = true},
        {.action_id = std::string{system_action_issue_contract()}, .require_explicit_action_grant = true},
        {.action_id = std::string{system_action_change_policy()}, .require_explicit_action_grant = true},
        {.action_id = std::string{system_action_tax_market()}, .require_explicit_action_grant = true},
        {.action_id = std::string{system_action_control_route()}, .require_explicit_action_grant = true},
        {.action_id = std::string{system_action_influence_faction()}, .require_explicit_action_grant = true},
        {
            .action_id = std::string{system_action_inject_event()},
            .tool_actor_allowed = true,
            .faction_actor_allowed = false,
            .require_faction = false,
            .require_explicit_action_grant = true,
        },
        {.action_id = std::string{system_action_advance_time()}, .require_explicit_action_grant = false},
    }};
}

data::ValidationReport validate_actor_authority(const ActorAuthority& actor) {
    data::ValidationReport report;
    if (actor.actor_id.empty()) {
        report.add_error("authority.actor.actor_id", "actor_id must not be empty");
    }
    if (actor.player_id.empty()) {
        report.add_error("authority.actor.player_id", "player_id must not be empty");
    }
    if (actor.faction_id.empty() && !actor.host_admin && !actor.tool_actor) {
        report.add_error("authority.actor.faction_id", "faction_id must not be empty for non-admin actors");
    }
    for (const auto& resource_id : actor.allowed_resource_ids) {
        if (resource_id.empty()) {
            report.add_error("authority.actor.allowed_resource_ids", "allowed resource ids must not be empty");
        }
    }
    for (const auto& action_id : actor.allowed_system_actions) {
        if (action_id.empty()) {
            report.add_error("authority.actor.allowed_system_actions", "allowed system action ids must not be empty");
        }
    }
    for (const auto& route_id : actor.controlled_route_ids) {
        if (route_id.empty()) {
            report.add_error("authority.actor.controlled_route_ids", "controlled route ids must not be empty");
        }
    }
    for (const auto& contract_id : actor.controlled_contract_ids) {
        if (contract_id.empty()) {
            report.add_error("authority.actor.controlled_contract_ids", "controlled contract ids must not be empty");
        }
    }
    for (const auto& faction_id : actor.controlled_faction_ids) {
        if (faction_id.empty()) {
            report.add_error("authority.actor.controlled_faction_ids", "controlled faction ids must not be empty");
        }
    }
    return report;
}

data::ValidationReport validate_system_permission_matrix(const SystemPermissionMatrix& matrix) {
    data::ValidationReport report;
    for (const auto& rule : matrix.rules) {
        if (rule.action_id.empty()) {
            report.add_error("authority.permission_matrix.action_id", "permission action_id must not be empty");
        }
        if (!rule.host_admin_allowed && !rule.tool_actor_allowed && !rule.faction_actor_allowed) {
            report.add_warning("authority.permission_matrix.rule", "permission rule has no allowed actor class");
        }
    }
    return report;
}

data::ValidationReport validate_action_authority_policy(const ActionAuthorityPolicy& policy) {
    data::ValidationReport report;
    if (policy.max_action_age_ticks == 0) {
        report.add_error("authority.policy.max_action_age_ticks", "max_action_age_ticks must be greater than zero");
    }
    append_validation(report, validate_system_permission_matrix(policy.permission_matrix));
    return report;
}

const SystemPermissionRule* system_permission_rule_by_action(
    const SystemPermissionMatrix& matrix,
    std::string_view action_id
) noexcept {
    for (const auto& rule : matrix.rules) {
        if (rule.action_id == action_id) {
            return &rule;
        }
    }
    return nullptr;
}

bool actor_has_system_permission(
    const ActorAuthority& actor,
    const SystemPermissionMatrix& matrix,
    std::string_view action_id
) {
    const auto* rule = system_permission_rule_by_action(matrix, action_id);
    if (rule == nullptr) {
        return false;
    }
    if (actor.host_admin && rule->host_admin_allowed) {
        return true;
    }
    if (actor.tool_actor && rule->tool_actor_allowed && system_action_allowed(actor, action_id)) {
        return true;
    }
    if (rule->require_faction && actor.faction_id.empty()) {
        return false;
    }
    if (!rule->faction_actor_allowed) {
        return false;
    }
    return !rule->require_explicit_action_grant || system_action_allowed(actor, action_id);
}

ActionAuthorityReview review_system_permission(
    std::string_view action_id,
    const ActorAuthority& actor,
    const SystemPermissionMatrix& matrix
) {
    ActionAuthorityReview review{
        .actor_id = actor.actor_id,
        .player_id = actor.player_id,
        .faction_id = actor.faction_id,
        .system_action_id = std::string{action_id},
        .authorized = true,
    };
    append_validation(review.validation, validate_actor_authority(actor));
    append_validation(review.validation, validate_system_permission_matrix(matrix));
    if (system_permission_rule_by_action(matrix, action_id) == nullptr) {
        reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.system_action_id", "system action has no permission rule");
    } else if (!actor_has_system_permission(actor, matrix, action_id)) {
        reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.system_action_id", "actor is not allowed to perform this system action");
    }
    return review;
}

ActionAuthorityReview review_runtime_action_authority(
    const SimulationRuntime& runtime,
    const ServerRuntimeActionEnvelope& envelope,
    const ActorAuthority& actor,
    const ActionAuthorityPolicy& policy
) {
    ActionAuthorityReview review{
        .sequence = envelope.sequence,
        .submitted_tick = envelope.submitted_tick,
        .current_tick = runtime.time.current_tick(),
        .action_id = envelope.action.action_id,
        .action_type = envelope.action.type,
        .actor_id = envelope.action.actor_id,
        .player_id = envelope.session.player_id,
        .faction_id = actor.faction_id,
        .target_id = envelope.action.target_id,
        .secondary_target_id = envelope.action.secondary_target_id,
        .resource_id = envelope.action.resource_id,
        .system_action_id = is_resource_action(envelope.action.type) ? std::string{system_action_trade()} : std::string{},
        .authorized = true,
    };

    append_validation(review.validation, validate_server_runtime_action_envelope(envelope));
    append_validation(review.validation, validate_actor_authority(actor));
    append_validation(review.validation, validate_action_authority_policy(policy));
    if (!review.validation.ok()) {
        review.authorized = false;
        review.rejection_reason = ActionAuthorityRejectionReason::conflict;
        return review;
    }

    if (policy.require_actor_match && envelope.action.actor_id != actor.actor_id) {
        reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.actor_id", "action actor_id does not match actor authority");
    }
    if (policy.require_actor_match && envelope.session.player_id != actor.player_id) {
        reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.player_id", "session player_id does not match actor authority");
    }

    if (envelope.submitted_tick > review.current_tick) {
        reject(review, ActionAuthorityRejectionReason::stale_target, "authority.submitted_tick", "submitted_tick is in the future");
    } else if (review.current_tick - envelope.submitted_tick > policy.max_action_age_ticks) {
        reject(review, ActionAuthorityRejectionReason::stale_target, "authority.submitted_tick", "action is older than max_action_age_ticks");
    }

    const auto action_type = std::string_view{envelope.action.type};
    const auto effective_matrix = policy.permission_matrix.rules.empty()
        ? make_default_system_permission_matrix()
        : policy.permission_matrix;
    if (action_type == runtime_action_type_advance_days && !actor.can_advance_time && !actor.host_admin) {
        reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.advance_days", "actor is not allowed to advance simulation time");
    }
    if (action_type == runtime_action_type_advance_days && !actor_has_system_permission(actor, effective_matrix, system_action_advance_time())) {
        reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.advance_time", "actor is not allowed by the system permission matrix to advance time");
    }

    if (!is_resource_action(action_type)) {
        return review;
    }

    if (!actor_has_system_permission(actor, effective_matrix, system_action_trade())) {
        reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.trade", "actor is not allowed by the system permission matrix to trade");
    }

    if (!resource_allowed(actor, envelope.action.resource_id)) {
        reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.resource_id", "actor is not allowed to use this resource");
    }

    if (!runtime.engine.has_settlement(envelope.action.target_id)) {
        reject(review, ActionAuthorityRejectionReason::invalid_target, "authority.target_id", "target settlement does not exist");
    }
    if (action_type == runtime_action_type_transfer_resource && !runtime.engine.has_settlement(envelope.action.secondary_target_id)) {
        reject(review, ActionAuthorityRejectionReason::invalid_target, "authority.secondary_target_id", "secondary target settlement does not exist");
    }

    if (policy.require_owned_resource_target && !actor_can_mutate_resource_targets(actor, policy) && runtime.engine.has_settlement(envelope.action.target_id)) {
        review.target_owner_faction_id = std::string{settlement_owner(runtime.ownership, envelope.action.target_id)};
        if (!target_owned_by_actor(runtime.ownership, envelope.action.target_id, actor)) {
            reject(review, ActionAuthorityRejectionReason::unauthorized, "authority.target_owner", "actor faction does not own target settlement");
        }
    }

    if ((action_type == runtime_action_type_remove_resource || action_type == runtime_action_type_transfer_resource)
        && runtime.engine.has_settlement(envelope.action.target_id)) {
        const auto available = runtime.engine.settlement_resource_amount(envelope.action.target_id, envelope.action.resource_id);
        if (available < envelope.action.amount) {
            reject(review, ActionAuthorityRejectionReason::insufficient_resource, "authority.resource_amount", "target settlement has insufficient resource amount");
        }
    }

    return review;
}

AuthorizedServerActionDispatchResult dispatch_authorized_server_runtime_action(
    SimulationRuntime& runtime,
    const ServerRuntimeActionEnvelope& envelope,
    const ActorAuthority& actor,
    const ActionAuthorityPolicy& policy
) {
    auto authority = review_runtime_action_authority(runtime, envelope, actor, policy);
    if (authority.authorized) {
        return AuthorizedServerActionDispatchResult{
            .authority = std::move(authority),
            .dispatch = dispatch_server_runtime_action(runtime, envelope),
        };
    }

    auto action_result = authority_rejected_action_result(envelope.action, authority);
    auto audit = make_authority_rejected_audit(runtime, envelope, action_result);
    auto validation = authority.validation;
    return AuthorizedServerActionDispatchResult{
        .authority = std::move(authority),
        .dispatch = ServerRuntimeActionDispatchResult{
            .action_result = std::move(action_result),
            .audit = std::move(audit),
            .validation = std::move(validation),
        },
    };
}

std::string action_authority_review_digest(const ActionAuthorityReview& review) {
    std::ostringstream out;
    out << "action_authority"
        << ";sequence=" << review.sequence
        << ";submitted_tick=" << review.submitted_tick
        << ";current_tick=" << review.current_tick
        << ";action=" << review.action_id
        << ";type=" << review.action_type
        << ";actor=" << review.actor_id
        << ";player=" << review.player_id
        << ";faction=" << review.faction_id
        << ";target=" << review.target_id
        << ";secondary_target=" << review.secondary_target_id
        << ";resource=" << review.resource_id
        << ";owner=" << review.target_owner_faction_id
        << ";system_action=" << review.system_action_id
        << ";authorized=" << (review.authorized ? "yes" : "no")
        << ";reason=" << action_authority_rejection_reason_name(review.rejection_reason)
        << ";diagnostics=" << review.validation.messages().size();
    return out.str();
}

} // namespace clc::sim
