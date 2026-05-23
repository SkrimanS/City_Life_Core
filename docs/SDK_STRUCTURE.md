# City Life Core SDK Structure / Структура SDK

Version: **1.0.0**

This document explains how the City Life Core 1.0.0 SDK is organized and how external C++/C projects should consume it.

---

## SDK purpose

City Life Core SDK provides a headless C++20 simulation core for games, servers, editors, backend services, MMO runtime layers and balancing tools.

The SDK contains:

- public C++ headers;
- minimal C interface header;
- C++ implementation;
- CMake target `CityLifeCore::core`;
- examples;
- demo data;
- developer documentation;
- optional tests and benchmarks for source builds.

---

## Recommended includes

For most C++ users:

```cpp
#include "clc/CityLifeCore.hpp"
```

For the minimal C interface:

```c
#include "clc/c/CityLifeCoreC.h"
```

The C interface covers version/time utilities, a minimal opaque `clc_world` handle, basic world state access, simple tick advancement and read-only world event inspection by index. Full runtime integration remains C++ API.

---

## Repository layout

```text
apps/                         # optional command-line tools
benchmarks/                   # optional benchmark runner
cmake/                        # CMake package config template
examples/                     # SDK examples and standalone consumers
include/clc/                  # installed public headers
src/clc/                      # implementation files
tests/                        # executable tests registered through CMake
data/demo_fantasy/            # demo data pack material
docs/                         # developer and release documentation
CMakeLists.txt
README.md
CHANGELOG.md
```

Important example directories:

```text
examples/find_package_consumer/   # standalone installed C++ consumer
examples/c_abi_consumer/          # standalone installed C consumer
```

---

## Installed SDK layout

A CMake install produces a layout based on GNUInstallDirs:

```text
<prefix>/
  include/clc/
    CityLifeCore.hpp
    c/CityLifeCoreC.h
    core/
    data/
    economy/
    sim/
  lib/
    libcity_life_core.*
    cmake/CityLifeCore/
      CityLifeCoreConfig.cmake
      CityLifeCoreConfigVersion.cmake
      CityLifeCoreTargets.cmake
  share/doc/CityLifeCore/
    README.md
    CHANGELOG.md
    docs/*.md
    examples/
  share/CityLifeCore/data/
    demo_fantasy/
```

`lib/` may be `lib64/` or another configured library directory depending on platform and toolchain.

---

## CMake integration modes

Installed C++ package:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

Installed C consumer:

```cmake
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)
find_package(CityLifeCore CONFIG REQUIRED)
add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

Source subdirectory:

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

When used as a subdirectory, tests/examples/tools are disabled by default unless explicitly requested.

---

## CMake options

| Option | Default | Meaning |
| --- | --- | --- |
| `CLC_BUILD_TESTS` | `ON` only for top-level project | Build tests. |
| `CLC_BUILD_EXAMPLES` | `ON` only for top-level project | Build examples. |
| `CLC_BUILD_TOOLS` | `ON` only for top-level project | Build CLI tools. |
| `CLC_BUILD_BENCHMARKS` | `OFF` | Build benchmarks. |

---

## SDK modules

### C++ umbrella

```text
include/clc/CityLifeCore.hpp
```

Recommended for most C++ game/server integrations.

### Minimal C interface

```text
include/clc/c/CityLifeCoreC.h
```

Main concepts:

- core version;
- C interface version `3`;
- tick constants and time conversions;
- opaque `clc_world` handle;
- world create/destroy;
- world name/seed/current tick/event count accessors;
- simple world tick advancement;
- read-only world event access by index.

Full runtime, data registries, containers, save/load, mutable event payload access and callbacks are not exposed through C.

### Core

```text
include/clc/core/
```

Version information, IDs/result helpers, `GameTime`, tick constants, event log and world lifecycle basics.

### Data

```text
include/clc/data/
```

Definitions, registry, validation reports and optional data-pack loading.

### Simulation

```text
include/clc/sim/
```

Storage, settlements, runtime, routes, caravans, factions, ownership, contracts, persistence and diagnostics.

### Economy

```text
include/clc/economy/
```

Market prices, market report lookup helpers, wallet, trade, ledger and experimental orders.

---

## Public header stability

All headers under `include/clc/` are installed. Stability and recommended usage are documented in:

```text
docs/PUBLIC_API_STATUS.md
```

General rule:

- prefer `clc/CityLifeCore.hpp` or stable-candidate C++ headers;
- use `clc/c/CityLifeCoreC.h` only for the minimal C interface;
- use experimental headers deliberately;
- treat raw persistence bridge and demo/helper layers as specialized APIs, not the default SDK entry point.

---

## Release package

A ZIP SDK package can be built through CPack. See:

```text
docs/SDK_ZIP_PACKAGE.md
```

Expected 1.0.0 artifact pattern:

```text
city-life-core-sdk-1.0.0-<system>-<processor>.zip
```
