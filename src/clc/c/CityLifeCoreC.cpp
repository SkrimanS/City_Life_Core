#include "clc/c/CityLifeCoreC.h"

#include "clc/core/EventLog.hpp"
#include "clc/core/Time.hpp"
#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"
#include "clc/sim/CoreCompletionReadiness.hpp"
#include "clc/sim/DeepReplayCoverage.hpp"
#include "clc/sim/GameDeveloperHandoff.hpp"
#include "clc/sim/PlatformCore.hpp"
#include "clc/sim/RegionalSimulationSystems.hpp"
#include "clc/sim/RuntimeCoreSystems.hpp"
#include "clc/sim/SdkHandoffManifest.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstddef>
#include <limits>
#include <new>
#include <string>
#include <utility>

struct clc_world {
    clc::World impl;
};

namespace {

const clc::Event* event_at(const clc_world* world, uint64_t index) {
    try {
        if (world == nullptr || index > static_cast<uint64_t>(std::numeric_limits<std::size_t>::max())) {
            return nullptr;
        }

        const auto& events = world->impl.event_log().events();
        const auto event_index = static_cast<std::size_t>(index);
        if (event_index >= events.size()) {
            return nullptr;
        }

        return &events[event_index];
    } catch (...) {
        return nullptr;
    }
}

std::string& c_api_scratch() {
    static thread_local std::string scratch;
    return scratch;
}

int advance_world_by_ticks(clc_world* world, uint64_t ticks) {
    try {
        if (world == nullptr) {
            return 0;
        }
        return world->impl.advance(ticks).ok() ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

} // namespace

clc_version clc_core_version_c(void) {
    const auto version = clc::core_version();
    return clc_version{
        .major = version.major,
        .minor = version.minor,
        .patch = version.patch,
    };
}

const char* clc_core_version_string_c(void) {
    return "4.0.0";
}

uint32_t clc_c_interface_version_c(void) {
    return 8u;
}

uint64_t clc_ticks_per_second_c(void) {
    return clc::ticks_per_second();
}

uint64_t clc_ticks_per_minute_c(void) {
    return clc::ticks_per_minute();
}

uint64_t clc_ticks_per_hour_c(void) {
    return clc::ticks_per_hour();
}

uint64_t clc_ticks_per_day_c(void) {
    return clc::ticks_per_day();
}

int clc_can_convert_seconds_to_ticks_c(uint64_t seconds) {
    return clc::can_convert_seconds_to_ticks(seconds) ? 1 : 0;
}

int clc_can_convert_minutes_to_ticks_c(uint64_t minutes) {
    return clc::can_convert_minutes_to_ticks(minutes) ? 1 : 0;
}

int clc_can_convert_hours_to_ticks_c(uint64_t hours) {
    return clc::can_convert_hours_to_ticks(hours) ? 1 : 0;
}

int clc_can_convert_days_to_ticks_c(uint64_t days) {
    return clc::can_convert_days_to_ticks(days) ? 1 : 0;
}

uint64_t clc_seconds_to_ticks_c(uint64_t seconds) {
    return clc::seconds_to_ticks(seconds);
}

uint64_t clc_minutes_to_ticks_c(uint64_t minutes) {
    return clc::minutes_to_ticks(minutes);
}

uint64_t clc_hours_to_ticks_c(uint64_t hours) {
    return clc::hours_to_ticks(hours);
}

uint64_t clc_days_to_ticks_c(uint64_t days) {
    return clc::days_to_ticks(days);
}

clc_world* clc_world_create_c(const char* name, uint64_t seed) {
    try {
        clc::WorldConfig config{
            .name = name == nullptr ? std::string{"City Life World"} : std::string{name},
            .seed = seed,
        };
        return new clc_world{clc::World{std::move(config)}};
    } catch (...) {
        return nullptr;
    }
}

void clc_world_destroy_c(clc_world* world) {
    try {
        delete world;
    } catch (...) {
    }
}

const char* clc_world_name_c(const clc_world* world) {
    try {
        if (world == nullptr) {
            return "";
        }
        return world->impl.config().name.c_str();
    } catch (...) {
        return "";
    }
}

uint64_t clc_world_seed_c(const clc_world* world) {
    try {
        if (world == nullptr) {
            return 0;
        }
        return world->impl.config().seed;
    } catch (...) {
        return 0;
    }
}

uint64_t clc_world_current_tick_c(const clc_world* world) {
    try {
        if (world == nullptr) {
            return 0;
        }
        return world->impl.time().current_tick();
    } catch (...) {
        return 0;
    }
}

uint64_t clc_world_event_count_c(const clc_world* world) {
    try {
        if (world == nullptr) {
            return 0;
        }
        return world->impl.event_log().size();
    } catch (...) {
        return 0;
    }
}

int clc_world_advance_c(clc_world* world, uint64_t ticks) {
    return advance_world_by_ticks(world, ticks);
}

int clc_world_advance_seconds_c(clc_world* world, uint64_t seconds) {
    return advance_world_by_ticks(world, clc::seconds_to_ticks(seconds));
}

int clc_world_advance_minutes_c(clc_world* world, uint64_t minutes) {
    return advance_world_by_ticks(world, clc::minutes_to_ticks(minutes));
}

int clc_world_advance_hours_c(clc_world* world, uint64_t hours) {
    return advance_world_by_ticks(world, clc::hours_to_ticks(hours));
}

int clc_world_advance_days_c(clc_world* world, uint64_t days) {
    return advance_world_by_ticks(world, clc::days_to_ticks(days));
}

uint64_t clc_world_event_id_c(const clc_world* world, uint64_t index) {
    const auto* event = event_at(world, index);
    if (event == nullptr) {
        return 0;
    }
    return event->id.value;
}

uint64_t clc_world_event_tick_c(const clc_world* world, uint64_t index) {
    const auto* event = event_at(world, index);
    if (event == nullptr) {
        return 0;
    }
    return event->tick;
}

const char* clc_world_event_type_c(const clc_world* world, uint64_t index) {
    const auto* event = event_at(world, index);
    if (event == nullptr) {
        return "";
    }
    return event->type.c_str();
}

const char* clc_world_event_payload_c(const clc_world* world, uint64_t index) {
    const auto* event = event_at(world, index);
    if (event == nullptr) {
        return "";
    }
    return event->payload.c_str();
}

uint64_t clc_supported_game_profile_count_c(void) {
    try {
        return static_cast<uint64_t>(clc::sim::game_integration_profiles().size());
    } catch (...) {
        return 0;
    }
}

const char* clc_game_profile_id_c(uint64_t index) {
    try {
        const auto& profiles = clc::sim::game_integration_profiles();
        if (index > static_cast<uint64_t>(std::numeric_limits<std::size_t>::max())) {
            return "";
        }
        const auto profile_index = static_cast<std::size_t>(index);
        if (profile_index >= profiles.size()) {
            return "";
        }
        auto& scratch = c_api_scratch();
        scratch = profiles[profile_index].id;
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}

int clc_sdk_handoff_ready_c(void) {
    try {
        for (const auto& profile : clc::sim::game_integration_profiles()) {
            if (profile.support != clc::sim::GameIntegrationProfileSupport::planned) {
                const auto report = clc::sim::make_game_developer_handoff_report(profile);
                if (!report.ready_for_game_team) {
                    return 0;
                }
            }
        }
        return 1;
    } catch (...) {
        return 0;
    }
}

const char* clc_sdk_handoff_digest_c(void) {
    try {
        auto& scratch = c_api_scratch();
        scratch = "sdk_handoff";
        scratch += ";version=";
        scratch += clc_core_version_string_c();
        scratch += ";profiles=";
        scratch += std::to_string(clc_supported_game_profile_count_c());
        scratch += ";ready=";
        scratch += clc_sdk_handoff_ready_c() ? "yes" : "no";
        scratch += ";c_interface=";
        scratch += std::to_string(clc_c_interface_version_c());
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}

const char* clc_sdk_handoff_digest_for_profile_c(const char* profile_id) {
    try {
        auto& scratch = c_api_scratch();
        scratch = clc::sim::game_developer_handoff_digest(
            clc::sim::make_game_developer_handoff_report(profile_id == nullptr ? "" : profile_id)
        );
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}

const char* clc_core_completion_readiness_digest_c(void) {
    try {
        auto& scratch = c_api_scratch();
        const auto processors = clc::sim::make_standard_core_processor_registry();
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
            .supported_profile_ids = {"native_cpp_game", "backend_service"},
        });
        scratch = clc::sim::core_completion_readiness_digest(clc::sim::make_core_completion_readiness_report({
            .processor = clc::sim::make_deep_simulation_core_readiness_report(processors),
            .platform = platform,
            .replay_coverage = clc::sim::make_deep_replay_coverage_report(clc::sim::make_standard_deep_replay_coverage_baseline(true)),
            .runtime_core_systems = clc::sim::evaluate_runtime_core_systems(clc::sim::make_basic_runtime_scenario().runtime),
            .sdk_handoff_manifest = clc::sim::make_sdk_handoff_manifest_report({
                .sdk_version = clc_core_version_string_c(),
                .c_interface_version = clc_c_interface_version_c(),
                .artifacts = clc::sim::make_standard_sdk_handoff_artifacts(true),
            }),
            .deep_domain_reports_available = true,
            .regional_reports_available = true,
            .runtime_workflow_available = true,
            .save_replay_available = true,
            .authority_available = true,
            .c_abi_handoff_available = true,
            .developer_handoff_available = true,
            .package_validation_available = true,
        }));
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}

const char* clc_runtime_core_systems_digest_c(void) {
    try {
        auto& scratch = c_api_scratch();
        auto bootstrap = clc::sim::make_basic_runtime_scenario();
        if (!bootstrap.ok()) {
            scratch = "runtime_core_systems;available=no";
            return scratch.c_str();
        }
        scratch = clc::sim::runtime_core_systems_digest(clc::sim::evaluate_runtime_core_systems(bootstrap.runtime));
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}

const char* clc_regional_simulation_digest_c(void) {
    try {
        auto& scratch = c_api_scratch();
        const auto market = clc::sim::regional_market_digest(clc::sim::evaluate_regional_market({
            .region_id = "default",
            .resource_id = "grain",
            .supply = 120,
            .demand = 100,
            .liquidity = 80,
            .volatility = 20,
            .market_depth = 80,
        }));
        const auto logistics = clc::sim::inter_region_logistics_digest(clc::sim::evaluate_inter_region_logistics({
            .route_id = "default_route",
            .source_region_id = "default",
            .target_region_id = "frontier",
            .distance = 12,
            .capacity = 80,
            .congestion = 15,
            .risk = 10,
        }));
        const auto maintenance = clc::sim::long_running_maintenance_digest(clc::sim::evaluate_long_running_maintenance({
            .region_count = 2,
            .market_count = 2,
            .route_count = 1,
            .caravan_count = 1,
            .faction_count = 1,
            .event_count = 12,
            .snapshot_count = 1,
            .ticks_since_compaction = clc::ticks_per_day(),
        }));
        scratch = "regional_simulation";
        scratch += ";market={";
        scratch += market;
        scratch += "};logistics={";
        scratch += logistics;
        scratch += "};maintenance={";
        scratch += maintenance;
        scratch += "}";
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}

const char* clc_sdk_handoff_manifest_digest_c(void) {
    try {
        auto& scratch = c_api_scratch();
        scratch = clc::sim::sdk_handoff_manifest_digest(clc::sim::make_sdk_handoff_manifest_report({
            .sdk_version = clc_core_version_string_c(),
            .c_interface_version = clc_c_interface_version_c(),
            .artifacts = clc::sim::make_standard_sdk_handoff_artifacts(true),
        }));
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}

const char* clc_deep_replay_coverage_digest_c(void) {
    try {
        auto& scratch = c_api_scratch();
        scratch = clc::sim::deep_replay_coverage_digest(
            clc::sim::make_deep_replay_coverage_report(clc::sim::make_standard_deep_replay_coverage_baseline(true))
        );
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}

const char* clc_platform_diagnostics_digest_c(void) {
    try {
        auto& scratch = c_api_scratch();
        scratch = clc::sim::platform_diagnostics_digest(clc::sim::make_platform_diagnostics_report(
            clc::sim::make_standard_platform_core_registry(),
            clc::sim::make_standard_platform_content_pack_manifest()
        ));
        return scratch.c_str();
    } catch (...) {
        return "";
    }
}
