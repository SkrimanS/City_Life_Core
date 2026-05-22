# C Interface / C ABI

Version: **0.9.9**  
C interface version: **1**

City Life Core is primarily a C++20 SDK. A small C-facing interface is provided for version and time utilities, and for simple FFI smoke checks.

City Life Core в первую очередь является C++20 SDK. Небольшой C-facing interface предоставлен для version/time utilities и простых FFI smoke checks.

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
- saturating time conversion helpers.

The current C interface does **not** expose:

- runtime state;
- registries;
- containers;
- strings with caller-owned allocation;
- save/load APIs;
- event callbacks;
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

## CMake consumer

External C project:

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyCityLifeCoreCConsumer LANGUAGES C)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

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
1
```

Future C interface expansion should preserve existing functions where possible and increase the C interface version when the C-facing surface changes meaningfully.
