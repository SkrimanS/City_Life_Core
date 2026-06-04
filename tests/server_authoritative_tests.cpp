#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::ServerSessionIdentity session() {
    return clc::sim::ServerSessionIdentity{
        .player_id = "player-a",
        .session_id = "session-a",
        .connection_id = "connection-a",
    };
}

clc::sim::RuntimeAction add_grain_action(std::string action_id, std::uint64_t amount = 5) {
    return clc::sim::RuntimeAction{
        .action_id = std::move(action_id),
        .type = std::string{clc::sim::runtime_action_type_add_resource},
        .actor_id = "player-a",
        .target_id = "riverwatch",
        .resource_id = "grain",
        .amount = amount,
    };
}

void shard_descriptor_validation_and_digest() {
    const clc::sim::ServerShardDescriptor shard{
        .shard_id = "shard-riverlands-1",
        .owner_service_id = "simulation-service-a",
        .region_id = "eu",
        .partition_id = "riverlands",
    };

    const auto validation = clc::sim::validate_server_shard_descriptor(shard);
    require(validation.ok(), "valid shard descriptor should pass validation");

    const auto digest = clc::sim::server_shard_descriptor_digest(shard);
    require(digest.find("id=shard-riverlands-1") != std::string::npos, "shard digest should include id");
    require(digest.find("authoritative=yes") != std::string::npos, "shard digest should include authoritative flag");

    const auto invalid = clc::sim::validate_server_shard_descriptor(clc::sim::ServerShardDescriptor{});
    require(!invalid.ok(), "empty shard descriptor should fail validation");
}

void authoritative_dispatch_records_host_session_separately_from_actor() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "basic runtime scenario should bootstrap");

    const auto before = bootstrap.runtime.engine.settlement_resource_amount("riverwatch", "grain");
    const clc::sim::ServerRuntimeActionEnvelope envelope{
        .sequence = 1,
        .submitted_tick = bootstrap.runtime.time.current_tick(),
        .shard_id = "shard-riverlands-1",
        .session = session(),
        .action = add_grain_action("server-add-1", 7),
    };

    const auto result = clc::sim::dispatch_server_runtime_action(bootstrap.runtime, envelope);
    require(result.validation.ok(), "valid server envelope should pass validation");
    require(result.action_result.accepted, "valid server action should be accepted");
    require(result.audit.accepted, "audit should mark accepted action");
    require(result.audit.sequence == 1, "audit should preserve sequence");
    require(result.audit.actor_id == "player-a", "audit should preserve simulation actor");
    require(result.audit.player_id == "player-a", "audit should preserve host player id");
    require(result.audit.session_id == "session-a", "audit should preserve host session id");
    require(result.audit.event_count == 1, "audit should report produced event");
    require(bootstrap.runtime.engine.settlement_resource_amount("riverwatch", "grain") == before + 7, "accepted action should mutate runtime");

    const auto digest = clc::sim::server_runtime_action_audit_digest(result.audit);
    require(digest.find("sequence=1") != std::string::npos, "audit digest should include sequence");
    require(digest.find("accepted=yes") != std::string::npos, "audit digest should include accepted flag");
}

void invalid_envelope_rejects_before_runtime_mutation() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "basic runtime scenario should bootstrap");

    const auto before = bootstrap.runtime.engine.settlement_resource_amount("riverwatch", "grain");
    const clc::sim::ServerRuntimeActionEnvelope envelope{
        .sequence = 0,
        .shard_id = "shard-riverlands-1",
        .session = session(),
        .action = add_grain_action("server-invalid-sequence", 7),
    };

    const auto result = clc::sim::dispatch_server_runtime_action(bootstrap.runtime, envelope);
    require(!result.validation.ok(), "zero sequence should fail envelope validation");
    require(!result.action_result.accepted, "invalid envelope should reject action");
    require(result.audit.validation_status == clc::sim::runtime_action_status_invalid, "audit should mark invalid status");
    require(bootstrap.runtime.engine.settlement_resource_amount("riverwatch", "grain") == before, "invalid envelope should not mutate runtime");
}

void sequence_summary_and_validation_detect_gaps() {
    std::vector<clc::sim::ServerRuntimeActionAuditRecord> records{
        clc::sim::ServerRuntimeActionAuditRecord{.sequence = 1, .accepted = true},
        clc::sim::ServerRuntimeActionAuditRecord{.sequence = 2, .accepted = false},
        clc::sim::ServerRuntimeActionAuditRecord{.sequence = 3, .accepted = true},
    };

    const auto summary = clc::sim::summarize_server_action_sequence(records);
    require(summary.record_count == 3, "summary should count records");
    require(summary.accepted_count == 2, "summary should count accepted records");
    require(summary.rejected_count == 1, "summary should count rejected records");
    require(summary.contiguous, "contiguous sequence should be marked contiguous");
    require(summary.strictly_increasing, "ordered sequence should be strictly increasing");
    require(clc::sim::validate_server_action_sequence(records).ok(), "contiguous sequence should validate");
    require(clc::sim::server_action_audit_by_sequence(records, 2) != nullptr, "sequence lookup should find record");

    records[2].sequence = 5;
    const auto gap_summary = clc::sim::summarize_server_action_sequence(records);
    require(!gap_summary.contiguous, "sequence gap should be reported");
    require(!clc::sim::validate_server_action_sequence(records).ok(), "sequence gap should fail validation");
}

void long_running_authoritative_tick_window_is_stable() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "basic runtime scenario should bootstrap");

    const auto run = clc::sim::run_runtime_ticks(
        bootstrap.runtime,
        clc::days_to_ticks(3),
        clc::hours_to_ticks(6)
    );

    require(run.ok(), "long-running authoritative tick smoke should complete");
    require(bootstrap.runtime.time.current_tick() == clc::days_to_ticks(3), "runtime tick clock should advance to three days");
}

} // namespace

int main() {
    shard_descriptor_validation_and_digest();
    authoritative_dispatch_records_host_session_separately_from_actor();
    invalid_envelope_rejects_before_runtime_mutation();
    sequence_summary_and_validation_detect_gaps();
    long_running_authoritative_tick_window_is_stable();
    return 0;
}
