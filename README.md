# City Life Core / Ядро

**City Life Core** is a headless C++20 simulation core for living settlements, resources, economy, routes, caravans, factions, ownership, contracts, persistence, deterministic replay, and server-authoritative game backends. It supports both day/turn-style simulation and real-time tick-based runtime flows for games where events may happen after seconds, minutes, or hours.

**City Life Core / Ядро** — это headless-ядро симуляции на C++20 для живых поселений, ресурсов, экономики, маршрутов, караванов, фракций, владения, контрактов, сохранений, детерминированного replay и server-authoritative игровых серверов. Ядро поддерживает как day/turn-style симуляцию, так и real-time tick-based runtime для игр, где события могут происходить через секунды, минуты или часы.

Current version: **0.9.9**  
Release role: **pre-1.0 audit build / сборка для аудита перед 1.0**

---

## Русский

### Что это такое

City Life Core — это не игра и не клиентский фреймворк. Это низкоуровневое симуляционное ядро, которое можно встроить в игру, сервер, редактор мира, backend-сервис, инструмент балансировки экономики или MMO/runtime слой.

Ядро отвечает за то, чтобы мир был:

- **детерминированным** — одинаковые действия дают одинаковое состояние;
- **проверяемым** — данные, runtime state и persistence проходят валидацию;
- **сохраняемым** — состояние мира можно сериализовать и восстановить;
- **headless** — без зависимости от графики, UI, платформы или движка;
- **time-scale independent** — логика не привязана только к дням: можно использовать дни, часы, минуты и секунды;
- **SDK-friendly** — публичные C++ API постепенно стабилизируются вокруг runtime сценариев;
- **готовым к server-authoritative архитектуре** — состояние, действия, события и replay отделены от клиента.

### Какие задачи закрывает ядро

City Life Core полезен, если нужно:

- моделировать поселения, склады, производство и потребление ресурсов;
- строить экономику на ресурсах, спросе, торговле, кошельке и ledger;
- связывать поселения маршрутами и караванами;
- добавлять фракции, репутацию, владение поселениями/караванами;
- создавать контракты на доставку ресурсов и вознаграждения;
- запускать караваны и торговцев не только на дни, но и на часы, минуты или секунды;
- автоматически отмечать прибытия караванов и просроченные контракты во время runtime tick;
- явно доставлять оставшийся cargo прибывшего каравана в destination settlement storage;
- сохранять и загружать runtime state вместе с runtime clock;
- проверять, что загруженный мир семантически равен исходному;
- продолжать симуляцию после загрузки без расхождения состояния;
- использовать ядро как основу SDK, MMO backend или backend simulation service.

### Real-time / MMO runtime

В версии `0.9.9` runtime больше не является чисто day-based слоем.

Что есть сейчас:

- `clc::GameTime` хранит runtime tick clock;
- `ticks_per_second()`, `minutes_to_ticks(...)`, `hours_to_ticks(...)`, `days_to_ticks(...)` задают единую шкалу времени;
- routes и caravans могут иметь `travel_ticks`, а не только `travel_days`;
- contracts могут иметь `due_ticks`, а не только `due_day`;
- `advance_runtime_ticks(runtime, ticks)` двигает caravan/contract runtime без `advance_day`;
- `run_runtime_ticks(runtime, total_ticks, step_ticks)` прогоняет runtime чанками;
- `run_runtime_until_first_caravan_arrival_by_ticks(...)` ждёт прибытие по ticks;
- `run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(...)` ждёт прибытие и выполняет контракт без daily engine advancement;
- event log пишет absolute runtime ticks, а не day numbers;
- runtime save/load сохраняет `runtime.time`, `due_ticks`, caravan tick progress и settlement tick remainders.

Day-based API сохранены как совместимые wrappers. Их можно использовать для пошаговых/дневных игр, но для real-time и MMO рекомендуется tick-based API.

### Текущий статус

Проект находится на стадии **0.9.9 Runtime Persistence, Real-Time Runtime & Public SDK Readiness**.

Готовые и активно укрепляемые области:

| Подсистема | Статус |
| --- | --- |
| Foundation/Core | зрелая основа |
| Data Registry | рабочий registry definitions + validation |
| Settlement/Storage | рабочая симуляция поселений, складов и partial tick remainders |
| Basic Economy/Market | базовая экономика, рынок, trade, ledger |
| Routes | day-based и tick-based маршруты между поселениями |
| Caravans | cargo, day/tick movement, arrival state, arrival reports, explicit cargo delivery |
| Factions/Ownership | фракции, reputation, ownership |
| Contracts | delivery contracts, fulfillment, reward ledger, due_day/due_ticks, overdue failures |
| Persistence | world/runtime save-load, runtime clock, due_ticks, corrupted-load coverage, replay validation |
| Runtime/World Integration | runtime bundle, scenario bootstrap, workflows, deterministic tick consequences |
| Runtime Events/Diagnostics | day/tick event logs, absolute timestamps, cargo delivery, fulfilled/failed contracts |
| Public SDK/API | в процессе стабилизации |
| C ABI | ещё не готово |
| Packaging/Release | CMake install/export package flow добавлен, binary release ещё впереди |
| External Docs | активно оформляется |

### Быстрый старт

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Запуск bootstrap CLI:

```bash
./build/clc_runner
```

Windows / multi-config generators:

```powershell
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

### SDK examples

При `CLC_BUILD_EXAMPLES=ON` собираются примеры:

- `clc_example_basic_runtime` — bootstrap runtime, запуск tick'ов и summary;
- `clc_example_save_load_roundtrip` — runtime save/load validation;
- `clc_example_replay_persistence` — midpoint save/load и deterministic replay continuation.

```bash
cmake -S . -B build -DCLC_BUILD_EXAMPLES=ON
cmake --build build
./build/clc_example_basic_runtime
./build/clc_example_save_load_roundtrip
./build/clc_example_replay_persistence
```

### CMake install / find_package

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

Внешний CMake-проект может подключать установленное ядро так:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Подробнее: [Packaging / Упаковка](docs/PACKAGING.md).

### Минимальный tick-based runtime-сценарий

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

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

### Минимальный save/load сценарий

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
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

### Документация

- [Public API / Публичный API](docs/PUBLIC_API.md)
- [SDK Structure / Структура SDK](docs/SDK_STRUCTURE.md)
- [Packaging / Упаковка](docs/PACKAGING.md)
- [Changelog](CHANGELOG.md)

### Ближайшие цели до 1.0.0

- прогнать 0.9.9 через аудит и полную проверку;
- стабилизировать Public SDK/API surface;
- довести release SDK layout до binary/package artifacts;
- расширить external documentation;
- начать C ABI только после стабилизации C++ SDK surface;
- после аудита поднять `1.0.0` и слить только проверенный блок.

---

## English

### What is City Life Core?

City Life Core is not a game and not a client framework. It is a low-level simulation core that can be embedded into a game, server, world editor, economy balancing tool, backend service, or MMO/runtime layer.

The core is designed to make world simulation:

- **deterministic** — the same actions produce the same state;
- **validated** — data, runtime state, and persistence are checked explicitly;
- **persistent** — runtime state can be saved, loaded, and compared semantically;
- **headless** — no rendering, UI, platform, or engine dependency;
- **time-scale independent** — logic is not limited to days: seconds, minutes, hours, and days can all be represented;
- **SDK-friendly** — public C++ APIs are being stabilized around runtime workflows;
- **server-authoritative ready** — state, actions, events, and replay are separated from clients.

### What problems does it solve?

City Life Core is useful when you need to:

- simulate settlements, storage, production, and resource consumption;
- build resource-driven economy with demand, trade, wallet, and ledger;
- connect settlements with routes and caravans;
- model factions, reputation, and ownership;
- create delivery contracts and reward flows;
- run caravans and traders over seconds, minutes, hours, or days;
- automatically report caravan arrivals and overdue contract failures during runtime ticks;
- explicitly deliver remaining arrived caravan cargo into destination settlement storage;
- save and load complete runtime state, including runtime clock;
- validate that loaded runtime state is semantically equivalent;
- continue simulation after load without deterministic drift;
- use the core as a foundation for a public SDK, MMO backend, or backend simulation service.

### Real-time / MMO runtime

In `0.9.9`, the runtime is no longer only a day-based orchestration layer.

Available now:

- `clc::GameTime` stores the runtime tick clock;
- `ticks_per_second()`, `minutes_to_ticks(...)`, `hours_to_ticks(...)`, and `days_to_ticks(...)` define one time scale;
- routes and caravans can use `travel_ticks`, not only `travel_days`;
- contracts can use `due_ticks`, not only `due_day`;
- `advance_runtime_ticks(runtime, ticks)` advances caravan/contract runtime without `advance_day`;
- `run_runtime_ticks(runtime, total_ticks, step_ticks)` runs runtime in tick chunks;
- `run_runtime_until_first_caravan_arrival_by_ticks(...)` waits for arrival by ticks;
- `run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(...)` waits for arrival and fulfills a contract without daily engine advancement;
- event logs write absolute runtime ticks, not day numbers;
- runtime save/load persists `runtime.time`, `due_ticks`, caravan tick progress, and settlement tick remainders.

Day-based APIs remain available as compatibility wrappers. They are still useful for turn-based or daily games, while tick-based APIs are the preferred path for real-time and MMO runtimes.

### Current status

The project is currently in **0.9.9 Runtime Persistence, Real-Time Runtime & Public SDK Readiness**.

Implemented and actively hardened areas:

| Subsystem | Status |
| --- | --- |
| Foundation/Core | mature foundation |
| Data Registry | working definitions registry + validation |
| Settlement/Storage | working settlement/storage simulation with partial tick remainders |
| Basic Economy/Market | basic economy, market, trade, ledger |
| Routes | day-based and tick-based settlement routes |
| Caravans | cargo, day/tick movement, arrival state, arrival reports, explicit cargo delivery |
| Factions/Ownership | factions, reputation, ownership |
| Contracts | delivery contracts, fulfillment, reward ledger, due_day/due_ticks, overdue failures |
| Persistence | world/runtime save-load, runtime clock, due_ticks, corrupted-load coverage, replay validation |
| Runtime/World Integration | runtime bundle, scenario bootstrap, workflows, deterministic tick consequences |
| Runtime Events/Diagnostics | day/tick event logs, absolute timestamps, cargo delivery, fulfilled/failed contracts |
| Public SDK/API | being stabilized |
| C ABI | not ready yet |
| Packaging/Release | CMake install/export package flow added, binary release still pending |
| External Docs | being actively written |

### Quick start

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the bootstrap CLI:

```bash
./build/clc_runner
```

Windows / multi-config generators:

```powershell
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

### SDK examples

When `CLC_BUILD_EXAMPLES=ON`, the SDK examples are built:

- `clc_example_basic_runtime` — runtime bootstrap, ticks, and summary;
- `clc_example_save_load_roundtrip` — runtime save/load validation;
- `clc_example_replay_persistence` — midpoint save/load and deterministic replay continuation.

```bash
cmake -S . -B build -DCLC_BUILD_EXAMPLES=ON
cmake --build build
./build/clc_example_basic_runtime
./build/clc_example_save_load_roundtrip
./build/clc_example_replay_persistence
```

### CMake install / find_package

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=/path/to/city-life-core-sdk
cmake --build build
cmake --install build
```

External CMake projects can consume the installed package with:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Details: [Packaging](docs/PACKAGING.md).

### Minimal tick-based runtime scenario

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

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

### Minimal save/load scenario

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
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

### Documentation

- [Public API](docs/PUBLIC_API.md)
- [SDK Structure](docs/SDK_STRUCTURE.md)
- [Packaging](docs/PACKAGING.md)
- [Changelog](CHANGELOG.md)

### Near-term goals before 1.0.0

- run 0.9.9 through audit and full verification;
- stabilize the Public SDK/API surface;
- finish release SDK layout into binary/package artifacts;
- expand external user documentation;
- start C ABI only after the C++ SDK surface is stable;
- raise `1.0.0` after audit and merge only the verified block.
