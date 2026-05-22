# City Life Core SDK Structure / Структура SDK

Version: **0.9.9**

This document explains how the City Life Core SDK is organized and how external C++ projects should consume it.

Этот документ объясняет, как устроен SDK City Life Core и как внешним C++ проектам его подключать.

---

## Русский

### Назначение SDK

City Life Core SDK предоставляет headless C++20 simulation core, который можно встроить в игру, сервер, редактор мира, backend-сервис или MMO runtime слой.

SDK содержит:

- public headers;
- C++ implementation;
- CMake target `CityLifeCore::core`;
- examples;
- demo data;
- developer documentation;
- tests and benchmarks for integrators who build from source.

### Рекомендуемый include

Для большинства пользователей:

```cpp
#include "clc/CityLifeCore.hpp"
```

Этот umbrella header подключает основной SDK surface: core, data registry, economy, storage, settlements, runtime workflows, routes, caravans, factions, ownership, contracts, diagnostics и tick runtime.

Для крупных проектов можно подключать headers точечно:

```cpp
#include "clc/core/Time.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/economy/Trade.hpp"
```

### Repository layout

```text
apps/
  clc_runner/                  # minimal CLI/bootstrap runner
benchmarks/                    # optional coarse benchmark runner
cmake/                         # CMake package config template
examples/                      # SDK examples for external users
  basic_runtime.cpp
  tick_runtime.cpp
  save_load_roundtrip.cpp
  replay_persistence.cpp
  find_package_consumer/
include/
  clc/
    CityLifeCore.hpp           # recommended SDK umbrella header
    core/                      # version, ids, result, time, event log, world
    data/                      # definitions, registry, validation, data packs
    economy/                   # market, trade, ledger, orders
    sim/                       # storage, settlements, runtime, routes, caravans, contracts, persistence
src/
  clc/                         # implementation files
tests/                         # executable tests registered through CMake
data/
  demo_fantasy/                # demo data pack material
docs/                          # developer documentation
CMakeLists.txt
README.md
CHANGELOG.md
```

### Installed SDK layout

A CMake install produces a layout based on GNUInstallDirs:

```text
<prefix>/
  include/
    clc/
      CityLifeCore.hpp
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
    *.md
    examples/
  share/CityLifeCore/data/
    demo_fantasy/
```

On platforms/generators with different `CMAKE_INSTALL_LIBDIR`, `lib/` may be `lib64/` or another configured library directory.

### CMake integration modes

#### 1. Installed package

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

Configure with the install prefix:

```bash
cmake -S my_game -B build -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
```

#### 2. Subdirectory

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

When used as a subdirectory, tests/examples/tools are not enabled by default unless explicitly requested.

#### 3. Source vendoring

A project may vendor the source tree and build `city_life_core` as part of its normal build. The recommended target remains:

```cmake
CityLifeCore::core
```

### CMake options

| Option | Default | Meaning |
| --- | --- | --- |
| `CLC_BUILD_TESTS` | `ON` only for top-level project | Build tests |
| `CLC_BUILD_EXAMPLES` | `ON` only for top-level project | Build examples |
| `CLC_BUILD_TOOLS` | `ON` only for top-level project | Build CLI tools |
| `CLC_BUILD_BENCHMARKS` | `OFF` | Build coarse benchmark executables |

### SDK modules

#### Core

Headers:

```text
include/clc/core/
```

Main concepts:

- version information;
- IDs/result helpers;
- `GameTime`;
- ticks per second/minute/hour/day;
- event log;
- world lifecycle basics.

#### Data

Headers:

```text
include/clc/data/
```

Main concepts:

- `ResourceDefinition`;
- `BuildingDefinition`;
- `ProfessionDefinition`;
- `SettlementDefinition`;
- `DataRegistry`;
- validation reports;
- optional data-pack loading.

#### Simulation

Headers:

```text
include/clc/sim/
```

Main concepts:

- `ResourceStorage`;
- `SettlementState`;
- `SimulationEngine`;
- `SimulationRuntime`;
- routes and caravans;
- factions and ownership;
- delivery contracts;
- runtime workflows;
- tick/day runtime execution;
- persistence and diagnostics.

#### Economy

Headers:

```text
include/clc/economy/
```

Main concepts:

- market prices;
- wallet;
- buy/sell trade;
- ledger entries;
- trade+ledger wrapper helpers;
- experimental orders layer.

### Runtime workflow layer

For external users, the runtime workflow layer is usually the best entry point:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

Workflow helpers preserve more invariants than direct mutation of runtime fields.

### Public header stability

All headers under `include/clc/` are installed. Stability and recommended usage are documented in:

```text
docs/PUBLIC_API_STATUS.md
```

General rule:

- prefer `clc/CityLifeCore.hpp` or stable-candidate headers;
- use experimental headers deliberately;
- treat raw persistence bridge and demo/helper layers as specialized APIs, not the default SDK entry point.

### Examples

When `CLC_BUILD_EXAMPLES=ON`, CMake builds:

- `clc_example_basic_runtime`;
- `clc_example_tick_runtime`;
- `clc_example_save_load_roundtrip`;
- `clc_example_replay_persistence`.

External package consumer example:

```text
examples/find_package_consumer/
```

### Benchmarks

Optional benchmark target:

```bash
cmake -S . -B build-bench -DCLC_BUILD_BENCHMARKS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF
cmake --build build-bench --target clc_core_benchmarks
./build-bench/clc_core_benchmarks
```

Benchmarks print baseline metrics and are not a substitute for game-specific profiling.

### C ABI

This SDK currently exposes a C++ API. A C ABI is not part of the current public interface. Integrations should use the C++ SDK target unless a future release documents a separate C ABI.

---

## English

### SDK purpose

City Life Core SDK provides a headless C++20 simulation core that can be embedded into a game, server, world editor, backend service, or MMO runtime layer.

The SDK contains:

- public headers;
- C++ implementation;
- CMake target `CityLifeCore::core`;
- examples;
- demo data;
- developer documentation;
- tests and benchmarks for source integrators.

### Recommended include

For most users:

```cpp
#include "clc/CityLifeCore.hpp"
```

This umbrella header includes the main SDK surface: core, data registry, economy, storage, settlements, runtime workflows, routes, caravans, factions, ownership, contracts, diagnostics and tick runtime.

Large projects may include headers more selectively:

```cpp
#include "clc/core/Time.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/economy/Trade.hpp"
```

### Repository layout

```text
apps/
  clc_runner/                  # minimal CLI/bootstrap runner
benchmarks/                    # optional coarse benchmark runner
cmake/                         # CMake package config template
examples/                      # SDK examples for external users
  basic_runtime.cpp
  tick_runtime.cpp
  save_load_roundtrip.cpp
  replay_persistence.cpp
  find_package_consumer/
include/
  clc/
    CityLifeCore.hpp           # recommended SDK umbrella header
    core/                      # version, ids, result, time, event log, world
    data/                      # definitions, registry, validation, data packs
    economy/                   # market, trade, ledger, orders
    sim/                       # storage, settlements, runtime, routes, caravans, contracts, persistence
src/
  clc/                         # implementation files
tests/                         # executable tests registered through CMake
data/
  demo_fantasy/                # demo data pack material
docs/                          # developer documentation
CMakeLists.txt
README.md
CHANGELOG.md
```

### Installed SDK layout

A CMake install produces a layout based on GNUInstallDirs:

```text
<prefix>/
  include/
    clc/
      CityLifeCore.hpp
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
    *.md
    examples/
  share/CityLifeCore/data/
    demo_fantasy/
```

On platforms/generators with different `CMAKE_INSTALL_LIBDIR`, `lib/` may be `lib64/` or another configured library directory.

### CMake integration modes

#### 1. Installed package

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

Configure with the install prefix:

```bash
cmake -S my_game -B build -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
```

#### 2. Subdirectory

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

When used as a subdirectory, tests/examples/tools are not enabled by default unless explicitly requested.

#### 3. Source vendoring

A project may vendor the source tree and build `city_life_core` as part of its normal build. The recommended target remains:

```cmake
CityLifeCore::core
```

### CMake options

| Option | Default | Meaning |
| --- | --- | --- |
| `CLC_BUILD_TESTS` | `ON` only for top-level project | Build tests |
| `CLC_BUILD_EXAMPLES` | `ON` only for top-level project | Build examples |
| `CLC_BUILD_TOOLS` | `ON` only for top-level project | Build CLI tools |
| `CLC_BUILD_BENCHMARKS` | `OFF` | Build coarse benchmark executables |

### SDK modules

#### Core

Headers:

```text
include/clc/core/
```

Main concepts:

- version information;
- IDs/result helpers;
- `GameTime`;
- ticks per second/minute/hour/day;
- event log;
- world lifecycle basics.

#### Data

Headers:

```text
include/clc/data/
```

Main concepts:

- `ResourceDefinition`;
- `BuildingDefinition`;
- `ProfessionDefinition`;
- `SettlementDefinition`;
- `DataRegistry`;
- validation reports;
- optional data-pack loading.

#### Simulation

Headers:

```text
include/clc/sim/
```

Main concepts:

- `ResourceStorage`;
- `SettlementState`;
- `SimulationEngine`;
- `SimulationRuntime`;
- routes and caravans;
- factions and ownership;
- delivery contracts;
- runtime workflows;
- tick/day runtime execution;
- persistence and diagnostics.

#### Economy

Headers:

```text
include/clc/economy/
```

Main concepts:

- market prices;
- wallet;
- buy/sell trade;
- ledger entries;
- trade+ledger wrapper helpers;
- experimental orders layer.

### Runtime workflow layer

For external users, the runtime workflow layer is usually the best entry point:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

Workflow helpers preserve more invariants than direct mutation of runtime fields.

### Public header stability

All headers under `include/clc/` are installed. Stability and recommended usage are documented in:

```text
docs/PUBLIC_API_STATUS.md
```

General rule:

- prefer `clc/CityLifeCore.hpp` or stable-candidate headers;
- use experimental headers deliberately;
- treat raw persistence bridge and demo/helper layers as specialized APIs, not the default SDK entry point.

### Examples

When `CLC_BUILD_EXAMPLES=ON`, CMake builds:

- `clc_example_basic_runtime`;
- `clc_example_tick_runtime`;
- `clc_example_save_load_roundtrip`;
- `clc_example_replay_persistence`.

External package consumer example:

```text
examples/find_package_consumer/
```

### Benchmarks

Optional benchmark target:

```bash
cmake -S . -B build-bench -DCLC_BUILD_BENCHMARKS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF
cmake --build build-bench --target clc_core_benchmarks
./build-bench/clc_core_benchmarks
```

Benchmarks print baseline metrics and are not a substitute for game-specific profiling.

### C ABI

This SDK currently exposes a C++ API. A C ABI is not part of the current public interface. Integrations should use the C++ SDK target unless a future release documents a separate C ABI.
