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
- running runtime ticks;
- reading run summary values.

Показывает:

- создание базового runtime-сценария;
- создание каравана на маршруте;
- запуск runtime tick'ов;
- чтение summary по результатам симуляции.

Run:

```bash
./build/clc_example_basic_runtime
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
