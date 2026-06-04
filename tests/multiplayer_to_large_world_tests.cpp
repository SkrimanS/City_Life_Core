#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::RuntimeAction add_resource_action(std::uint64_t amount = 5) {
    return clc::sim::RuntimeAction{
        .action_id = "add_grain",
        .type = std::string{clc::sim::runtime_action_type_add_resource},
        .actor_id = "player_a",
        .target_id = "riverwatch",
        .resource_id = "grain",
        .amount = amount,
    };
}

clc::sim::ServerRuntimeActionEnvelope envelope(std::uint64_t sequence, clc::sim::RuntimeAction action) {
    return clc::sim::ServerRuntimeActionEnvelope{
        .sequence = sequence,
        .submitted_tick = 0,
        .shard_id = "shard_a",
        .session = {.player_id = "player_a", .session_id = "session_a"},
        .action = std::move(action),
    };
}

clc::sim::ActorAuthority actor() {
    return clc::sim::ActorAuthority{
        .actor_id = "player_a",
        .player_id = "player_a",
        .faction_id = "riverwatch",
        .can_advance_time = true,
        .allowed_system_actions = {std::string{clc::sim::system_action_trade()}, std::string{clc::sim::system_action_dispatch_caravan()}},
        .allowed_resource_ids = {"grain"},
    };
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "basic runtime should bootstrap");
    auto& runtime = bootstrap.runtime;
    require(clc::sim::set_settlement_owner(runtime.ownership, "riverwatch", "riverwatch").ok(), "riverwatch owner should set");

    const auto before = runtime.engine.settlement_resource_amount("riverwatch", "grain");

    const auto authorized = clc::sim::review_runtime_action_authority(runtime, envelope(1, add_resource_action()), actor());
    require(authorized.authorized, "owned actor should be authorized");
    require(clc::sim::action_authority_review_digest(authorized).find("authorized=yes") != std::string::npos, "authority digest should show authorized");

    const auto matrix = clc::sim::make_default_system_permission_matrix();
    require(clc::sim::actor_has_system_permission(actor(), matrix, clc::sim::system_action_trade()), "actor should be allowed to trade");
    require(!clc::sim::actor_has_system_permission(actor(), matrix, clc::sim::system_action_change_policy()), "actor should not be allowed to change policy without grant");
    const auto denied_policy = clc::sim::review_system_permission(clc::sim::system_action_change_policy(), actor(), matrix);
    require(!denied_policy.authorized, "permission matrix should reject ungranted policy action");
    auto tool_actor = actor();
    tool_actor.tool_actor = true;
    tool_actor.host_admin = false;
    tool_actor.faction_id = {};
    tool_actor.allowed_system_actions = {std::string{clc::sim::system_action_inject_event()}};
    require(clc::sim::actor_has_system_permission(tool_actor, matrix, clc::sim::system_action_inject_event()), "tool actor should inject events by matrix");

    auto rejected_actor = actor();
    rejected_actor.actor_id = "other_player";
    const auto rejected = clc::sim::dispatch_authorized_server_runtime_action(runtime, envelope(1, add_resource_action()), rejected_actor);
    require(!rejected.authority.authorized, "mismatched actor should be rejected");
    require(!rejected.dispatch.action_result.accepted, "rejected actor should not dispatch");
    require(runtime.engine.settlement_resource_amount("riverwatch", "grain") == before, "rejected action should not mutate state");

    const auto accepted = clc::sim::dispatch_authorized_server_runtime_action(runtime, envelope(1, add_resource_action()), actor());
    require(accepted.authority.authorized, "matching actor should be authorized");
    require(accepted.dispatch.action_result.accepted, "authorized action should dispatch");
    require(runtime.engine.settlement_resource_amount("riverwatch", "grain") == before + 5, "accepted action should mutate state");

    const auto full_snapshot = clc::sim::make_runtime_snapshot_summary(runtime);
    require(full_snapshot.visible_settlement_count == full_snapshot.settlement_count, "full snapshot should include all settlements");
    require(clc::sim::validate_runtime_snapshot_summary(full_snapshot).ok(), "full snapshot should validate");

    const auto faction_snapshot = clc::sim::make_runtime_snapshot_summary(runtime, {
        .scope = clc::sim::SnapshotVisibilityScope::faction,
        .faction_id = "riverwatch",
    });
    require(faction_snapshot.visible_settlement_count == 1, "faction snapshot should filter settlements");
    require(clc::sim::runtime_snapshot_summary_digest(faction_snapshot).find("scope=faction") != std::string::npos, "snapshot digest should include scope");

    std::vector<clc::sim::MultiplayerCommandLogEntry> log{
        {.envelope = envelope(1, add_resource_action(1)), .actor = actor()},
        {.envelope = envelope(2, add_resource_action(2)), .actor = actor()},
    };
    auto replay_runtime = clc::sim::make_basic_runtime_scenario();
    require(replay_runtime.ok(), "replay runtime should bootstrap");
    require(clc::sim::set_settlement_owner(replay_runtime.runtime.ownership, "riverwatch", "riverwatch").ok(), "replay owner should set");
    const auto replay = clc::sim::replay_multiplayer_command_log(replay_runtime.runtime, log);
    require(replay.summary.entry_count == 2, "replay should count entries");
    require(replay.summary.accepted_count == 2, "replay should accept authorized entries");
    require(clc::sim::validate_multiplayer_command_log(log).ok(), "command log should validate");

    const auto price = clc::economy::MarketPrice{.resource_id = "grain", .base_value = 4, .supply = 10, .demand = 20, .price = 5};
    runtime.wallet.coins = 100;
    const auto buy_review = clc::sim::review_buy_resource_safety(runtime, "player_a", "riverwatch", price, 4, "buy-1");
    require(buy_review.allowed, "buy should be allowed with enough coins");
    const auto sell_review = clc::sim::review_sell_resource_safety(runtime, "player_a", "riverwatch", price, 999999, "sell-1");
    require(!sell_review.allowed, "sell should reject insufficient resources");

    const std::vector<clc::sim::ServerRuntimeActionAuditRecord> audits{accepted.dispatch.audit, rejected.dispatch.audit};
    const auto load = clc::sim::make_multiplayer_load_snapshot(runtime, audits, 2);
    require(load.action_count == 2, "load snapshot should count actions");
    require(load.rejected_action_count == 1, "load snapshot should count rejected actions");
    require(clc::sim::validate_multiplayer_load_snapshot(load).ok(), "load snapshot should validate");

    const auto readiness = clc::sim::make_pre3_readiness_report(true, true, true, true, load, true);
    require(readiness.status == "ready", "pre3 readiness should be ready");
    require(clc::sim::validate_pre3_readiness_report(readiness).ok(), "readiness should validate");

    clc::sim::SimulationProcessorRegistry processors;
    require(clc::sim::add_simulation_feature(processors, {.feature_id = "market.depth", .enabled = true}).ok(), "processor feature should add");
    require(clc::sim::add_simulation_feature(processors, {.feature_id = "weather", .enabled = false}).ok(), "disabled feature should add");
    require(clc::sim::add_simulation_dependency(processors, {
        .dependency_id = "weather.market",
        .source_system_id = "weather",
        .target_system_id = "market",
        .enabled = false,
    }).ok(), "processor dependency should add");
    require(clc::sim::add_simulation_processor(processors, {
        .processor_id = "market",
        .deterministic_order = 20,
        .required_feature_ids = {"market.depth"},
    }).ok(), "market processor should add");
    require(clc::sim::add_simulation_processor(processors, {
        .processor_id = "weather_market_effects",
        .deterministic_order = 30,
        .required_feature_ids = {"weather"},
        .required_dependency_ids = {"weather.market"},
    }).ok(), "gated processor should add");
    const auto processor_report = clc::sim::make_simulation_processor_run_report(processors);
    require(processor_report.enabled_processor_count == 1, "only enabled processor should run");
    require(processor_report.skipped_processor_count == 1, "disabled dependency processor should skip");
    const auto core_readiness = clc::sim::make_deep_simulation_core_readiness_report(processors);
    require(core_readiness.processor_architecture_ready, "processor architecture should be ready");
    require(core_readiness.dependency_toggles_ready, "dependency toggles should be ready");

    clc::sim::LargeWorldCatalog large_world;
    require(clc::sim::add_world_region(large_world, {.region_id = "north", .display_name = "North", .shard_id = "shard_a"}).ok(), "region should add");
    require(clc::sim::add_world_region(large_world, {.region_id = "south", .display_name = "South", .shard_id = "shard_a"}).ok(), "second region should add");
    require(clc::sim::assign_settlement_region(large_world, "riverwatch", "north").ok(), "settlement region should assign");
    require(clc::sim::add_regional_feature_config(large_world, {
        .region_id = "north",
        .enabled_feature_ids = {"market.depth"},
        .disabled_feature_ids = {"weather"},
    }).ok(), "regional feature config should add");
    require(clc::sim::add_regional_dependency_config(large_world, {
        .region_id = "north",
        .dependency_id = "weather.market",
        .source_system_id = "weather",
        .target_system_id = "market",
        .enabled = false,
    }).ok(), "regional dependency config should add");
    require(clc::sim::add_cross_region_reference(large_world, {
        .reference_id = "north-south-trade",
        .source_region_id = "north",
        .target_region_id = "south",
        .reference_type = "trade_route",
        .object_id = "route-a",
    }).ok(), "cross-region reference should add");
    const auto large_summary = clc::sim::make_large_world_summary(runtime, large_world);
    require(large_summary.region_count == 2, "large-world summary should count regions");
    require(large_summary.regional_feature_config_count == 1, "large-world summary should count feature configs");
    require(large_summary.cross_region_reference_count == 1, "large-world summary should count cross-region refs");
    require(clc::sim::validate_large_world_catalog(runtime, large_world).ok(), "large-world catalog should validate");
    require(clc::sim::large_world_summary_digest(large_summary).find("regions=2") != std::string::npos, "large-world digest should count regions");
    const auto regional_snapshot = clc::sim::make_regional_snapshot_summary(runtime, large_world, "north");
    require(regional_snapshot.visible_settlement_count == 1, "regional snapshot should count assigned settlement");
    require(regional_snapshot.enabled_feature_count == 1, "regional snapshot should count enabled features");
    clc::EventLog events;
    events.append(42, "regional_event", "region=north;market=grain");
    events.append(43, "regional_event", "region=south;market=grain");
    const auto regional_events = clc::sim::make_regional_event_stream_summary(large_world, events, "north");
    require(regional_events.event_count == 1, "regional event stream should filter by region");

    const auto platform = clc::sim::make_platform_readiness_report({
        .module_boundaries_documented = true,
        .profile_requirements_documented = true,
        .content_pipeline_requirements_documented = true,
        .editor_admin_api_requirements_documented = true,
        .diagnostics_api_requirements_documented = true,
        .cxx_api_ready = true,
        .c_abi_readonly_diagnostics_ready = true,
        .save_load_migration_ready = true,
        .replay_migration_ready = true,
        .regional_large_world_ready = true,
        .supported_profile_ids = {"native_cpp", "unity_c_abi", "server_authoritative", "tools"},
    });
    require(platform.ready, "platform readiness should be ready with all gates satisfied");
    require(clc::sim::platform_readiness_digest(platform).find("target=4.0.0") != std::string::npos, "platform readiness digest should include target");

    return 0;
}
