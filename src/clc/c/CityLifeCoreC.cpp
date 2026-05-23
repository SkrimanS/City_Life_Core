#include "clc/c/CityLifeCoreC.h"

#include "clc/core/EventLog.hpp"
#include "clc/core/Time.hpp"
#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"

#include <limits>
#include <new>
#include <string>
#include <utility>

struct clc_world {
    clc::World impl;
};

namespace {

const clc::Event* event_at(const clc_world* world, uint64_t index) noexcept {
    if (world == nullptr || index > static_cast<uint64_t>(std::numeric_limits<std::size_t>::max())) {
        return nullptr;
    }

    const auto& events = world->impl.event_log().events();
    const auto event_index = static_cast<std::size_t>(index);
    if (event_index >= events.size()) {
        return nullptr;
    }

    return &events[event_index];
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
    return "0.9.9";
}

uint32_t clc_c_interface_version_c(void) {
    return 3u;
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
    delete world;
}

const char* clc_world_name_c(const clc_world* world) {
    if (world == nullptr) {
        return "";
    }
    return world->impl.config().name.c_str();
}

uint64_t clc_world_seed_c(const clc_world* world) {
    if (world == nullptr) {
        return 0;
    }
    return world->impl.config().seed;
}

uint64_t clc_world_current_tick_c(const clc_world* world) {
    if (world == nullptr) {
        return 0;
    }
    return world->impl.time().current_tick();
}

uint64_t clc_world_event_count_c(const clc_world* world) {
    if (world == nullptr) {
        return 0;
    }
    return world->impl.event_log().size();
}

int clc_world_advance_c(clc_world* world, uint64_t ticks) {
    if (world == nullptr) {
        return 0;
    }
    return world->impl.advance(ticks).ok() ? 1 : 0;
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
