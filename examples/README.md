# City Life Core SDK Examples / Примеры SDK

These examples demonstrate the recommended public C++ API entry points without introducing a separate framework, renderer, or game client.

Эти примеры показывают рекомендуемые точки входа публичного C++ API без отдельного фреймворка, рендера или игрового клиента.

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

Run:

```bash
./build/clc_example_basic_runtime
```

### `clc_example_tick_runtime`

Source: `examples/tick_runtime.cpp`

Shows the preferred real-time/MMO-style flow:

- create a tick-based route;
- create and own a caravan;
- load cargo at origin;
- wait for arrival by ticks;
- fulfill a resource delivery contract;
- record reward ledger entry;
- explicitly deliver remaining cargo into destination storage.

Run:

```bash
./build/clc_example_tick_runtime
```

### `clc_example_full_runtime_flow`

Source: `examples/full_runtime_flow.cpp`

Shows a complete explicit integration without `make_basic_runtime_scenario()`:

- create a custom `DataRegistry`;
- create origin and destination settlements;
- add factions and ownership;
- add a tick-based route;
- add a delivery contract with `due_ticks`;
- create and own a caravan;
- load cargo at origin;
- run runtime ticks until arrival;
- fulfill the contract from the arrived caravan;
- record reward ledger entry;
- inspect wallet, ledger and remaining cargo.

This is the best example to start from when integrating City Life Core into a real game/server because every important runtime object is created explicitly.

Run:

```bash
./build/clc_example_full_runtime_flow
```

### `clc_example_custom_data_registry`

Source: `examples/custom_data_registry.cpp`

Shows how to define game data without using built-in demo scenarios:

- create an inline `.clcd` data pack string;
- load it with `DataPackLoader::load_string(...)`;
- validate resources, professions, buildings and settlements;
- create a `SimulationRuntime` from the loaded registry;
- add a building using custom definitions;
- advance settlement simulation and inspect produced resources.

This is the best example for editor/tool integrations and games that want to provide their own resources/buildings/settlement definitions.

Run:

```bash
./build/clc_example_custom_data_registry
```

### `clc_example_save_load_roundtrip`

Source: `examples/save_load_roundtrip.cpp`

Shows:

- creating a runtime scenario;
- saving runtime state;
- loading runtime state;
- validating semantic save/load equivalence.

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

Run:

```bash
./build/clc_example_replay_persistence
```

---

## Installed SDK consumer example / Пример внешнего consumer-проекта

`examples/find_package_consumer/` is a standalone consumer project. It is meant to be built outside the repository after City Life Core has been installed.

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

Recommended include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Start with custom game data if you want to define your own resources/buildings/settlements:

```text
examples/custom_data_registry.cpp
```

Start with the explicit flow if you want to integrate your own runtime world:

```text
examples/full_runtime_flow.cpp
```

Start with the basic bootstrap if you want a minimal smoke test:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
```

Then add routes, caravans, contracts, ticks, persistence and validation through the runtime workflow APIs.
