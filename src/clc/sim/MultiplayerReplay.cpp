#include "clc/sim/MultiplayerReplay.hpp"

#include <sstream>
#include <utility>

namespace clc::sim {

MultiplayerCommandLogSummary summarize_multiplayer_command_log(
    const std::vector<MultiplayerCommandLogEntry>& entries
) {
    MultiplayerCommandLogSummary summary{.entry_count = entries.size()};
    if (entries.empty()) {
        return summary;
    }

    summary.first_sequence = entries.front().envelope.sequence;
    summary.last_sequence = entries.back().envelope.sequence;
    for (std::size_t index = 1; index < entries.size(); ++index) {
        const auto previous = entries[index - 1].envelope.sequence;
        const auto current = entries[index].envelope.sequence;
        if (current <= previous) {
            summary.strictly_increasing = false;
        }
        if (current != previous + 1) {
            summary.contiguous = false;
        }
    }
    return summary;
}

data::ValidationReport validate_multiplayer_command_log(
    const std::vector<MultiplayerCommandLogEntry>& entries,
    std::uint64_t expected_first_sequence
) {
    data::ValidationReport report;
    if (entries.empty()) {
        report.add_warning("multiplayer.command_log", "command log is empty");
        return report;
    }
    if (entries.front().envelope.sequence != expected_first_sequence) {
        report.add_error("multiplayer.command_log.first_sequence", "first sequence does not match expected_first_sequence");
    }
    for (std::size_t index = 0; index < entries.size(); ++index) {
        const auto envelope_validation = validate_server_runtime_action_envelope(entries[index].envelope);
        for (const auto& message : envelope_validation.messages()) {
            if (message.severity == data::ValidationSeverity::warning) {
                report.add_warning(message.path, message.message);
            } else {
                report.add_error(message.path, message.message);
            }
        }
        if (index > 0) {
            const auto previous = entries[index - 1].envelope.sequence;
            const auto current = entries[index].envelope.sequence;
            if (current <= previous) {
                report.add_error("multiplayer.command_log.order", "command log sequence must be strictly increasing");
            } else if (current != previous + 1) {
                report.add_error("multiplayer.command_log.gap", "command log sequence must be contiguous");
            }
        }
    }
    return report;
}

MultiplayerReplayResult replay_multiplayer_command_log(
    SimulationRuntime& runtime,
    const std::vector<MultiplayerCommandLogEntry>& entries,
    const ActionAuthorityPolicy& policy
) {
    MultiplayerReplayResult result;
    result.validation = validate_multiplayer_command_log(entries);
    for (const auto& entry : entries) {
        auto dispatch = dispatch_authorized_server_runtime_action(runtime, entry.envelope, entry.actor, policy);
        result.audits.push_back(std::move(dispatch.dispatch.audit));
    }

    auto sequence_summary = summarize_server_action_sequence(result.audits);
    result.summary.entry_count = sequence_summary.record_count;
    result.summary.accepted_count = sequence_summary.accepted_count;
    result.summary.rejected_count = sequence_summary.rejected_count;
    result.summary.first_sequence = sequence_summary.first_sequence;
    result.summary.last_sequence = sequence_summary.last_sequence;
    result.summary.contiguous = sequence_summary.contiguous;
    result.summary.strictly_increasing = sequence_summary.strictly_increasing;
    return result;
}

std::string multiplayer_command_log_summary_digest(const MultiplayerCommandLogSummary& summary) {
    std::ostringstream out;
    out << "multiplayer_command_log"
        << ";entries=" << summary.entry_count
        << ";accepted=" << summary.accepted_count
        << ";rejected=" << summary.rejected_count
        << ";first=" << summary.first_sequence
        << ";last=" << summary.last_sequence
        << ";contiguous=" << (summary.contiguous ? "yes" : "no")
        << ";strictly_increasing=" << (summary.strictly_increasing ? "yes" : "no");
    return out.str();
}

} // namespace clc::sim
