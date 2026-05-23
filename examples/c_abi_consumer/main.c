#include "clc/c/CityLifeCoreC.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const clc_version version = clc_core_version_c();
    if (version.major != 1 || version.minor != 0 || version.patch != 0) {
        fprintf(stderr, "Unexpected version: %d.%d.%d\n", version.major, version.minor, version.patch);
        return 1;
    }

    if (strcmp(clc_core_version_string_c(), "1.0.0") != 0) {
        fprintf(stderr, "Unexpected version string: %s\n", clc_core_version_string_c());
        return 1;
    }

    if (clc_c_interface_version_c() != 3u) {
        fprintf(stderr, "Unexpected C interface version: %u\n", (unsigned)clc_c_interface_version_c());
        return 1;
    }

    const uint64_t five_minutes = clc_minutes_to_ticks_c(5);
    const uint64_t two_hours = clc_hours_to_ticks_c(2);

    if (five_minutes != 300 || two_hours != 7200) {
        fprintf(stderr, "Unexpected tick conversion\n");
        return 1;
    }

    clc_world* world = clc_world_create_c("C ABI Consumer World", 42);
    if (world == NULL) {
        fprintf(stderr, "Failed to create C ABI world handle\n");
        return 1;
    }

    if (clc_world_event_count_c(world) != 1) {
        fprintf(stderr, "Unexpected initial event count\n");
        clc_world_destroy_c(world);
        return 1;
    }

    if (strcmp(clc_world_event_type_c(world, 0), "world.created") != 0 ||
        strcmp(clc_world_event_payload_c(world, 0), "C ABI Consumer World") != 0) {
        fprintf(stderr, "Unexpected creation event\n");
        clc_world_destroy_c(world);
        return 1;
    }

    if (clc_world_advance_c(world, 5) != 1) {
        fprintf(stderr, "Failed to advance C ABI world handle\n");
        clc_world_destroy_c(world);
        return 1;
    }

    if (clc_world_event_count_c(world) != 2 ||
        clc_world_event_id_c(world, 1) != 2 ||
        clc_world_event_tick_c(world, 1) != 5 ||
        strcmp(clc_world_event_type_c(world, 1), "world.advanced") != 0) {
        fprintf(stderr, "Unexpected advance event\n");
        clc_world_destroy_c(world);
        return 1;
    }

    printf("version=%s\n", clc_core_version_string_c());
    printf("c_interface_version=%u\n", (unsigned)clc_c_interface_version_c());
    printf("ticks_per_day=%llu\n", (unsigned long long)clc_ticks_per_day_c());
    printf("five_minutes=%llu\n", (unsigned long long)five_minutes);
    printf("two_hours=%llu\n", (unsigned long long)two_hours);
    printf("world_name=%s\n", clc_world_name_c(world));
    printf("world_seed=%llu\n", (unsigned long long)clc_world_seed_c(world));
    printf("world_tick=%llu\n", (unsigned long long)clc_world_current_tick_c(world));
    printf("world_events=%llu\n", (unsigned long long)clc_world_event_count_c(world));
    printf("event0=%llu:%llu:%s:%s\n",
        (unsigned long long)clc_world_event_id_c(world, 0),
        (unsigned long long)clc_world_event_tick_c(world, 0),
        clc_world_event_type_c(world, 0),
        clc_world_event_payload_c(world, 0));
    printf("event1=%llu:%llu:%s:%s\n",
        (unsigned long long)clc_world_event_id_c(world, 1),
        (unsigned long long)clc_world_event_tick_c(world, 1),
        clc_world_event_type_c(world, 1),
        clc_world_event_payload_c(world, 1));

    clc_world_destroy_c(world);
    return 0;
}
