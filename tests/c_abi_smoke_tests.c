#include "clc/c/CityLifeCoreC.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int require_int(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "Test failed: %s\n", message);
        return 1;
    }
    return 0;
}

int main(void) {
    const clc_version version = clc_core_version_c();
    if (require_int(version.major == 1, "major version should be 1")) {
        return 1;
    }
    if (require_int(version.minor == 0, "minor version should be 0")) {
        return 1;
    }
    if (require_int(version.patch == 0, "patch version should be 0")) {
        return 1;
    }
    if (require_int(strcmp(clc_core_version_string_c(), "1.0.0") == 0, "version string should match")) {
        return 1;
    }
    if (require_int(clc_c_interface_version_c() == 4u, "C interface version should be 4")) {
        return 1;
    }

    if (require_int(clc_ticks_per_second_c() == 1, "ticks per second should be 1")) {
        return 1;
    }
    if (require_int(clc_ticks_per_minute_c() == 60, "ticks per minute should be 60")) {
        return 1;
    }
    if (require_int(clc_ticks_per_hour_c() == 3600, "ticks per hour should be 3600")) {
        return 1;
    }
    if (require_int(clc_ticks_per_day_c() == 86400, "ticks per day should be 86400")) {
        return 1;
    }

    if (require_int(clc_minutes_to_ticks_c(5) == 300, "5 minutes should convert to 300 ticks")) {
        return 1;
    }
    if (require_int(clc_hours_to_ticks_c(2) == 7200, "2 hours should convert to 7200 ticks")) {
        return 1;
    }
    if (require_int(clc_days_to_ticks_c(1) == 86400, "1 day should convert to 86400 ticks")) {
        return 1;
    }

    if (require_int(clc_can_convert_days_to_ticks_c(UINT64_MAX) == 0, "huge day conversion should not fit")) {
        return 1;
    }
    if (require_int(clc_days_to_ticks_c(UINT64_MAX) == UINT64_MAX, "huge day conversion should saturate")) {
        return 1;
    }

    if (require_int(clc_world_name_c(NULL)[0] == '\0', "null world name should be empty")) {
        return 1;
    }
    if (require_int(clc_world_seed_c(NULL) == 0, "null world seed should be zero")) {
        return 1;
    }
    if (require_int(clc_world_current_tick_c(NULL) == 0, "null world tick should be zero")) {
        return 1;
    }
    if (require_int(clc_world_event_count_c(NULL) == 0, "null world event count should be zero")) {
        return 1;
    }
    if (require_int(clc_world_event_id_c(NULL, 0) == 0, "null world event id should be zero")) {
        return 1;
    }
    if (require_int(clc_world_event_tick_c(NULL, 0) == 0, "null world event tick should be zero")) {
        return 1;
    }
    if (require_int(clc_world_event_type_c(NULL, 0)[0] == '\0', "null world event type should be empty")) {
        return 1;
    }
    if (require_int(clc_world_event_payload_c(NULL, 0)[0] == '\0', "null world event payload should be empty")) {
        return 1;
    }
    if (require_int(clc_world_event_id_c(NULL, UINT64_MAX) == 0, "null world max event id should be zero")) {
        return 1;
    }
    if (require_int(clc_world_event_tick_c(NULL, UINT64_MAX) == 0, "null world max event tick should be zero")) {
        return 1;
    }
    if (require_int(clc_world_event_type_c(NULL, UINT64_MAX)[0] == '\0', "null world max event type should be empty")) {
        return 1;
    }
    if (require_int(clc_world_event_payload_c(NULL, UINT64_MAX)[0] == '\0', "null world max event payload should be empty")) {
        return 1;
    }
    if (require_int(clc_world_advance_c(NULL, 1) == 0, "null world advance should fail")) {
        return 1;
    }
    if (require_int(clc_world_advance_seconds_c(NULL, 1) == 0, "null world seconds advance should fail")) {
        return 1;
    }
    if (require_int(clc_world_advance_minutes_c(NULL, 1) == 0, "null world minutes advance should fail")) {
        return 1;
    }
    if (require_int(clc_world_advance_hours_c(NULL, 1) == 0, "null world hours advance should fail")) {
        return 1;
    }
    if (require_int(clc_world_advance_days_c(NULL, 1) == 0, "null world days advance should fail")) {
        return 1;
    }
    clc_world_destroy_c(NULL);

    clc_world* world = clc_world_create_c("C ABI World", 42);
    if (require_int(world != NULL, "world handle should create")) {
        return 1;
    }
    if (require_int(strcmp(clc_world_name_c(world), "C ABI World") == 0, "world name should be preserved")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_seed_c(world) == 42, "world seed should be preserved")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == 0, "new world should start at tick zero")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_count_c(world) == 1, "new world should have creation event")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_id_c(world, 0) == 1, "creation event id should be one")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_tick_c(world, 0) == 0, "creation event tick should be zero")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(strcmp(clc_world_event_type_c(world, 0), "world.created") == 0, "creation event type should be exposed")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(strcmp(clc_world_event_payload_c(world, 0), "C ABI World") == 0, "creation event payload should be exposed")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_id_c(world, 1) == 0, "out-of-range event id should be zero")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_tick_c(world, 1) == 0, "out-of-range event tick should be zero")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_type_c(world, 1)[0] == '\0', "out-of-range event type should be empty")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_payload_c(world, 1)[0] == '\0', "out-of-range event payload should be empty")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_id_c(world, UINT64_MAX) == 0, "max-index event id should be zero")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_tick_c(world, UINT64_MAX) == 0, "max-index event tick should be zero")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_type_c(world, UINT64_MAX)[0] == '\0', "max-index event type should be empty")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_payload_c(world, UINT64_MAX)[0] == '\0', "max-index event payload should be empty")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_c(world, 0) == 0, "zero tick advance should fail")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_seconds_c(world, 0) == 0, "zero second advance should fail")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_minutes_c(world, 0) == 0, "zero minute advance should fail")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_hours_c(world, 0) == 0, "zero hour advance should fail")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_days_c(world, 0) == 0, "zero day advance should fail")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == 0, "failed advance should not change tick")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_count_c(world) == 1, "failed advance should not append event")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_c(world, 5) == 1, "positive tick advance should succeed")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == 5, "world should advance to tick five")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_count_c(world) == 2, "successful advance should append event")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_id_c(world, 1) == 2, "advance event id should be two")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_tick_c(world, 1) == 5, "advance event tick should be five")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(strcmp(clc_world_event_type_c(world, 1), "world.advanced") == 0, "advance event type should be exposed")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_payload_c(world, 1)[0] == '\0', "advance event payload should be empty")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_seconds_c(world, 5) == 1, "seconds advance should succeed")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == 10, "seconds advance should add five ticks")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_minutes_c(world, 2) == 1, "minutes advance should succeed")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == 130, "minutes advance should add 120 ticks")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_hours_c(world, 1) == 1, "hours advance should succeed")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == 3730, "hours advance should add 3600 ticks")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_days_c(world, 1) == 1, "days advance should succeed")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == 90130, "days advance should add 86400 ticks")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_count_c(world) == 6, "time helpers should append successful advance events")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_c(world, UINT64_MAX) == 1, "huge tick advance should succeed with saturation")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == UINT64_MAX, "huge tick advance should saturate current tick")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_advance_c(world, 1) == 1, "post-saturation positive advance should remain accepted")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == UINT64_MAX, "post-saturation advance should not wrap current tick")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_count_c(world) == 8, "saturating advances should append events without wrapping")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_tick_c(world, 6) == UINT64_MAX, "saturating advance event tick should be max")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_event_tick_c(world, 7) == UINT64_MAX, "post-saturation event tick should remain max")) {
        clc_world_destroy_c(world);
        return 1;
    }
    clc_world_destroy_c(world);

    clc_world* default_world = clc_world_create_c(NULL, 7);
    if (require_int(default_world != NULL, "default world handle should create")) {
        return 1;
    }
    if (require_int(strcmp(clc_world_name_c(default_world), "City Life World") == 0, "null name should use default world name")) {
        clc_world_destroy_c(default_world);
        return 1;
    }
    if (require_int(strcmp(clc_world_event_payload_c(default_world, 0), "City Life World") == 0, "default world creation payload should use default name")) {
        clc_world_destroy_c(default_world);
        return 1;
    }
    if (require_int(clc_world_seed_c(default_world) == 7, "default world seed should be preserved")) {
        clc_world_destroy_c(default_world);
        return 1;
    }
    clc_world_destroy_c(default_world);

    clc_world* empty_name_world = clc_world_create_c("", 9);
    if (require_int(empty_name_world != NULL, "empty-name world handle should create")) {
        return 1;
    }
    if (require_int(strcmp(clc_world_name_c(empty_name_world), "") == 0, "empty world name should be preserved")) {
        clc_world_destroy_c(empty_name_world);
        return 1;
    }
    if (require_int(clc_world_event_payload_c(empty_name_world, 0)[0] == '\0', "empty-name creation payload should be empty")) {
        clc_world_destroy_c(empty_name_world);
        return 1;
    }
    if (require_int(clc_world_seed_c(empty_name_world) == 9, "empty-name world seed should be preserved")) {
        clc_world_destroy_c(empty_name_world);
        return 1;
    }
    clc_world_destroy_c(empty_name_world);

    return 0;
}
