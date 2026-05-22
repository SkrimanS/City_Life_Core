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

    return 0;
}
