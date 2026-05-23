#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct clc_version {
    int major;
    int minor;
    int patch;
} clc_version;

typedef struct clc_world clc_world;

clc_version clc_core_version_c(void);
const char* clc_core_version_string_c(void);

uint32_t clc_c_interface_version_c(void);

uint64_t clc_ticks_per_second_c(void);
uint64_t clc_ticks_per_minute_c(void);
uint64_t clc_ticks_per_hour_c(void);
uint64_t clc_ticks_per_day_c(void);

int clc_can_convert_seconds_to_ticks_c(uint64_t seconds);
int clc_can_convert_minutes_to_ticks_c(uint64_t minutes);
int clc_can_convert_hours_to_ticks_c(uint64_t hours);
int clc_can_convert_days_to_ticks_c(uint64_t days);

uint64_t clc_seconds_to_ticks_c(uint64_t seconds);
uint64_t clc_minutes_to_ticks_c(uint64_t minutes);
uint64_t clc_hours_to_ticks_c(uint64_t hours);
uint64_t clc_days_to_ticks_c(uint64_t days);

clc_world* clc_world_create_c(const char* name, uint64_t seed);
void clc_world_destroy_c(clc_world* world);

const char* clc_world_name_c(const clc_world* world);
uint64_t clc_world_seed_c(const clc_world* world);
uint64_t clc_world_current_tick_c(const clc_world* world);
uint64_t clc_world_event_count_c(const clc_world* world);
int clc_world_advance_c(clc_world* world, uint64_t ticks);

#ifdef __cplusplus
}
#endif
