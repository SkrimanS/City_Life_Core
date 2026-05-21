# City Life Core SDK Structure / Структура SDK

Version: **0.9.9**  
Status: **pre-1.0 audit build / сборка для аудита перед 1.0**

This document describes the intended SDK and release structure for City Life Core before 1.0.0. The project is currently a source-first C++ SDK. Binary packages and C ABI are not finalized yet.

Этот документ описывает целевую структуру SDK и релиза City Life Core перед 1.0.0. Сейчас проект является source-first C++ SDK. Бинарные пакеты и C ABI ещё не финализированы.

---

## Русский

### Цель SDK

SDK должен позволять внешнему проекту подключить City Life Core как независимое headless-ядро симуляции:

- без графики;
- без клиентского UI;
- без зависимости от конкретного игрового движка;
- с понятной C++ API поверх runtime workflows;
- с day-based и tick-based runtime;
- с поддержкой real-time/MMO сценариев через секунды, минуты и часы;
- с тестируемым persistence/replay поведением;
- с будущей возможностью server-authoritative backend integration.

### Текущая структура репозитория

```text
apps/
  clc_runner/                  # минимальный CLI/bootstrap runner
examples/                      # SDK examples for external users
  basic_runtime.cpp
  save_load_roundtrip.cpp
  replay_persistence.cpp
include/
  clc/
    core/                      # version, time, world, event log
    data/                      # definitions, registry, validation, data packs
    economy/                   # market, trade, ledger, orders
    sim/                       # simulation, runtime, persistence, workflows
src/
  clc/                         # implementation files
tests/                         # executable tests registered through CMake
data/
  demo_fantasy/                # demo data pack material
docs/                          # public documentation
CMakeLists.txt
README.md
CHANGELOG.md
```

### Целевая release SDK layout

Для source/audit release:

```text
city-life-core-sdk-0.9.9/
  include/
    clc/
      core/
      data/
      economy/
      sim/
  src/
    clc/
  examples/
    basic_runtime.cpp
    save_load_roundtrip.cpp
    replay_persistence.cpp
  data/
    demo_fantasy/
  docs/
    PUBLIC_API.md
    SDK_STRUCTURE.md
    PACKAGING.md
    RELEASE_NOTES_0.9.9.md
  tests/                       # optional, for integrators and auditors
  CMakeLists.txt
  README.md
  CHANGELOG.md
```

Для будущего binary release:

```text
city-life-core-sdk-1.0.0/
  include/
    clc/
  lib/
    cmake/CityLifeCore/
    <platform libraries>
  bin/
    clc_runner                 # optional tools
    clc_example_basic_runtime
    clc_example_save_load_roundtrip
    clc_example_replay_persistence
  data/
    demo_fantasy/
  docs/
    PUBLIC_API.md
    SDK_STRUCTURE.md
    PACKAGING.md
  examples/
    basic_runtime.cpp
    save_load_roundtrip.cpp
    replay_persistence.cpp
```

### Public include policy

Публичными считаются headers внутри `include/clc/`.

Рекомендуемые точки входа:

```cpp
#include "clc/core/Version.hpp"
#include "clc/core/Time.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Definitions.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Внешнему коду лучше начинать с runtime-level API, а не собирать все подсистемы вручную:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
```

### C++ SDK boundary в 0.9.9

В C++ SDK входят:

- definitions and registry;
- validation reports;
- resource storage;
- simulation engine;
- settlement day and partial tick simulation;
- settlement routes with day/tick travel;
- caravans with day/tick progress;
- factions and ownership;
- contracts, `due_day`, `due_ticks`, rewards, and ledger integration;
- economy ledger;
- runtime workflows;
- tick-based runtime helpers;
- day/tick runtime event diagnostics;
- runtime save/load;
- semantic runtime validation;
- legacy save compatibility for missing `time` and missing `due_ticks`.

Пока не считать стабильным:

- окончательные имена всех workflow helpers;
- binary package layout;
- C ABI;
- binary compatibility;
- plugin/data-pack ABI;
- network/server action protocol.

### SDK examples

При `CLC_BUILD_EXAMPLES=ON` CMake собирает:

- `clc_example_basic_runtime` — bootstrap runtime, ticks, summary;
- `clc_example_save_load_roundtrip` — save/load roundtrip validation;
- `clc_example_replay_persistence` — midpoint persistence and deterministic replay continuation.

```bash
cmake -S . -B build -DCLC_BUILD_EXAMPLES=ON
cmake --build build
./build/clc_example_basic_runtime
./build/clc_example_save_load_roundtrip
./build/clc_example_replay_persistence
```

### Recommended integration modes

#### 1. CMake subdirectory

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

#### 2. Source vendoring

Copy `include/`, `src/`, `examples/`, `data/`, `docs/`, and `CMakeLists.txt` into your vendor tree and link `CityLifeCore::core`.

#### 3. Package mode

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

### Future C ABI

C ABI пока не готов. Его стоит начинать только после стабилизации C++ SDK surface.

Цели будущего C ABI:

- использовать ядро из C, C#, Rust, Python bindings, game engines;
- скрыть C++ ABI;
- дать opaque handles для runtime/registry/reports;
- вернуть validation errors через plain C structs или callback buffers.

C ABI не должен появляться раньше, чем будут закрыты:

- Public SDK API naming;
- runtime tick consequences;
- persistence/replay stability;
- release package layout;
- external docs.

### Release readiness checklist

Перед merge/release block:

- version updated in `CMakeLists.txt`;
- version updated in `include/clc/core/Version.hpp`;
- smoke test expects the same version;
- README reflects current phase;
- CHANGELOG has release notes;
- PUBLIC_API is up to date;
- SDK_STRUCTURE is up to date;
- PACKAGING is up to date;
- release notes exist for the target version;
- SDK examples build registration is up to date;
- new tests are registered in CMake;
- `main` receives only completed blocks;
- no force push;
- no accidental PR/CI run unless explicitly requested.

---

## English

### SDK goal

The SDK should let external projects integrate City Life Core as an independent headless simulation core:

- no rendering dependency;
- no client UI dependency;
- no game-engine lock-in;
- clear C++ API around runtime workflows;
- day-based and tick-based runtime;
- real-time/MMO scenarios measured in seconds, minutes, and hours;
- testable persistence/replay behavior;
- future server-authoritative backend integration.

### Current repository structure

```text
apps/
  clc_runner/                  # minimal CLI/bootstrap runner
examples/                      # SDK examples for external users
  basic_runtime.cpp
  save_load_roundtrip.cpp
  replay_persistence.cpp
include/
  clc/
    core/                      # version, time, world, event log
    data/                      # definitions, registry, validation, data packs
    economy/                   # market, trade, ledger, orders
    sim/                       # simulation, runtime, persistence, workflows
src/
  clc/                         # implementation files
tests/                         # executable tests registered through CMake
data/
  demo_fantasy/                # demo data pack material
docs/                          # public documentation
CMakeLists.txt
README.md
CHANGELOG.md
```

### Target release SDK layout

Source/audit release:

```text
city-life-core-sdk-0.9.9/
  include/
    clc/
      core/
      data/
      economy/
      sim/
  src/
    clc/
  examples/
    basic_runtime.cpp
    save_load_roundtrip.cpp
    replay_persistence.cpp
  data/
    demo_fantasy/
  docs/
    PUBLIC_API.md
    SDK_STRUCTURE.md
    PACKAGING.md
    RELEASE_NOTES_0.9.9.md
  tests/                       # optional for integrators and auditors
  CMakeLists.txt
  README.md
  CHANGELOG.md
```

Future binary release:

```text
city-life-core-sdk-1.0.0/
  include/
    clc/
  lib/
    cmake/CityLifeCore/
    <platform libraries>
  bin/
    clc_runner                 # optional tools
    clc_example_basic_runtime
    clc_example_save_load_roundtrip
    clc_example_replay_persistence
  data/
    demo_fantasy/
  docs/
    PUBLIC_API.md
    SDK_STRUCTURE.md
    PACKAGING.md
  examples/
    basic_runtime.cpp
    save_load_roundtrip.cpp
    replay_persistence.cpp
```

### Public include policy

Headers under `include/clc/` are the public include surface.

Recommended entry points:

```cpp
#include "clc/core/Version.hpp"
#include "clc/core/Time.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Definitions.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

External code should prefer runtime-level APIs over manually wiring every subsystem:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
```

### C++ SDK boundary in 0.9.9

The C++ SDK currently includes:

- definitions and registry;
- validation reports;
- resource storage;
- simulation engine;
- settlement day and partial tick simulation;
- settlement routes with day/tick travel;
- caravans with day/tick progress;
- factions and ownership;
- contracts, `due_day`, `due_ticks`, rewards, and ledger integration;
- economy ledger;
- runtime workflows;
- tick-based runtime helpers;
- day/tick runtime event diagnostics;
- runtime save/load;
- semantic runtime validation;
- legacy save compatibility for missing `time` and missing `due_ticks`.

Still not fully stable:

- final workflow helper naming;
- binary package layout;
- C ABI;
- binary compatibility;
- plugin/data-pack ABI;
- network/server action protocol.

### SDK examples

When `CLC_BUILD_EXAMPLES=ON`, CMake builds:

- `clc_example_basic_runtime` — runtime bootstrap, ticks, and summary;
- `clc_example_save_load_roundtrip` — save/load roundtrip validation;
- `clc_example_replay_persistence` — midpoint persistence and deterministic replay continuation.

```bash
cmake -S . -B build -DCLC_BUILD_EXAMPLES=ON
cmake --build build
./build/clc_example_basic_runtime
./build/clc_example_save_load_roundtrip
./build/clc_example_replay_persistence
```

### Recommended integration modes

#### 1. CMake subdirectory

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

#### 2. Source vendoring

Copy `include/`, `src/`, `examples/`, `data/`, `docs/`, and `CMakeLists.txt` into your vendor tree and link `CityLifeCore::core`.

#### 3. Package mode

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

### Future C ABI

C ABI is not ready yet. It should start only after the C++ SDK surface is stable.

Expected C ABI goals:

- use the core from C, C#, Rust, Python bindings, and game engines;
- hide C++ ABI details;
- provide opaque handles for runtime/registry/reports;
- expose validation errors through plain C structs or callback buffers.

C ABI should not begin before:

- Public SDK API naming is stable;
- runtime tick consequences are complete;
- persistence/replay stability is strong;
- release package layout is defined;
- external docs exist.

### Release readiness checklist

Before each merge/release block:

- version updated in `CMakeLists.txt`;
- version updated in `include/clc/core/Version.hpp`;
- smoke test expects the same version;
- README reflects current phase;
- CHANGELOG has release notes;
- PUBLIC_API is up to date;
- SDK_STRUCTURE is up to date;
- PACKAGING is up to date;
- release notes exist for the target version;
- SDK examples build registration is up to date;
- new tests are registered in CMake;
- `main` receives only completed blocks;
- no force push;
- no accidental PR/CI run unless explicitly requested.
