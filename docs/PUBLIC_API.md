# City Life Core Public API / Публичный API

Version: **0.9.3**

This document describes the current public C++ API surface of City Life Core. The API is usable today, but it is still pre-1.0.0: names and structures can still change while the SDK surface is being finalized.

Этот документ описывает текущий публичный C++ API City Life Core. API уже можно использовать, но проект ещё не достиг 1.0.0: имена и структуры могут уточняться до финальной стабилизации SDK.

---

## Русский

### Общая модель

City Life Core разделён на несколько уровней:

1. **Core** — версия, базовые ID, время, world lifecycle, event log.
2. **Data** — definitions, registry, data pack loading, validation.
3. **Simulation** — settlements, storage, engine, routes, caravans, factions, ownership, contracts, runtime workflows.
4. **Economy** — market, trade, wallet, ledger, orders.
5. **Persistence** — snapshot/world-state/runtime save-load, validation, deterministic equivalence.

Главный namespace:

```cpp
namespace clc {}
namespace clc::data {}
namespace clc::sim {}
namespace clc::economy {}
```

### Core API

Основные headers:

```cpp
#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"
#include "clc/core/Time.hpp"
#include "clc/core/EventLog.hpp"
```

Ключевые возможности:

- `clc::core_version()` — compile-time version object.
- `clc::core_version_string()` — строковая версия ядра.
- базовые world lifecycle primitives;
- in-memory event log;
- игровые time primitives.

Пример:

```cpp
#include "clc/core/Version.hpp"

static_assert(clc::core_version().major == 0);
auto version = clc::core_version_string();
```

### Data Registry API

Основные headers:

```cpp
#include "clc/data/Definitions.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/DataPackLoader.hpp"
#include "clc/data/Validation.hpp"
```

Главные типы definitions:

- `ResourceDefinition`
- `CurrencyDefinition`
- `BuildingDefinition`
- `ProfessionDefinition`
- `SettlementDefinition`

`DataRegistry` отвечает за добавление, хранение и lookup definitions:

```cpp
clc::data::DataRegistry registry;
registry.add(clc::data::ResourceDefinition{
    .id = "grain",
    .display_name = "Grain",
    .category = "food",
    .base_value = 10,
});

const auto* grain = registry.resource("grain");
```

Validation convention:

- большинство операций возвращают `clc::data::ValidationReport`;
- `report.ok()` означает отсутствие errors;
- warnings и errors хранятся как messages.

### Settlement and Storage API

Основные headers:

```cpp
#include "clc/sim/Storage.hpp"
#include "clc/sim/Settlement.hpp"
```

Ключевые типы:

- `ResourceStorage`
- `SettlementState`
- `BuildingInstance`
- `SettlementTickReport`
- `SettlementReport`

`ResourceStorage` — контролируемый контейнер ресурсов:

```cpp
clc::sim::ResourceStorage storage;
storage.add("grain", 50);
storage.try_remove("grain", 10);
auto amount = storage.amount("grain");
```

Settlement simulation поддерживает:

- population;
- food consumption;
- building input consumption;
- building output production;
- deterministic settlement reports.

### Economy API

Основные headers:

```cpp
#include "clc/economy/Market.hpp"
#include "clc/economy/Trade.hpp"
#include "clc/economy/Ledger.hpp"
#include "clc/economy/Orders.hpp"
```

Ключевые области:

- market demand and price reports;
- wallet operations;
- buy/sell trade transactions;
- economy ledger;
- market orders.

Ledger сохраняет последовательные entries и используется contract reward flows.

### Simulation Engine API

Основной header:

```cpp
#include "clc/sim/SimulationEngine.hpp"
```

`SimulationEngine` содержит:

- `DataRegistry`;
- `MarketState`;
- settlement list;
- current day;
- cumulative engine events.

Пример:

```cpp
clc::sim::SimulationEngine engine{registry};
engine.create_settlement("riverwatch");
engine.add_resource_to_settlement("riverwatch", "grain", 50);

const auto day = engine.advance_day();
const auto snapshot = engine.snapshot();
```

Useful APIs:

- `create_settlement()`
- `add_resource_to_settlement()`
- `remove_resource_from_settlement()`
- `transfer_resource_between_settlements()`
- `advance_day()`
- `run_days()`
- `snapshot()`
- `export_state()` / `restore_state()`
- `events()`, `recent_events()`, `events_by_type()`

### Routes and Caravans API

Основные headers:

```cpp
#include "clc/sim/Routes.hpp"
#include "clc/sim/Caravans.hpp"
```

Routes describe movement between settlements:

```cpp
clc::sim::SettlementRoute route{
    .id = "riverwatch_to_hillford",
    .display_name = "Riverwatch to Hillford",
    .origin_settlement_id = "riverwatch",
    .destination_settlement_id = "hillford",
    .travel_days = 2,
};
```

Caravans carry cargo and progress by days:

```cpp
auto caravan = clc::sim::create_caravan_for_route(
    route,
    "caravan_01",
    "Caravan 01"
);

clc::sim::advance_caravan_day(caravan);
```

Runtime day ticks now expose arrival consequences through `SimulationRuntimeDayReport::arrived_caravan_ids`.

### Factions and Ownership API

Основные headers:

```cpp
#include "clc/sim/Factions.hpp"
#include "clc/sim/Ownership.hpp"
```

Supported concepts:

- faction catalog;
- faction reputation;
- settlement ownership;
- caravan ownership;
- reference validation.

### Contracts API

Основные headers:

```cpp
#include "clc/sim/Contracts.hpp"
#include "clc/sim/ContractRewards.hpp"
```

Supported contract model:

- resource delivery contract;
- issuer faction;
- receiver faction;
- required resource and quantity;
- reward coins;
- due day;
- status;
- fulfillment from arrived caravan;
- overdue failure through runtime day ticks;
- reward + ledger integration.

Example:

```cpp
clc::sim::ResourceDeliveryContract contract{
    .id = "grain_delivery",
    .display_name = "Grain Delivery",
    .issuer_faction_id = "riverwatch",
    .receiver_faction_id = "traders_guild",
    .resource_id = "grain",
    .quantity = 30,
    .reward_coins = 75,
    .due_day = 8,
};
```

### Runtime API

Основные headers:

```cpp
#include "clc/sim/SimulationRuntime.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
```

`SimulationRuntime` bundles the major runtime subsystems:

- engine;
- routes;
- caravans;
- factions;
- ownership;
- contracts;
- wallet;
- ledger.

Recommended entry point for examples and tests:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return 1;
}

auto& runtime = bootstrap.runtime;
```

Workflow helpers:

- `create_runtime_settlement()`
- `add_runtime_route()`
- `add_runtime_faction()`
- `set_runtime_faction_reputation()`
- `set_runtime_settlement_owner()`
- `set_runtime_caravan_owner()`
- `add_runtime_resource_delivery_contract()`
- `create_runtime_caravan_for_route()`
- `load_runtime_caravan_at_origin()`
- `advance_runtime_caravan_day()`
- `unload_runtime_caravan_at_destination()`
- `fulfill_runtime_contract_from_arrived_caravan_with_reward_and_ledger()`
- `fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger()`

Runtime tick reports:

- `advance_runtime_day()` advances engine and caravans;
- `SimulationRuntimeDayReport::arrived_caravan_ids` lists caravans that arrived on that tick;
- `SimulationRuntimeDayReport::contracts` contains overdue contract failures for the tick;
- `SimulationRuntimeRunSummary::contract_failures` aggregates failed contracts across a run.

Runtime events:

- `runtime.day.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

### Persistence API

Основные headers:

```cpp
#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Persistence layers:

- snapshot persistence;
- world-state persistence;
- runtime persistence;
- runtime save-load validation;
- semantic runtime equivalence validation.

Common runtime roundtrip:

```cpp
clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};

auto result = clc::sim::validate_simulation_runtime_save_load_roundtrip(
    runtime,
    loaded,
    "runtime.clcs"
);

if (!result.ok()) {
    // inspect result.validation / result.load
}
```

Semantic runtime comparison:

```cpp
auto report = clc::sim::validate_simulation_runtimes_match(expected, actual);
if (!report.ok()) {
    // runtime drift detected
}
```

Current validation covers:

- engine day/state/events;
- market demands;
- registry counts;
- referenced resource and settlement definition semantics;
- routes;
- caravans and cargo;
- factions and reputation;
- ownership;
- contracts;
- wallet;
- ledger.

### Stability notes before 1.0.0

Stable enough for internal integration:

- data definitions;
- registry lookup;
- storage operations;
- runtime scenario bootstrap;
- runtime workflow helpers;
- runtime tick reports;
- runtime event diagnostics;
- runtime persistence validation.

Still evolving:

- public SDK packaging;
- deeper settlement-side arrival consequences;
- external C ABI;
- binary distribution layout;
- final naming of some workflow helpers.

---

## English

### Overall model

City Life Core is split into several layers:

1. **Core** — version, IDs, time, world lifecycle, event log.
2. **Data** — definitions, registry, data pack loading, validation.
3. **Simulation** — settlements, storage, engine, routes, caravans, factions, ownership, contracts, runtime workflows.
4. **Economy** — market, trade, wallet, ledger, orders.
5. **Persistence** — snapshot/world-state/runtime save-load, validation, deterministic equivalence.

Main namespaces:

```cpp
namespace clc {}
namespace clc::data {}
namespace clc::sim {}
namespace clc::economy {}
```

### Core API

Main headers:

```cpp
#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"
#include "clc/core/Time.hpp"
#include "clc/core/EventLog.hpp"
```

Key features:

- `clc::core_version()` — compile-time version object.
- `clc::core_version_string()` — string version.
- world lifecycle primitives;
- in-memory event log;
- game time primitives.

### Data Registry API

Main headers:

```cpp
#include "clc/data/Definitions.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/DataPackLoader.hpp"
#include "clc/data/Validation.hpp"
```

Definition types:

- `ResourceDefinition`
- `CurrencyDefinition`
- `BuildingDefinition`
- `ProfessionDefinition`
- `SettlementDefinition`

`DataRegistry` stores and validates definitions:

```cpp
clc::data::DataRegistry registry;
registry.add(clc::data::ResourceDefinition{
    .id = "grain",
    .display_name = "Grain",
    .category = "food",
    .base_value = 10,
});

const auto* grain = registry.resource("grain");
```

### Settlement and Storage API

Main headers:

```cpp
#include "clc/sim/Storage.hpp"
#include "clc/sim/Settlement.hpp"
```

Key types:

- `ResourceStorage`
- `SettlementState`
- `BuildingInstance`
- `SettlementTickReport`
- `SettlementReport`

`ResourceStorage` is a controlled resource container:

```cpp
clc::sim::ResourceStorage storage;
storage.add("grain", 50);
storage.try_remove("grain", 10);
auto amount = storage.amount("grain");
```

### Economy API

Main headers:

```cpp
#include "clc/economy/Market.hpp"
#include "clc/economy/Trade.hpp"
#include "clc/economy/Ledger.hpp"
#include "clc/economy/Orders.hpp"
```

Supported areas:

- market demand and price reports;
- wallet operations;
- buy/sell trade transactions;
- economy ledger;
- market orders.

### Simulation Engine API

Main header:

```cpp
#include "clc/sim/SimulationEngine.hpp"
```

`SimulationEngine` owns:

- data registry;
- market state;
- settlements;
- current day;
- cumulative events.

```cpp
clc::sim::SimulationEngine engine{registry};
engine.create_settlement("riverwatch");
engine.add_resource_to_settlement("riverwatch", "grain", 50);

const auto day = engine.advance_day();
const auto snapshot = engine.snapshot();
```

### Routes and Caravans API

Main headers:

```cpp
#include "clc/sim/Routes.hpp"
#include "clc/sim/Caravans.hpp"
```

Caravans carry cargo, progress by days, and surface arrival IDs through runtime day reports.

### Contracts API

Main headers:

```cpp
#include "clc/sim/Contracts.hpp"
#include "clc/sim/ContractRewards.hpp"
```

Supported contract model:

- resource delivery contracts;
- issuer/receiver factions;
- required resource and quantity;
- reward coins;
- due day and status;
- fulfillment from arrived caravan;
- overdue failure through runtime ticks;
- reward and ledger integration.

### Runtime API

Main headers:

```cpp
#include "clc/sim/SimulationRuntime.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
```

`SimulationRuntime` is the recommended integration surface for game/server code. It bundles engine, routes, caravans, factions, ownership, contracts, wallet, and ledger.

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return 1;
}

auto& runtime = bootstrap.runtime;
```

Runtime tick reports:

- `advance_runtime_day()` advances engine and caravans;
- `SimulationRuntimeDayReport::arrived_caravan_ids` lists caravans that arrived on that tick;
- `SimulationRuntimeDayReport::contracts` contains overdue contract failures for the tick;
- `SimulationRuntimeRunSummary::contract_failures` aggregates failed contracts across a run.

Runtime events:

- `runtime.day.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

### Persistence API

Main headers:

```cpp
#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Runtime roundtrip:

```cpp
clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};

auto result = clc::sim::validate_simulation_runtime_save_load_roundtrip(
    runtime,
    loaded,
    "runtime.clcs"
);
```

Runtime equivalence:

```cpp
auto report = clc::sim::validate_simulation_runtimes_match(expected, actual);
if (!report.ok()) {
    // runtime drift detected
}
```

### Stability notes before 1.0.0

Ready for internal integration:

- data definitions;
- registry lookup;
- storage operations;
- runtime scenario bootstrap;
- runtime workflow helpers;
- runtime tick reports;
- runtime event diagnostics;
- runtime persistence validation.

Still evolving:

- public SDK packaging;
- deeper settlement-side arrival consequences;
- external C ABI;
- binary distribution layout;
- final naming of some workflow helpers.
