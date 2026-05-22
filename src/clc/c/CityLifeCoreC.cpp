#include "clc/c/CityLifeCoreC.h"

#include "clc/core/Time.hpp"
#include "clc/core/Version.hpp"

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
    return 1u;
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
