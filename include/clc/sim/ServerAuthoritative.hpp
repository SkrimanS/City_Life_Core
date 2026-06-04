#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/ActionBridge.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct ServerSessionIdentity final {
    std::string player_id{};
    std::string session_id{};
    std::string connection_id{};
};

struct ServerShardDescriptor final {
    std::string shard_id{};
    std::string owner_service_id{};
    std::string region_id{};
    std::string partition_id{};
    bool authoritative{true};
};

struct ServerRuntimeActionEnvelope final {
    std::uint64_t sequence{0};
    std::uint64_t submitted_tick{0};
    std::string shard_id{};
    ServerSessionIdentity session{};
    RuntimeAction action{};
};

struct ServerRuntimeActionAuditRecord final {
    std::uint64_t sequence{0};
    std::uint64_t submitted_tick{0};
    std::uint64_t resulting_day{0};
    std::string shard_id{};
    std::string action_id{};
    std::string action_type{};
    std::string actor_id{};
    std::string player_id{};
    std::string session_id{};
    bool accepted{false};
    std::string validation_status{};
    std::string error_code{};
    std::uint64_t event_count{0};
    std::uint64_t diagnostic_count{0};
};

struct ServerRuntimeActionDispatchResult final {
    RuntimeActionResult action_result{};
    ServerRuntimeActionAuditRecord audit{};
    data::ValidationReport validation{};
};

struct ServerActionSequenceSummary final {
    std::uint64_t record_count{0};
    std::uint64_t accepted_count{0};
    std::uint64_t rejected_count{0};
    std::uint64_t first_sequence{0};
    std::uint64_t last_sequence{0};
    bool contiguous{true};
    bool strictly_increasing{true};
};

[[nodiscard]] data::ValidationReport validate_server_session_identity(const ServerSessionIdentity& session);
[[nodiscard]] data::ValidationReport validate_server_shard_descriptor(const ServerShardDescriptor& shard);
[[nodiscard]] data::ValidationReport validate_server_runtime_action_envelope(const ServerRuntimeActionEnvelope& envelope);

[[nodiscard]] ServerRuntimeActionDispatchResult dispatch_server_runtime_action(
    SimulationRuntime& runtime,
    const ServerRuntimeActionEnvelope& envelope
);

[[nodiscard]] ServerActionSequenceSummary summarize_server_action_sequence(
    const std::vector<ServerRuntimeActionAuditRecord>& records
);

[[nodiscard]] data::ValidationReport validate_server_action_sequence(
    const std::vector<ServerRuntimeActionAuditRecord>& records,
    std::uint64_t expected_first_sequence = 1
);

[[nodiscard]] std::string server_shard_descriptor_digest(const ServerShardDescriptor& shard);
[[nodiscard]] std::string server_runtime_action_audit_digest(const ServerRuntimeActionAuditRecord& audit);
[[nodiscard]] std::string server_action_sequence_summary_digest(const ServerActionSequenceSummary& summary);
[[nodiscard]] const ServerRuntimeActionAuditRecord* server_action_audit_by_sequence(
    const std::vector<ServerRuntimeActionAuditRecord>& records,
    std::uint64_t sequence
) noexcept;

} // namespace clc::sim
