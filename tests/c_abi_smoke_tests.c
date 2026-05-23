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
    if (require_int(version.major == 0, "major version should be 0")) {
        return 1;
    }
    if (require_int(version.minor == 9, "minor version should be 9")) {
        return 1;
    }
    if (require_int(version.patch == 9, "patch version should be 9")) {
        return 1;
    }
    if (require_int(strcmp(clc_core_version_string_c(), "0.9.9") == 0, "version string should match")) {
        return 1;
    }
    if (require_int(clc_c_interface_version_c() == 2u, "C interface version should be 2")) {
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
    if (require_int(clc_world_advance_c(NULL, 1) == 0, "null world advance should fail")) {
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
    if (require_int(clc_world_advance_c(world, 0) == 0, "zero tick advance should fail")) {
        clc_world_destroy_c(world);
        return 1;
    }
    if (require_int(clc_world_current_tick_c(world) == 0, "failed advance should not change tick")) {
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
    clc_world_destroy_c(world);

    clc_world* default_world = clc_world_create_c(NULL, 7);
    if (require_int(default_world != NULL, "default world handle should create")) {
        return 1;
    }
    if (require_int(strcmp(clc_world_name_c(default_world), "City Life World") == 0, "null name should use default world name")) {
        clc_world_destroy_c(default_world);
        return 1;
    }
    if (require_int(clc_world_seed_c(default_world) == 7, "default world seed should be preserved")) {
        clc_world_destroy_c(default_world);
        return 1;
    }
    clc_world_destroy_c(default_world);

    return 0;
}
