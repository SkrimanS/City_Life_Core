#include "clc/CityLifeCore.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

bool ok(const clc::data::ValidationReport& report) {
    if (report.ok()) {
        return true;
    }
    for (const auto& message : report.messages()) {
        std::cerr << message.path << ": " << message.message << '\n';
    }
    return false;
}

clc::sim::RuntimeAction add_grain_action(std::uint64_t amount) {
    return clc::sim::RuntimeAction{
        .action_id = "add_grain_" + std::to_string(amount),
        .type = std::string{clc::sim::runtime_action_type_add_resource},
        .actor_id = "player_a",
        .target_id = "riverwatch",
        .resource_id = "grain",
        .amount = amount,
    };
}

clc::sim::ServerRuntimeActionEnvelope make_envelope(std::uint64_t sequence, clc::sim::RuntimeAction action) {
    return clc::sim::ServerRuntimeActionEnvelope{
        .sequence = sequence,
        .submitted_tick = 0,
        .shard_id = "shard_a",
        .session = {.player_id = "player_a", .session_id = "session_a"},
        .action = std::move(action),
    };
}

clc::sim::ActorAuthority make_actor() {
    return clc::sim::ActorAuthority{
        .actor_id = "player_a",
        .player_id = "player_a",
        .faction_id = "riverwatch",
        .can_advance_time = true,
        .allowed_resource_ids = {"grain"},
    };
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;
    if (!ok(clc::sim::set_settlement_owner(runtime.ownership, "riverwatch", "riverwatch"))) {
        return 1;
    }

    const auto actor = make_actor();
    const auto first = clc::sim::dispatch_authorized_server_runtime_action(
        runtime,
        make_envelope(1, add_grain_action(5)),
        actor
    );
    std::cout << clc::sim::action_authority_review_digest(first.authority) << '\n';
    std::cout << clc::sim::server_runtime_action_audit_digest(first.dispatch.audit) << '\n';

    const auto snapshot = clc::sim::make_runtime_snapshot_summary(runtime, {
        .scope = clc::sim::SnapshotVisibilityScope::faction,
        .faction_id = "riverwatch",
    });
    std::cout << clc::sim::runtime_snapshot_summary_digest(snapshot) << '\n';

    std::vector<clc::sim::MultiplayerCommandLogEntry> log{
        {.envelope = make_envelope(1, add_grain_action(1)), .actor = actor},
        {.envelope = make_envelope(2, add_grain_action(2)), .actor = actor},
    };
    auto replay = clc::sim::replay_multiplayer_command_log(runtime, log);
    std::cout << clc::sim::multiplayer_command_log_summary_digest(replay.summary) << '\n';

    const auto price = clc::economy::MarketPrice{.resource_id = "grain", .base_value = 4, .supply = 10, .demand = 20, .price = 5};
    runtime.wallet.coins = 100;
    const auto economy = clc::sim::review_buy_resource_safety(runtime, "player_a", "riverwatch", price, 4, "buy-1");
    std::cout << clc::sim::economy_safety_review_digest(economy) << '\n';

    std::vector<clc::sim::ServerRuntimeActionAuditRecord> audits{first.dispatch.audit};
    audits.insert(audits.end(), replay.audits.begin(), replay.audits.end());
    const auto load = clc::sim::make_multiplayer_load_snapshot(runtime, audits, 1);
    std::cout << clc::sim::multiplayer_load_snapshot_digest(load) << '\n';

    const auto readiness = clc::sim::make_pre3_readiness_report(true, true, true, true, load, true);
    std::cout << clc::sim::pre3_readiness_report_digest(readiness) << '\n';

    clc::sim::LargeWorldCatalog large_world;
    if (!ok(clc::sim::add_world_region(large_world, {.region_id = "north", .display_name = "North", .shard_id = "shard_a"}))
        || !ok(clc::sim::assign_settlement_region(large_world, "riverwatch", "north"))) {
        return 1;
    }
    std::cout << clc::sim::large_world_summary_digest(clc::sim::make_large_world_summary(runtime, large_world)) << '\n';

    return first.dispatch.action_result.accepted && replay.validation.ok() && economy.allowed ? 0 : 1;
}
