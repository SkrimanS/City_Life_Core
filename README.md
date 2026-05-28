# City Life Core

**City Life Core** is a headless C++20 simulation SDK for living settlements, resources, economy, routes, caravans, factions, ownership, contracts, persistence, deterministic replay, and server-authoritative game backends.

Version: **1.0.0**  
Status: **released / v1.0.0**  
License: **Apache-2.0**. See [`LICENSE`](LICENSE).

Russian documentation is available in [`docs/ru/README.md`](docs/ru/README.md).

---

## What it is

City Life Core is not a game, renderer, UI framework, or network framework. It is a simulation core that a game, server, editor, backend service, MMO runtime layer, or balancing tool can embed.

The SDK provides:

- deterministic simulation state;
- data registry and validation for game definitions;
- settlement, resource and storage simulation;
- day-based and tick-based time models;
- routes and caravans;
- factions, reputation and ownership;
- delivery contracts and reward flows;
- market, wallet, trade and ledger primitives;
- runtime workflows for server-authoritative logic;
- persistence, save/load validation and deterministic replay diagnostics;
- CMake install/export package for external projects;
- a minimal C interface for version/time utilities, an opaque world handle and read-only world event access;
- initial C# / Unity guidance through the C ABI and P/Invoke.

---

## Recommended includes

For most C++ integrations:

```cpp
#include "clc/CityLifeCore.hpp"
```

For the minimal C interface:

```c
#include "clc/c/CityLifeCoreC.h"
```

The C interface exposes version utilities, time utilities, a minimal opaque `clc_world` handle for create/destroy, basic state access, simple tick advancement and read-only world event inspection by index. Full runtime integration is provided by the C++ API.

For Unity and C# integrations, use the C ABI through P/Invoke. See [`docs/csharp-unity.md`](docs/csharp-unity.md). For browser and WebAssembly planning, see [`docs/browser-wasm.md`](docs/browser-wasm.md). For the staged C ABI expansion plan, see [`docs/c-abi-expansion-plan.md`](docs/c-abi-expansion-plan.md).

---

## Build from source

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Windows / multi-config generators:

```powershell
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

Optional benchmarks:

```bash
cmake -S . -B build-bench -DCLC_BUILD_BENCHMARKS=ON
cmake --build build-bench --target clc_core_benchmarks
./build-bench/clc_core_benchmarks
```

---

## CMake integration

Install the SDK:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Use it from an external C++ project:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Use it from an external C project that consumes the minimal C interface:

```cmake
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)
find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

`main.c` is compiled as C. `LINKER_LANGUAGE CXX` is recommended because `CityLifeCore::core` is implemented in C++.

See [`docs/cmake-package.md`](docs/cmake-package.md) for exported targets and installed package variables.

---

## Time model

The core is not limited to turn/day simulation.

```cpp
clc::ticks_per_second(); // 1
clc::ticks_per_minute(); // 60
clc::ticks_per_hour();   // 3600
clc::ticks_per_day();    // 86400

auto five_minutes = clc::minutes_to_ticks(5);
auto two_hours = clc::hours_to_ticks(2);
auto one_day = clc::days_to_ticks(1);
```

Tick-aware runtime fields include `SettlementRoute::travel_ticks`, `CaravanState::total_travel_ticks`, `CaravanState::ticks_remaining`, `ResourceDeliveryContract::due_ticks` and `SimulationRuntime::time`.

Day-based helpers remain available for daily or turn-based games. For real-time, MMO and server runtime flows, prefer tick-based APIs.

---

## Minimal runtime scenario

```cpp
#include "clc/CityLifeCore.hpp"

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "demo_caravan",
        "Demo Caravan"
    );
    if (!caravan.ok()) {
        return 1;
    }

    auto run = clc::sim::run_runtime_ticks(
        runtime,
        clc::hours_to_ticks(2),
        clc::minutes_to_ticks(30)
    );

    return run.ok() ? 0 : 1;
}
```

---

## Documentation

Start here:

- [Documentation index](docs/README.md)
- [Core concepts](docs/core-concepts.md)
- [Public API](docs/public-api.md)
- [Public API status](docs/public-api-status.md)
- [C interface](docs/c-abi.md)
- [C ABI expansion plan](docs/c-abi-expansion-plan.md)
- [Integration targets](docs/integration-targets.md)
- [C# and Unity integration](docs/csharp-unity.md)
- [Browser and WebAssembly integration](docs/browser-wasm.md)
- [SDK structure](docs/sdk-structure.md)
- [CMake package](docs/cmake-package.md)
- [Packaging](docs/packaging.md)
- [SDK ZIP package](docs/sdk-zip-package.md)
- [Compatibility](docs/compatibility.md)
- [Migration](docs/migration.md)
- [Roadmap](docs/roadmap.md)
- [Versioning and release policy](docs/versioning.md)
- [Release notes](docs/release-notes-1.0.0.md)
- [Release manifest](docs/release-manifest-1.0.0.md)
- [Release verification](docs/verifying-releases.md)
- [Russian documentation](docs/ru/README.md)
