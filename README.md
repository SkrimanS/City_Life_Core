# City Life Core / Ядро

**City Life Core** is a headless C++20 simulation core for living settlements, resources, economy, routes, caravans, factions, ownership, contracts, persistence, deterministic replay, and server-authoritative game backends.

**City Life Core / Ядро** — headless-ядро симуляции на C++20 для живых поселений, ресурсов, экономики, маршрутов, караванов, фракций, владения, контрактов, сохранений, детерминированного replay и server-authoritative игровых серверов.

Version: **0.9.9**

---

## Русский

### Что это такое

City Life Core — это не игра и не UI framework. Это библиотека/SDK, которую можно встроить в игру, сервер, редактор мира, backend-сервис, MMO runtime слой или инструмент балансировки экономики.

Ядро предоставляет:

- deterministic simulation state;
- data registry и validation для игровых definitions;
- settlement/resource/storage simulation;
- day-based и tick-based time model;
- routes и caravans;
- factions, reputation и ownership;
- delivery contracts и reward flows;
- basic economy: market, wallet, trade, ledger;
- runtime workflows для server-authoritative логики;
- persistence, save/load validation и deterministic replay diagnostics;
- CMake install/export package для внешних C++ проектов.

### Когда использовать

City Life Core подходит, если игре или backend нужно:

- моделировать поселения, склады, производство и потребление ресурсов;
- запускать караваны, торговцев или доставки на секунды, минуты, часы или дни;
- хранить состояние мира на сервере без зависимости от клиента;
- валидировать данные и runtime state;
- сохранять/загружать мир и проверять, что состояние не разошлось;
- строить экономику с market prices, wallet, trade ledger и contract rewards;
- использовать ядро как foundation для SDK, моддинга или серверной симуляции.

### Time model

Ядро не привязано только к пошаговым дням.

Базовая шкала:

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

Routes, caravans и contracts могут использовать tick-based поля:

- `SettlementRoute::travel_ticks`
- `CaravanState::total_travel_ticks`
- `CaravanState::ticks_remaining`
- `ResourceDeliveryContract::due_ticks`
- `SimulationRuntime::time`

Day-based helpers сохранены для игр, где один шаг равен дню. Для real-time, MMO и server runtime рекомендуется tick-based API.

### Подключение через CMake

Установка SDK:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Использование из внешнего проекта:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Рекомендуемый include:

```cpp
#include "clc/CityLifeCore.hpp"
```

### Build from source

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

### Минимальный runtime scenario

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

### Создание собственного registry

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

### Settlement и storage

```cpp
clc::sim::ResourceStorage storage;
storage.add("grain", 100);
storage.try_remove("grain", 25);

auto amount = storage.amount("grain");
```

Settlement simulation поддерживает:

- population food consumption;
- building input consumption;
- building output production;
- daily advancement;
- partial tick advancement;
- deterministic tick remainders;
- deterministic reports.

### Routes и caravans

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

Runtime workflows также умеют:

- создавать caravan по route id;
- загружать cargo в origin settlement;
- двигать caravan по ticks/days;
- доставлять cargo в destination settlement;
- выполнять contract fulfillment после прибытия.

### Economy, trade и ledger

Recommended trade path — использовать wrapper, который сразу пишет ledger:

```cpp
clc::economy::buy_resource_with_ledger(wallet, storage, price, quantity, ledger);
clc::economy::sell_resource_with_ledger(wallet, storage, price, quantity, ledger);
```

Trade model является abstract-market моделью:

- buy списывает coins и добавляет resource в local storage;
- sell удаляет resource из local storage и начисляет coins;
- ledger фиксирует buy/sell/contract reward entries;
- если игре нужна полностью closed economy, её можно строить поверх этого слоя.

### Contracts

Delivery contracts описывают:

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

Reward+ledger helpers используют staged mutation: contract/cargo/wallet/ledger изменяются только если вся операция успешна.

### Persistence

Runtime/world-state persistence позволяет:

- сохранить runtime state;
- загрузить runtime state;
- восстановить runtime clock;
- сохранить caravan progress;
- сохранить contract deadlines;
- проверить semantic equivalence после save/load;
- использовать replay diagnostics для поиска drift.

Минимальный save/load validation:

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

### Examples

При `CLC_BUILD_EXAMPLES=ON` собираются:

- `clc_example_basic_runtime`
- `clc_example_tick_runtime`
- `clc_example_save_load_roundtrip`
- `clc_example_replay_persistence`

```bash
cmake -S . -B build -DCLC_BUILD_EXAMPLES=ON
cmake --build build
./build/clc_example_basic_runtime
./build/clc_example_tick_runtime
./build/clc_example_save_load_roundtrip
./build/clc_example_replay_persistence
```

External `find_package` example:

```text
examples/find_package_consumer/
```

### Документация

Developer-facing docs:

- [Public API](docs/PUBLIC_API.md)
- [SDK Structure](docs/SDK_STRUCTURE.md)
- [Packaging](docs/PACKAGING.md)
- [Compatibility](docs/COMPATIBILITY.md)
- [Migration](docs/MIGRATION.md)
- [Release verification](docs/VERIFYING_RELEASES.md)

---

## English

### What is City Life Core?

City Life Core is not a game and not a UI framework. It is a C++20 library/SDK that can be embedded into a game, server, world editor, backend service, MMO runtime layer, or economy balancing tool.

The core provides:

- deterministic simulation state;
- data registry and validation for game definitions;
- settlement/resource/storage simulation;
- day-based and tick-based time model;
- routes and caravans;
- factions, reputation and ownership;
- delivery contracts and reward flows;
- basic economy: market, wallet, trade and ledger;
- runtime workflows for server-authoritative logic;
- persistence, save/load validation and deterministic replay diagnostics;
- CMake install/export package for external C++ projects.

### When to use it

City Life Core is useful when your game or backend needs to:

- simulate settlements, storage, production and resource consumption;
- run caravans, traders or deliveries over seconds, minutes, hours or days;
- keep authoritative world state on a server;
- validate data and runtime state;
- save/load the world and verify that state has not drifted;
- build an economy with market prices, wallet, trade ledger and contract rewards;
- use a simulation core as a foundation for SDK, modding or backend simulation.

### Time model

The core is not limited to day/turn-based simulation.

Base scale:

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

Routes, caravans and contracts can use tick-based fields:

- `SettlementRoute::travel_ticks`
- `CaravanState::total_travel_ticks`
- `CaravanState::ticks_remaining`
- `ResourceDeliveryContract::due_ticks`
- `SimulationRuntime::time`

Day-based helpers remain available for games where one step equals one day. For real-time, MMO and server runtime flows, prefer tick-based APIs.

### CMake integration

Install the SDK:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Use from an external project:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Recommended include:

```cpp
#include "clc/CityLifeCore.hpp"
```

### Build from source

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

### Minimal runtime scenario

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

### Custom registry

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

### Settlement and storage

```cpp
clc::sim::ResourceStorage storage;
storage.add("grain", 100);
storage.try_remove("grain", 25);

auto amount = storage.amount("grain");
```

Settlement simulation supports population food consumption, building input/output processing, daily advancement, partial tick advancement, deterministic remainders and deterministic reports.

### Routes and caravans

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

### Economy, trade and ledger

Recommended trade path:

```cpp
clc::economy::buy_resource_with_ledger(wallet, storage, price, quantity, ledger);
clc::economy::sell_resource_with_ledger(wallet, storage, price, quantity, ledger);
```

The trade model is an abstract-market model. Buy spends coins and adds resource to local storage. Sell removes resource from local storage and credits coins. Ledger records buy/sell/contract reward entries. A fully closed economy can be built on top if a game needs one.

### Contracts

Delivery contracts describe issuer faction, receiver faction, resource id, quantity, reward coins, due day/ticks and status.

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

Reward+ledger helpers use staged mutation: contract/cargo/wallet/ledger are changed only when the whole operation succeeds.

### Persistence

Runtime/world-state persistence can save runtime state, load runtime state, restore runtime clock, preserve caravan progress, preserve contract deadlines, validate semantic equivalence after save/load and support replay diagnostics.

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

### Examples

When `CLC_BUILD_EXAMPLES=ON`, these examples are built:

- `clc_example_basic_runtime`
- `clc_example_tick_runtime`
- `clc_example_save_load_roundtrip`
- `clc_example_replay_persistence`

```bash
cmake -S . -B build -DCLC_BUILD_EXAMPLES=ON
cmake --build build
./build/clc_example_basic_runtime
./build/clc_example_tick_runtime
./build/clc_example_save_load_roundtrip
./build/clc_example_replay_persistence
```

External `find_package` example:

```text
examples/find_package_consumer/
```

### Documentation

Developer-facing docs:

- [Public API](docs/PUBLIC_API.md)
- [SDK Structure](docs/SDK_STRUCTURE.md)
- [Packaging](docs/PACKAGING.md)
- [Compatibility](docs/COMPATIBILITY.md)
- [Migration](docs/MIGRATION.md)
- [Release verification](docs/VERIFYING_RELEASES.md)
