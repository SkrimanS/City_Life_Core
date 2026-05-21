# City Life Core SDK Structure / Структура SDK

Version: **0.9.2**

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
- с тестируемым persistence/replay поведением;
- с будущей возможностью server-authoritative backend integration.

### Текущая структура репозитория

```text
apps/
  clc_runner/                  # минимальный CLI/bootstrap runner
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

Для source release:

```text
city-life-core-sdk-0.9.2/
  include/
    clc/
      core/
      data/
      economy/
      sim/
  src/
    clc/
  data/
    demo_fantasy/
  docs/
    PUBLIC_API.md
    SDK_STRUCTURE.md
  tests/                       # optional, for integrators
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
  data/
    demo_fantasy/
  docs/
    PUBLIC_API.md
    SDK_STRUCTURE.md
  examples/
    basic_runtime/
    save_load_roundtrip/
    replay_persistence/
```

### Public include policy

Публичными считаются headers внутри `include/clc/`.

Рекомендуемые точки входа:

```cpp
#include "clc/core/Version.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Definitions.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Внешнему коду лучше начинать с runtime-level API, а не собирать все подсистемы вручную:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
```

### C++ SDK boundary

В C++ SDK входят:

- definitions and registry;
- validation reports;
- resource storage;
- simulation engine;
- settlement routes;
- caravans;
- factions and ownership;
- contracts and rewards;
- economy ledger;
- runtime workflows;
- runtime save/load;
- semantic runtime validation.

Пока не считать стабильным:

- окончательные имена всех workflow helpers;
- packaging layout;
- C ABI;
- binary compatibility;
- plugin/data-pack ABI;
- network/server action protocol.

### Future C ABI

C ABI пока не готов. Его стоит начинать только после стабилизации C++ SDK surface.

Предполагаемая цель C ABI:

- использовать ядро из C, C#, Rust, Python bindings, game engines;
- скрыть C++ ABI;
- дать opaque handles для runtime/registry/reports;
- вернуть validation errors через plain C structs или callback buffers.

Потенциальная будущая структура:

```text
include/
  clc/
    c_api/
      clc_api.h
      clc_runtime.h
      clc_registry.h
      clc_persistence.h
```

C ABI не должен появляться раньше, чем будут закрыты:

- Public SDK API naming;
- runtime tick consequences;
- persistence/replay stability;
- release package layout;
- external docs.

### Release readiness checklist

Перед каждым release block:

- version updated in `CMakeLists.txt`;
- version updated in `include/clc/core/Version.hpp`;
- README reflects current phase;
- CHANGELOG has release notes;
- PUBLIC_API is up to date;
- SDK_STRUCTURE is up to date;
- new tests are registered in CMake;
- `main` receives only completed blocks by fast-forward;
- no force push;
- no accidental PR/CI run unless explicitly requested.

### Recommended integration modes

#### 1. CMake subdirectory

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

#### 2. Source vendoring

Copy `include/`, `src/`, and `CMakeLists.txt` into your vendor tree and link `CityLifeCore::core`.

#### 3. Future package mode

Not finalized yet. Target direction:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

---

## English

### SDK goal

The SDK should let external projects integrate City Life Core as an independent headless simulation core:

- no rendering dependency;
- no client UI dependency;
- no game-engine lock-in;
- clear C++ API around runtime workflows;
- testable persistence/replay behavior;
- future server-authoritative backend integration.

### Current repository structure

```text
apps/
  clc_runner/                  # minimal CLI/bootstrap runner
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

Source release:

```text
city-life-core-sdk-0.9.2/
  include/
    clc/
      core/
      data/
      economy/
      sim/
  src/
    clc/
  data/
    demo_fantasy/
  docs/
    PUBLIC_API.md
    SDK_STRUCTURE.md
  tests/                       # optional for integrators
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
  data/
    demo_fantasy/
  docs/
    PUBLIC_API.md
    SDK_STRUCTURE.md
  examples/
    basic_runtime/
    save_load_roundtrip/
    replay_persistence/
```

### Public include policy

Headers under `include/clc/` are the public include surface.

Recommended entry points:

```cpp
#include "clc/core/Version.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Definitions.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

External code should prefer runtime-level APIs over manually wiring every subsystem:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
```

### C++ SDK boundary

The C++ SDK currently includes:

- definitions and registry;
- validation reports;
- resource storage;
- simulation engine;
- settlement routes;
- caravans;
- factions and ownership;
- contracts and rewards;
- economy ledger;
- runtime workflows;
- runtime save/load;
- semantic runtime validation.

Still not fully stable:

- final workflow helper naming;
- packaging layout;
- C ABI;
- binary compatibility;
- plugin/data-pack ABI;
- network/server action protocol.

### Future C ABI

C ABI is not ready yet. It should start only after the C++ SDK surface is stable.

Expected C ABI goals:

- use the core from C, C#, Rust, Python bindings, and game engines;
- hide C++ ABI details;
- provide opaque handles for runtime/registry/reports;
- expose validation errors through plain C structs or callback buffers.

Potential future structure:

```text
include/
  clc/
    c_api/
      clc_api.h
      clc_runtime.h
      clc_registry.h
      clc_persistence.h
```

C ABI should not begin before:

- Public SDK API naming is stable;
- runtime tick consequences are complete;
- persistence/replay stability is strong;
- release package layout is defined;
- external docs exist.

### Release readiness checklist

Before each release block:

- version updated in `CMakeLists.txt`;
- version updated in `include/clc/core/Version.hpp`;
- README reflects current phase;
- CHANGELOG has release notes;
- PUBLIC_API is up to date;
- SDK_STRUCTURE is up to date;
- new tests are registered in CMake;
- `main` receives only completed blocks by fast-forward;
- no force push;
- no accidental PR/CI run unless explicitly requested.

### Recommended integration modes

#### 1. CMake subdirectory

```cmake
add_subdirectory(external/City_Life_Core)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

#### 2. Source vendoring

Copy `include/`, `src/`, and `CMakeLists.txt` into your vendor tree and link `CityLifeCore::core`.

#### 3. Future package mode

Not finalized yet. Target direction:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```
