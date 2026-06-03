#include "clc/CityLifeCore.hpp"

#include <iostream>
#include <vector>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        std::cerr << "failed to create runtime scenario\n";
        return 1;
    }

    clc::sim::ServerShardDescriptor shard{
        .shard_id = "riverlands-1",
        .owner_service_id = "simulation-service-a",
        .region_id = "local",
        .partition_id = "riverlands",
    };

    const auto shard_validation = clc::sim::validate_server_shard_descriptor(shard);
    if (!shard_validation.ok()) {
        std::cerr << "invalid shard descriptor\n";
        return 1;
    }

    const clc::sim::ServerSessionIdentity session{
        .player_id = "player-a",
        .session_id = "session-a",
        .connection_id = "connection-a",
    };

    std::vector<clc::sim::ServerRuntimeActionAuditRecord> audit_log;

    const clc::sim::ServerRuntimeActionEnvelope add_grain{
        .sequence = 1,
        .submitted_tick = bootstrap.runtime.time.current_tick(),
        .shard_id = shard.shard_id,
        .session = session,
        .action = clc::sim::RuntimeAction{
            .action_id = "server-add-grain",
            .type = std::string{clc::sim::runtime_action_type_add_resource},
            .actor_id = "player-a",
            .target_id = "riverwatch",
            .resource_id = "grain",
            .amount = 10,
        },
    };

    const auto add_result = clc::sim::dispatch_server_runtime_action(bootstrap.runtime, add_grain);
    audit_log.push_back(add_result.audit);

    const clc::sim::ServerRuntimeActionEnvelope advance_day{
        .sequence = 2,
        .submitted_tick = bootstrap.runtime.time.current_tick(),
        .shard_id = shard.shard_id,
        .session = session,
        .action = clc::sim::RuntimeAction{
            .action_id = "server-advance-day",
            .type = std::string{clc::sim::runtime_action_type_advance_days},
            .actor_id = "player-a",
            .days = 1,
        },
    };

    const auto advance_result = clc::sim::dispatch_server_runtime_action(bootstrap.runtime, advance_day);
    audit_log.push_back(advance_result.audit);

    const auto sequence_validation = clc::sim::validate_server_action_sequence(audit_log);
    if (!sequence_validation.ok()) {
        std::cerr << "server action sequence is invalid\n";
        return 1;
    }

    const auto sequence_summary = clc::sim::summarize_server_action_sequence(audit_log);

    std::cout << clc::sim::server_shard_descriptor_digest(shard) << '\n';
    for (const auto& audit : audit_log) {
        std::cout << clc::sim::server_runtime_action_audit_digest(audit) << '\n';
    }
    std::cout << clc::sim::server_action_sequence_summary_digest(sequence_summary) << '\n';
    std::cout << "riverwatch grain=" << bootstrap.runtime.engine.settlement_resource_amount("riverwatch", "grain") << '\n';
    std::cout << "current day=" << bootstrap.runtime.engine.current_day() << '\n';

    return add_result.action_result.accepted && advance_result.action_result.accepted ? 0 : 1;
}
