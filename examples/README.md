# City Life Core SDK Examples / Примеры SDK

These examples demonstrate the recommended public C++ API entry points without introducing a separate framework, renderer, network server, or game client.

Эти примеры показывают рекомендуемые точки входа публичного C++ API без отдельного фреймворка, рендера, сетевого сервера или игрового клиента.

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

### `clc_example_action_bridge`

Source: `examples/action_bridge.cpp`

Shows the v1.2.0 local Action Bridge flow:

- accept a JSON action with `action_id`, `type`, optional `actor_id` and `payload`;
- validate the action before mutation;
- dispatch it into the runtime engine;
- return compact JSON result status, diagnostics count and produced event count;
- keep the bridge local and transport-agnostic, without HTTP, WebSocket, accounts, multiplayer or UI.

Run:

```bash
./build/clc_example_action_bridge
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

### `clc_example_faction_ownership_contracts`

Source: `examples/faction_ownership_contracts.cpp`

Shows how factions and ownership affect gameplay permissions:

- create two factions;
- assign settlement ownership;
- assign caravan ownership;
- set faction reputation;
- create a delivery contract;
- reject contract fulfillment when the caravan owner does not match the expected faction;
- reassign the caravan to the correct faction;
- fulfill the contract and write the reward ledger entry;
- query owner and owned-caravan helpers.

This example is useful for strategy games, faction-based economies and server-side permission checks.

Run:

```bash
./build/clc_example_faction_ownership_contracts
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

## C ABI consumer example / Пример C ABI consumer

`examples/c_abi_consumer/` is a standalone C consumer project for the installed SDK. It is the native integration layer that C#, Unity and other non-C++ integrations should build on.

---

## C# / Unity wrapper example / Пример C# / Unity wrapper

`examples/csharp_unity/` contains an initial C# P/Invoke wrapper for the current C ABI.

It shows:

- how to import the native `city_life_core` library from C#;
- how to read version and tick utility functions;
- how to wrap the opaque `clc_world` handle in a disposable managed class;
- how to inspect read-only world events from Unity or another C# host.

See also:

```text
docs/csharp-unity.md
```

---

## Integration direction / Как использовать в интеграции

Recommended include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Start with the Action Bridge if an external game layer, editor, tool or future server adapter needs to send actions without reaching into runtime internals:

```text
examples/action_bridge.cpp
docs/action-bridge.md
```

Start with custom game data if you want to define your own resources/buildings/settlements:

```text
examples/custom_data_registry.cpp
```

Start with the explicit flow if you want to integrate your own runtime world:

```text
examples/full_runtime_flow.cpp
```

Start with faction and ownership permissions if your game has faction-owned settlements/caravans:

```text
examples/faction_ownership_contracts.cpp
```

Start with the C ABI if you are integrating from C, C#, Unity or another language/runtime that should not call the C++ API directly:

```text
examples/c_abi_consumer/
examples/csharp_unity/
```

Start with the basic bootstrap if you want a minimal smoke test:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
```

Then add routes, caravans, contracts, ticks, persistence, validation and Action Bridge dispatch through the runtime workflow APIs.
