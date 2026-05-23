# C ABI Consumer Example

This is a standalone C project that consumes an installed City Life Core SDK through CMake.

The current C ABI is intentionally minimal. It exposes version utilities, time utility functions, and a small opaque `clc_world` handle for create/destroy, basic state access, and simple tick advancement. Full runtime, containers, save/load, event payload and callback APIs remain C++ APIs.

---

## Build against installed SDK

Install City Life Core first:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF
cmake --build build-sdk
cmake --install build-sdk --prefix /tmp/city-life-core-sdk
```

Build this C consumer:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-c-abi-consumer
./build-c-abi-consumer/city_life_core_c_abi_consumer
```

On Windows/MSVC the executable may be under the configuration directory:

```powershell
.\build-c-abi-consumer\Release\city_life_core_c_abi_consumer.exe
```

---

## Header

```c
#include "clc/c/CityLifeCoreC.h"
```

---

## Covered functions

- `clc_core_version_c()`
- `clc_core_version_string_c()`
- `clc_c_interface_version_c()`
- `clc_ticks_per_second_c()`
- `clc_ticks_per_minute_c()`
- `clc_ticks_per_hour_c()`
- `clc_ticks_per_day_c()`
- `clc_can_convert_*_to_ticks_c(...)`
- `clc_*_to_ticks_c(...)`
- `clc_world_create_c(...)`
- `clc_world_destroy_c(...)`
- `clc_world_name_c(...)`
- `clc_world_seed_c(...)`
- `clc_world_current_tick_c(...)`
- `clc_world_event_count_c(...)`
- `clc_world_advance_c(...)`
