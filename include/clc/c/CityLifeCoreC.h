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

clc_version clc_core_version_c(void);
const char* clc_core_version_string_c(void);

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

#ifdef __cplusplus
}
#endif
