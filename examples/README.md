# City Life Core SDK Examples / Примеры SDK

These examples are intentionally small. They demonstrate the recommended public C++ API entry points without introducing a separate framework, renderer, or game client.

Эти примеры намеренно небольшие. Они показывают рекомендуемые точки входа публичного C++ API без отдельного фреймворка, рендера или игрового клиента.

---

## Build / Сборка

```bash
cmake -S . -B build -DCLC_BUILD_EXAMPLES=ON
cmake --build build
```

With tests as well:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

---

## Examples / Примеры

### `clc_example_basic_runtime`

Source: `examples/basic_runtime.cpp`

Shows:

- creating a basic runtime scenario;
- creating a caravan on a route;
- running runtime ticks in fixed tick chunks;
- reading tick-run summary values.

Показывает:

- создание базового runtime-сценария;
- создание каравана на маршруте;
- запуск runtime tick'ов фиксированными чанками;
- чтение tick-run summary по результатам симуляции.

Run:

```bash
./build/clc_example_basic_runtime
```

### `clc_example_tick_runtime`

Source: `examples/tick_runtime.cpp`

Shows the preferred real-time/MMO-style 0.9.9 flow:

- create a tick-based route;
- create and own a caravan;
- load cargo at origin;
- wait for arrival by ticks;
- fulfill a resource delivery contract;
- record reward ledger entry;
- explicitly deliver remaining cargo into destination storage.

Показывает предпочтительный real-time/MMO-style flow версии 0.9.9:

- создание tick-based маршрута;
- создание каравана и назначение владельца;
- загрузку cargo в origin;
- ожидание прибытия по ticks;
- выполнение resource delivery contract;
- запись reward ledger entry;
- явную доставку оставшегося cargo в destination storage.

Run:

```bash
./build/clc_example_tick_runtime
```

### `clc_example_save_load_roundtrip`

Source: `examples/save_load_roundtrip.cpp`

Shows:

- creating a runtime scenario;
- saving runtime state;
- loading runtime state;
- validating semantic save/load equivalence.

Показывает:

- создание runtime-сценария;
- сохранение runtime state;
- загрузку runtime state;
- проверку semantic save/load equivalence.

Run:

```bash
./build/clc_example_save_load_roundtrip
```

### `clc_example_replay_persistence`

Source: `examples/replay_persistence.cpp`

Shows:

- preparing two deterministic runtimes;
- saving a midpoint runtime state;
- loading the midpoint;
- continuing control and loaded runtime with the same actions;
- validating deterministic replay equivalence.

Показывает:

- подготовку двух детерминированных runtime;
- сохранение midpoint runtime state;
- загрузку midpoint;
- продолжение control и loaded runtime одинаковыми действиями;
- проверку deterministic replay equivalence.

Run:

```bash
./build/clc_example_replay_persistence
```

---

## Installed SDK consumer example / Пример внешнего consumer-проекта

`examples/find_package_consumer/` is a standalone consumer project. It is meant to be built outside the repository after City Life Core has been installed.

`examples/find_package_consumer/` — это отдельный consumer-проект. Его нужно собирать вне репозитория после установки City Life Core.

Install City Life Core:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF
cmake --build build-sdk
cmake --install build-sdk --prefix /tmp/city-life-core-sdk
```

Build the consumer against the installed SDK:

```bash
cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-consumer
./build-consumer/city_life_core_consumer
```

---

## Integration direction / Как использовать в интеграции

Prefer runtime-level APIs first:

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Start with:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
```

Then add routes, caravans, contracts, ticks, persistence, and validation through the runtime workflow APIs.

Для интеграции начинайте с runtime-level API, а не с ручной сборки всех подсистем. Затем добавляйте маршруты, караваны, контракты, tick'и, persistence и validation через runtime workflow APIs.
