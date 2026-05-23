# C Interface / C ABI

Version: **0.9.9**  
C interface version: **2**

City Life Core is primarily a C++20 SDK. A small C-facing interface is provided for version and time utilities, a minimal opaque world handle, and simple FFI smoke checks.

City Life Core в первую очередь является C++20 SDK. Небольшой C-facing interface предоставлен для version/time utilities, минимального opaque world handle и простых FFI smoke checks.

---

## Header

```c
#include "clc/c/CityLifeCoreC.h"
```

The header is installed with the SDK under:

```text
include/clc/c/CityLifeCoreC.h
```

---

## Scope

The current C interface exposes:

- core version;
- C interface version;
- tick constants;
- safe time conversion preflight helpers;
- saturating time conversion helpers;
- a minimal opaque `clc_world` handle;
- world create/destroy;
- world name/seed/current tick/event count accessors;
- simple world tick advancement.

The current C interface does **not** expose:

- data registries;
- containers;
- strings with caller-owned allocation;
- save/load APIs;
- event callbacks;
- event payload access;
- caravans/contracts/economy workflows.

Use the C++ API for full SDK integration.

---

## Version functions

```c
typedef struct clc_version {
    int major;
    int minor;
    int patch;
} clc_version;

clc_version clc_core_version_c(void);
const char* clc_core_version_string_c(void);
uint32_t clc_c_interface_version_c(void);
```

Example:

```c
const clc_version version = clc_core_version_c();
printf("%d.%d.%d\n", version.major, version.minor, version.patch);
printf("%s\n", clc_core_version_string_c());
printf("C interface version: %u\n", (unsigned)clc_c_interface_version_c());
```

`clc_core_version_string_c()` returns a static null-terminated string. The caller must not free it.

---

## Time constants

```c
uint64_t clc_ticks_per_second_c(void);
uint64_t clc_ticks_per_minute_c(void);
uint64_t clc_ticks_per_hour_c(void);
uint64_t clc_ticks_per_day_c(void);
```

Expected values:

```text
clc_ticks_per_second_c() == 1
clc_ticks_per_minute_c() == 60
clc_ticks_per_hour_c()   == 3600
clc_ticks_per_day_c()    == 86400
```

---

## Conversion preflight helpers

```c
int clc_can_convert_seconds_to_ticks_c(uint64_t seconds);
int clc_can_convert_minutes_to_ticks_c(uint64_t minutes);
int clc_can_convert_hours_to_ticks_c(uint64_t hours);
int clc_can_convert_days_to_ticks_c(uint64_t days);
```

These functions return:

- `1` when the conversion fits into `uint64_t`;
- `0` when the conversion would overflow.

---

## Conversion helpers

```c
uint64_t clc_seconds_to_ticks_c(uint64_t seconds);
uint64_t clc_minutes_to_ticks_c(uint64_t minutes);
uint64_t clc_hours_to_ticks_c(uint64_t hours);
uint64_t clc_days_to_ticks_c(uint64_t days);
```

These functions saturate to `UINT64_MAX` on overflow.

Example:

```c
uint64_t five_minutes = clc_minutes_to_ticks_c(5); // 300
uint64_t two_hours = clc_hours_to_ticks_c(2);      // 7200
```

---

## Minimal world handle

```c
typedef struct clc_world clc_world;

clc_world* clc_world_create_c(const char* name, uint64_t seed);
void clc_world_destroy_c(clc_world* world);

const char* clc_world_name_c(const clc_world* world);
uint64_t clc_world_seed_c(const clc_world* world);
uint64_t clc_world_current_tick_c(const clc_world* world);
uint64_t clc_world_event_count_c(const clc_world* world);
int clc_world_advance_c(clc_world* world, uint64_t ticks);
```

`clc_world` is an opaque handle. Callers create it with `clc_world_create_c()` and must release it with `clc_world_destroy_c()`.

Ownership and lifetime rules:

- `clc_world_create_c()` returns `NULL` if allocation or construction fails.
- Passing `NULL` as `name` creates a world with the default name `City Life World`.
- `clc_world_destroy_c(NULL)` is valid and does nothing.
- `clc_world_name_c()` returns a pointer owned by the world handle. Do not free it and do not keep it after destroying the world.
- null world accessors return empty string or zero values.
- `clc_world_advance_c()` returns `1` on success and `0` on failure.
- advancing by zero ticks fails and does not mutate the world.

Example:

```c
clc_world* world = clc_world_create_c("C ABI World", 42);
if (world == NULL) {
    return 1;
}

if (clc_world_advance_c(world, 5) != 1) {
    clc_world_destroy_c(world);
    return 1;
}

printf("%s tick=%llu events=%llu\n",
    clc_world_name_c(world),
    (unsigned long long)clc_world_current_tick_c(world),
    (unsigned long long)clc_world_event_count_c(world));

clc_world_destroy_c(world);
```

---

## CMake consumer

External C consumer project:

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

`main.c` is still compiled as C. `LINKER_LANGUAGE CXX` is recommended because `CityLifeCore::core` is implemented as a C++ library and may need the C++ linker/runtime.

Configure with an installed SDK:

```bash
cmake -S my_consumer -B build -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build
```

---

## Standalone example

A complete installed-SDK C consumer is provided at:

```text
examples/c_abi_consumer/
```

Build it after installing the SDK:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build-c-abi-consumer
./build-c-abi-consumer/city_life_core_c_abi_consumer
```

On Windows/MSVC the executable can be under the configuration directory:

```powershell
.\build-c-abi-consumer\Release\city_life_core_c_abi_consumer.exe
```

---

## Testing and package validation

The repository contains:

```text
tests/c_abi_smoke_tests.c
examples/c_abi_consumer/
```

CI validates:

- C header compilation;
- linking against `CityLifeCore::core`;
- installed SDK C consumer;
- unpacked ZIP SDK C consumer.

---

## Compatibility note

`clc_c_interface_version_c()` returns the version of the C-facing interface. It is separate from the core SDK version.

For `0.9.9`, the C interface version is:

```text
2
```

Future C interface expansion should preserve existing functions where possible and increase the C interface version when the C-facing surface changes meaningfully.