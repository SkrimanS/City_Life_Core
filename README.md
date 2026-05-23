# City Life Core / Ядро

**City Life Core** is a headless C++20 simulation SDK for living settlements, resources, economy, routes, caravans, factions, ownership, contracts, persistence, deterministic replay, and server-authoritative game backends.

**City Life Core / Ядро** — headless SDK симуляции на C++20 для живых поселений, ресурсов, экономики, маршрутов, караванов, фракций, владения, контрактов, сохранений, детерминированного replay и server-authoritative игровых серверов.

Version: **0.9.9**

---

## What it is / Что это такое

City Life Core is not a game, renderer, UI framework, or network framework. It is a simulation core that a game, server, editor, backend service, MMO runtime layer, or balancing tool can embed.

City Life Core — это не игра, не рендер, не UI framework и не network framework. Это simulation core, который можно встроить в игру, сервер, редактор, backend-сервис, MMO runtime слой или инструмент балансировки.

The SDK provides:

- deterministic simulation state;
- data registry and validation for game definitions;
- settlement/resource/storage simulation;
- day-based and tick-based time model;
- routes and caravans;
- factions, reputation and ownership;
- delivery contracts and reward flows;
- basic economy: market, wallet, trade, ledger;
- runtime workflows for server-authoritative logic;
- persistence, save/load validation and deterministic replay diagnostics;
- CMake install/export package for external projects;
- minimal C interface for version/time utilities and an opaque world handle.

---

## Recommended includes / Рекомендуемые includes

For most C++ integrations:

```cpp
#include "clc/CityLifeCore.hpp"
```

For the minimal C interface:

```c
#include "clc/c/CityLifeCoreC.h"
```

The C interface currently exposes version utilities, time utilities, and a minimal opaque `clc_world` handle for create/destroy, basic state access and simple tick advancement. Full runtime integration is C++ API.

---

## Time model / Модель времени

The core is not limited to turn/day simulation.

Ядро не привязано только к пошаговым дням.

```cpp
clc::ticks_per_second(); // 1
clc::ticks_per_minute(); // 60
clc::ticks_per_hour();   // 3600
clc::ticks_per_day();    // 86400
```

Helpers:

```cpp
auto five_minutes = clc::minutes_to_ticks(5);
auto two_hours = clc::hours_to_ticks(2);
auto one_day = clc::days_to_ticks(1);
```

Tick-aware runtime fields include:

- `SettlementRoute::travel_ticks`
- `CaravanState::total_travel_ticks`
- `CaravanState::ticks_remaining`
- `ResourceDeliveryContract::due_ticks`
- `SimulationRuntime::time`

Day-based helpers remain available for daily/turn-based games. For real-time, MMO and server runtime flows, prefer tick-based APIs.

---

## CMake integration / Подключение через CMake

Install the SDK:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Use from an external C++ project:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Use from an external C project that consumes the minimal C interface:

```cmake
project(MyCityLifeCoreCConsumer LANGUAGES C CXX)
find_package(CityLifeCore CONFIG REQUIRED)

add_executable(my_consumer main.c)
set_target_properties(my_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(my_consumer PRIVATE CityLifeCore::core)
```

`main.c` is compiled as C. `LINKER_LANGUAGE CXX` is recommended because `CityLifeCore::core` is implemented in C++.

The package config also exposes installed directory variables such as `CityLifeCore_DATA_DIR` and `CityLifeCore_EXAMPLES_DIR`. See `docs/CMAKE_PACKAGE.md`.

---

## Build from source / Сборка из исходников

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

## SDK ZIP package / SDK ZIP архив

Build an install-layout ZIP package:

```bash
cmake -S . -B build-sdk-zip -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk-zip --config Release
cpack --config build-sdk-zip/CPackConfig.cmake -G ZIP
cmake -E sha256sum city-life-core-sdk-*.zip > SHA256SUMS.txt
```

The ZIP can be consumed with `CMAKE_PREFIX_PATH` after unpacking. See `docs/SDK_ZIP_PACKAGE.md` for layout and verification.

---

## Minimal runtime scenario / Минимальный runtime scenario

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

## Custom game data / Свои игровые данные

Create definitions in C++:

```cpp
#include "clc/CityLifeCore.hpp"

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;

    registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    });

    registry.add(clc::data::SettlementDefinition{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .starting_population = 100,
    });

    return registry;
}
```

Or load `.clcd` data packs with `clc::data::DataPackLoader`. See `examples/custom_data_registry.cpp`.

---

## Settlements and storage / Поселения и склады

```cpp
clc::sim::ResourceStorage storage;
storage.add("grain", 100);
storage.try_remove("grain", 25);

auto amount = storage.amount("grain");
```

Settlement simulation supports:

- population food consumption;
- building input consumption;
- building output production;
- daily advancement;
- partial tick advancement;
- deterministic tick remainders;
- deterministic reports.

---

## Routes and caravans / Маршруты и караваны

```cpp
auto route = clc::sim::make_settlement_route_ticks(
    "riverwatch_to_hillford_3h",
    "Riverwatch to Hillford 3h",
    "riverwatch",
    "hillford",
    clc::hours_to_ticks(3)
);

auto caravan = clc::sim::create_caravan_for_route(
    route,
    "caravan_01",
    "Caravan 01"
);

clc::sim::advance_caravan_ticks(caravan, clc::minutes_to_ticks(30));
```

Runtime workflows can create caravans by route id, load cargo, advance caravans by ticks/days, deliver arrived cargo and fulfill contracts after arrival.

---

## Economy, trade and ledger / Экономика, trade и ledger

Market reports include registered supplied resources and registered demand-only resources. Lookup helpers are available:

```cpp
auto report = clc::economy::make_market_report(registry, storage, market);
const auto* grain_price = clc::economy::market_price_by_resource(report, "grain");
auto price = clc::economy::market_price_or(report, "grain", 10);
```

Recommended trade path:

```cpp
clc::economy::buy_resource_with_ledger(wallet, storage, price, quantity, ledger);
clc::economy::sell_resource_with_ledger(wallet, storage, price, quantity, ledger);
```

The trade model is an abstract-market model. Buy spends coins and adds resource to local storage. Sell removes resource from local storage and credits coins. Ledger records buy/sell/contract reward entries. A fully closed economy can be built on top if a game needs one.

---

## Factions, ownership and contracts / Фракции, владение и контракты

Delivery contracts describe:

- issuer faction;
- receiver faction;
- resource id;
- quantity;
- reward coins;
- due day/ticks;
- status.

Recommended reward path:

```cpp
clc::sim::fulfill_contract_from_storage_with_reward_and_ledger(
    contracts,
    contract_id,
    delivered_storage,
    wallet,
    ledger
);
```

Runtime helpers can also fulfill contracts from arrived owned caravans. See `examples/faction_ownership_contracts.cpp`.

---

## Persistence / Сохранения

Runtime/world-state persistence can:

- save runtime state;
- load runtime state;
- restore runtime clock;
- preserve caravan progress;
- preserve contract deadlines;
- validate semantic equivalence after save/load;
- support deterministic replay diagnostics.

Minimal save/load validation:

```cpp
#include "clc/CityLifeCore.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"

#include <filesystem>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        return 1;
    }

    auto& runtime = bootstrap.runtime;
    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};

    auto result = clc::sim::validate_simulation_runtime_save_load_roundtrip(
        runtime,
        loaded,
        std::filesystem::temp_directory_path() / "runtime.clcs"
    );

    return result.ok() ? 0 : 1;
}
```

---

## Examples / Примеры

When `CLC_BUILD_EXAMPLES=ON`, CMake builds:

- `clc_example_basic_runtime`
- `clc_example_tick_runtime`
- `clc_example_full_runtime_flow`
- `clc_example_custom_data_registry`
- `clc_example_faction_ownership_contracts`
- `clc_example_save_load_roundtrip`
- `clc_example_replay_persistence`

```bash
cmake -S . -B build -DCLC_BUILD_EXAMPLES=ON
cmake --build build
./build/clc_example_basic_runtime
./build/clc_example_tick_runtime
./build/clc_example_full_runtime_flow
./build/clc_example_custom_data_registry
./build/clc_example_faction_ownership_contracts
./build/clc_example_save_load_roundtrip
./build/clc_example_replay_persistence
```

Standalone installed-package consumers:

```text
examples/find_package_consumer/   # C++ consumer
examples/c_abi_consumer/          # C consumer for minimal C interface
```

---

## Documentation / Документация

Start here:

- [Documentation index](docs/README.md)
- [Core Concepts](docs/CORE_CONCEPTS.md)
- [Public API](docs/PUBLIC_API.md)
- [Public API Status](docs/PUBLIC_API_STATUS.md)
- [C Interface](docs/C_ABI.md)
- [SDK Structure](docs/SDK_STRUCTURE.md)
- [CMake Package](docs/CMAKE_PACKAGE.md)
- [Packaging](docs/PACKAGING.md)
- [SDK ZIP Package](docs/SDK_ZIP_PACKAGE.md)
- [Compatibility](docs/COMPATIBILITY.md)
- [Migration](docs/MIGRATION.md)
- [Release verification](docs/VERIFYING_RELEASES.md)
