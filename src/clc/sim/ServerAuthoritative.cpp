#include "clc/sim/ServerAuthoritative.hpp"

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

std::string status_or_invalid(const RuntimeActionResult& result, const data::ValidationReport& validation) {
    if (!result.validation_status.empty()) {
        return result.validation_status;
    }
    return validation.ok() ? std::string{runtime_action_status_accepted} : std::string{runtime_action_status_invalid};
}

} // namespace

data::ValidationReport validate_server_session_identity(const ServerSessionIdentity& session) {
    data::ValidationReport report;
    if (session.player_id.empty()) {
        report.add_error("server.session.player_id", "player_id must not be empty");
    }
    if (session.session_id.empty()) {
        report.add_error("server.session.session_id", "session_id must not be empty");
    }
    return report;
}

data::ValidationReport validate_server_shard_descriptor(const ServerShardDescriptor& shard) {
    data::ValidationReport report;
    if (shard.shard_id.empty()) {
        report.add_error("server.shard.shard_id", "shard_id must not be empty");
    }
    if (shard.owner_service_id.empty()) {
        report.add_error("server.shard.owner_service_id", "owner_service_id must not be empty");
    }
    if (!shard.authoritative) {
        report.add_warning("server.shard.authoritative", "non-authoritative shard descriptors are informational only");
    }
    return report;
}

data::ValidationReport validate_server_runtime_action_envelope(const ServerRuntimeActionEnvelope& envelope) {
    data::ValidationReport report;
    if (envelope.sequence == 0) {
        report.add_error("server.action.sequence", "sequence must be greater than zero");
    }
    if (envelope.shard_id.empty()) {
        report.add_error("server.action.shard_id", "shard_id must not be empty");
    }
    append_validation(report, validate_server_session_identity(envelope.session));
    append_validation(report, validate_runtime_action(envelope.action));
    if (!envelope.action.actor_id.empty() && envelope.action.actor_id != envelope.session.player_id) {
        report.add_warning("server.action.actor_id", "actor_id differs from host-owned player_id");
    }
    return report;
}

ServerRuntimeActionDispatchResult dispatch_server_runtime_action(
    SimulationRuntime& runtime,
    const ServerRuntimeActionEnvelope& envelope
) {
    auto envelope_validation = validate_server_runtime_action_envelope(envelope);
    RuntimeActionResult action_result;

    if (envelope_validation.ok()) {
        action_result = dispatch_runtime_action(runtime.engine, envelope.action);
    } else {
        action_result = RuntimeActionResult{
            .action_id = envelope.action.action_id,
            .type = envelope.action.type,
            .accepted = false,
            .validation_status = std::string{runtime_action_status_invalid},
            .error_code = std::string{runtime_action_error_invalid_action},
            .message = envelope_validation.messages().empty() ? std::string{"invalid server action envelope"} : envelope_validation.messages().front().message,
            .validation = envelope_validation,
        };
    }

    ServerRuntimeActionAuditRecord audit{
        .sequence = envelope.sequence,
        .submitted_tick = envelope.submitted_tick,
        .resulting_day = runtime.engine.current_day(),
        .shard_id = envelope.shard_id,
        .action_id = envelope.action.action_id,
        .action_type = envelope.action.type,
        .actor_id = envelope.action.actor_id,
        .player_id = envelope.session.player_id,
        .session_id = envelope.session.session_id,
        .accepted = action_result.accepted,
        .validation_status = status_or_invalid(action_result, envelope_validation),
        .error_code = action_result.error_code,
        .event_count = action_result.events.size(),
        .diagnostic_count = action_result.validation.messages().size(),
    };

    return ServerRuntimeActionDispatchResult{
        .action_result = std::move(action_result),
        .audit = std::move(audit),
        .validation = std::move(envelope_validation),
    };
}

ServerActionSequenceSummary summarize_server_action_sequence(
    const std::vector<ServerRuntimeActionAuditRecord>& records
) {
    ServerActionSequenceSummary summary{
        .record_count = records.size(),
        .contiguous = true,
        .strictly_increasing = true,
    };

    if (records.empty()) {
        return summary;
    }

    summary.first_sequence = records.front().sequence;
    summary.last_sequence = records.back().sequence;

    for (std::size_t index = 0; index < records.size(); ++index) {
        const auto& record = records[index];
        if (record.accepted) {
            ++summary.accepted_count;
        } else {
            ++summary.rejected_count;
        }

        if (index > 0) {
            const auto previous = records[index - 1].sequence;
            if (record.sequence <= previous) {
                summary.strictly_increasing = false;
            }
            if (record.sequence != previous + 1) {
                summary.contiguous = false;
            }
        }
    }

    return summary;
}

data::ValidationReport validate_server_action_sequence(
    const std::vector<ServerRuntimeActionAuditRecord>& records,
    std::uint64_t expected_first_sequence
) {
    data::ValidationReport report;
    if (records.empty()) {
        report.add_warning("server.sequence", "action audit sequence is empty");
        return report;
    }
    if (records.front().sequence != expected_first_sequence) {
        report.add_error("server.sequence.first", "first sequence does not match expected first sequence");
    }

    for (std::size_t index = 1; index < records.size(); ++index) {
        const auto previous = records[index - 1].sequence;
        const auto current = records[index].sequence;
        if (current <= previous) {
            report.add_error("server.sequence.order", "action audit sequence must be strictly increasing");
        } else if (current != previous + 1) {
            report.add_error("server.sequence.gap", "action audit sequence must be contiguous");
        }
    }
    return report;
}

std::string server_shard_descriptor_digest(const ServerShardDescriptor& shard) {
    std::ostringstream out;
    out << "server_shard"
        << ";id=" << shard.shard_id
        << ";owner=" << shard.owner_service_id
        << ";region=" << shard.region_id
        << ";partition=" << shard.partition_id
        << ";authoritative=" << (shard.authoritative ? "yes" : "no");
    return out.str();
}

std::string server_runtime_action_audit_digest(const ServerRuntimeActionAuditRecord& audit) {
    std::ostringstream out;
    out << "server_action_audit"
        << ";sequence=" << audit.sequence
        << ";shard=" << audit.shard_id
        << ";action=" << audit.action_id
        << ";type=" << audit.action_type
        << ";actor=" << audit.actor_id
        << ";player=" << audit.player_id
        << ";session=" << audit.session_id
        << ";accepted=" << (audit.accepted ? "yes" : "no")
        << ";status=" << audit.validation_status
        << ";error=" << audit.error_code
        << ";events=" << audit.event_count
        << ";diagnostics=" << audit.diagnostic_count
        << ";day=" << audit.resulting_day;
    return out.str();
}

std::string server_action_sequence_summary_digest(const ServerActionSequenceSummary& summary) {
    std::ostringstream out;
    out << "server_action_sequence"
        << ";records=" << summary.record_count
        << ";accepted=" << summary.accepted_count
        << ";rejected=" << summary.rejected_count
        << ";first=" << summary.first_sequence
        << ";last=" << summary.last_sequence
        << ";contiguous=" << (summary.contiguous ? "yes" : "no")
        << ";strictly_increasing=" << (summary.strictly_increasing ? "yes" : "no");
    return out.str();
}

const ServerRuntimeActionAuditRecord* server_action_audit_by_sequence(
    const std::vector<ServerRuntimeActionAuditRecord>& records,
    std::uint64_t sequence
) noexcept {
    const auto found = std::find_if(records.begin(), records.end(), [sequence](const ServerRuntimeActionAuditRecord& record) {
        return record.sequence == sequence;
    });
    return found == records.end() ? nullptr : &*found;
}

} // namespace clc::sim
