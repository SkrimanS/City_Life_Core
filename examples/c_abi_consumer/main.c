#include "clc/c/CityLifeCoreC.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    const clc_version version = clc_core_version_c();
    if (version.major != 0 || version.minor != 9 || version.patch != 9) {
        fprintf(stderr, "Unexpected version: %d.%d.%d\n", version.major, version.minor, version.patch);
        return 1;
    }

    if (strcmp(clc_core_version_string_c(), "0.9.9") != 0) {
        fprintf(stderr, "Unexpected version string: %s\n", clc_core_version_string_c());
        return 1;
    }

    if (clc_c_interface_version_c() != 1u) {
        fprintf(stderr, "Unexpected C interface version: %u\n", (unsigned)clc_c_interface_version_c());
        return 1;
    }

    const uint64_t five_minutes = clc_minutes_to_ticks_c(5);
    const uint64_t two_hours = clc_hours_to_ticks_c(2);

    if (five_minutes != 300 || two_hours != 7200) {
        fprintf(stderr, "Unexpected tick conversion\n");
        return 1;
    }

    printf("version=%s\n", clc_core_version_string_c());
    printf("c_interface_version=%u\n", (unsigned)clc_c_interface_version_c());
    printf("ticks_per_day=%llu\n", (unsigned long long)clc_ticks_per_day_c());
    printf("five_minutes=%llu\n", (unsigned long long)five_minutes);
    printf("two_hours=%llu\n", (unsigned long long)two_hours);

    return 0;
}
