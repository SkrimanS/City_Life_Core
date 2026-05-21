# City Life Core Public API / Публичный API

Version: **0.9.9**  
Status: **pre-1.0 audit build / сборка для аудита перед 1.0**

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

Главные namespaces:

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

- `clc::core_version()` — compile-time version object;
- `clc::core_version_string()` — строковая версия ядра;
- `clc::GameTime` — runtime tick clock;
- `ticks_per_second()`, `ticks_per_minute()`, `ticks_per_hour()`, `ticks_per_day()`;
- `seconds_to_ticks(...)`, `minutes_to_ticks(...)`, `hours_to_ticks(...)`, `days_to_ticks(...)`;
- in-memory `EventLog`.

Пример:

```cpp
#include "clc/core/Version.hpp"
#include "clc/core/Time.hpp"

static_assert(clc::core_version().minor == 9);
auto two_hours = clc::hours_to_ticks(2);
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
- `SettlementTickRemainder`
- `SettlementReport`

Settlement simulation поддерживает:

- population;
- food consumption;
- building input consumption;
- building output production;
- day advancement;
- partial tick advancement with deterministic remainders;
- deterministic settlement reports.

### Routes and Caravans API

Основные headers:

```cpp
#include "clc/sim/Routes.hpp"
#include "clc/sim/Caravans.hpp"
```

Routes могут быть day-based или tick-based:

```cpp
auto route = clc::sim::make_settlement_route_ticks(
    "riverwatch_to_hillford_3h",
    "Riverwatch to Hillford 3h",
    "riverwatch",
    "hillford",
    clc::hours_to_ticks(3)
);
```

Caravans carry cargo and progress by days or ticks:

```cpp
auto caravan = clc::sim::create_caravan_for_route(
    route,
    "caravan_01",
    "Caravan 01"
);

clc::sim::advance_caravan_ticks(caravan, clc::minutes_to_ticks(30));
```

Important helpers:

- `settlement_route_travel_ticks(...)`
- `caravan_total_travel_ticks(...)`
- `caravan_ticks_remaining(...)`
- `advance_caravan_ticks(...)`
- `advance_caravan_day(...)`
- `caravan_arrived(...)`

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
- `due_day` for day-based games;
- `due_ticks` for real-time/MMO runtime;
- status;
- fulfillment from arrived caravan;
- overdue failure through runtime ticks;
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
    .due_ticks = clc::hours_to_ticks(6),
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

- `engine`;
- `routes`;
- `caravans`;
- `factions`;
- `ownership`;
- `contracts`;
- `wallet`;
- `ledger`;
- `time`.

Recommended entry point:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return 1;
}

auto& runtime = bootstrap.runtime;
```

Workflow helpers:

- `create_runtime_settlement(...)`
- `add_runtime_route(...)`
- `add_runtime_faction(...)`
- `set_runtime_faction_reputation(...)`
- `set_runtime_settlement_owner(...)`
- `set_runtime_caravan_owner(...)`
- `add_runtime_resource_delivery_contract(...)`
- `create_runtime_caravan_for_route(...)`
- `load_runtime_caravan_at_origin(...)`
- `advance_runtime_caravan_day(...)`
- `unload_runtime_caravan_at_destination(...)`
- `deliver_runtime_arrived_caravan_cargo_to_destination(...)`
- `deliver_all_runtime_arrived_caravan_cargo_to_destinations(...)`
- `fulfill_runtime_contract_from_arrived_caravan_with_reward_and_ledger(...)`
- `fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(...)`

### Tick-based runtime API

`0.9.9` добавляет полноценный tick-based orchestration layer. Он нужен для real-time игр, MMO и серверов, где события происходят через секунды/минуты/часы, а не только через дни.

Главные функции:

```cpp
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(10));

clc::sim::run_runtime_ticks(
    runtime,
    clc::hours_to_ticks(2),
    clc::minutes_to_ticks(30)
);

clc::sim::run_runtime_until_first_caravan_arrival_by_ticks(
    runtime,
    clc::hours_to_ticks(6),
    clc::minutes_to_ticks(15)
);

clc::sim::run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(
    runtime,
    clc::hours_to_ticks(6),
    clc::minutes_to_ticks(15),
    "riverwatch"
);
```

Ключевые result types:

- `SimulationRuntimeTickReport`
- `SimulationRuntimeTickRunSummary`
- `SimulationRuntimeTickRunResult`
- `SimulationRuntimeTickRunUntilArrivalResult`
- `SimulationRuntimeTickArrivalContractResult`

Day-based functions remain available:

- `advance_runtime_day(...)`
- `run_runtime_days(...)`
- `run_runtime_until_first_caravan_arrival(...)`
- `run_runtime_until_first_caravan_arrival_and_fulfill_contract(...)`

Но для real-time/MMO рекомендуется tick-based API.

### Runtime Events / Diagnostics API

Event log helpers:

- `append_runtime_day_report_events(...)`
- `append_runtime_run_events(...)`
- `append_runtime_arrival_contract_events(...)`
- `append_runtime_tick_report_events(...)`
- `append_runtime_tick_run_events(...)`
- `append_runtime_tick_arrival_contract_events(...)`
- `append_runtime_caravan_cargo_delivery_event(...)`
- `append_runtime_bulk_caravan_cargo_delivery_events(...)`

Runtime events:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

All event timestamps are absolute runtime ticks, not day numbers.

Validation/analysis helpers:

- `analyze_runtime_event_log(...)`
- `validate_runtime_event_log_tick_order(...)`
- `validate_runtime_event_log_known_types(...)`
- `validate_runtime_event_log_payloads(...)`
- `validate_runtime_event_log(...)`
- `calculate_runtime_event_log_checksum(...)`
- `validate_runtime_event_logs_match(...)`

### Persistence API

Основные headers:

```cpp
#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Runtime persistence сохраняет:

- engine state;
- routes;
- caravans and cargo;
- factions;
- ownership;
- contracts;
- wallet;
- ledger entries;
- runtime `time`;
- caravan tick progress;
- contract `due_ticks`;
- settlement tick remainders.

Common runtime roundtrip:

```cpp
clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};

auto result = clc::sim::validate_simulation_runtime_save_load_roundtrip(
    runtime,
    loaded,
    "runtime.clcs"
);
```

Semantic runtime comparison:

```cpp
auto report = clc::sim::validate_simulation_runtimes_match(expected, actual);
if (!report.ok()) {
    // runtime drift detected
}
```

Compatibility:

- old saves without explicit `time` synchronize runtime clock from `current_day`;
- old contract rows without `due_ticks` derive tick deadlines from `due_day`.

### Stability notes before 1.0.0

Stable enough for 0.9.9 audit:

- data definitions;
- registry lookup;
- storage operations;
- day and tick time primitives;
- runtime scenario bootstrap;
- runtime workflow helpers;
- explicit runtime cargo delivery;
- tick-based runtime reports;
- day/tick runtime event diagnostics;
- runtime persistence validation.

Still evolving:

- public SDK naming freeze;
- broader settlement-side arrival effects;
- external C ABI;
- binary distribution layout;
- final 1.0 documentation pass.

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

- `clc::core_version()` — compile-time version object;
- `clc::core_version_string()` — string version;
- `clc::GameTime` — runtime tick clock;
- `ticks_per_second()`, `ticks_per_minute()`, `ticks_per_hour()`, `ticks_per_day()`;
- `seconds_to_ticks(...)`, `minutes_to_ticks(...)`, `hours_to_ticks(...)`, `days_to_ticks(...)`;
- in-memory `EventLog`.

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

Most mutating/validation operations return `clc::data::ValidationReport`.

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
- `SettlementTickRemainder`
- `SettlementReport`

Settlement simulation supports day advancement and partial tick advancement with deterministic remainders.

### Routes and Caravans API

Main headers:

```cpp
#include "clc/sim/Routes.hpp"
#include "clc/sim/Caravans.hpp"
```

Routes may be day-based or tick-based:

```cpp
auto route = clc::sim::make_settlement_route_ticks(
    "riverwatch_to_hillford_3h",
    "Riverwatch to Hillford 3h",
    "riverwatch",
    "hillford",
    clc::hours_to_ticks(3)
);
```

Caravans carry cargo and progress by days or ticks.

Important helpers:

- `settlement_route_travel_ticks(...)`
- `caravan_total_travel_ticks(...)`
- `caravan_ticks_remaining(...)`
- `advance_caravan_ticks(...)`
- `advance_caravan_day(...)`
- `caravan_arrived(...)`

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
- `due_day` for day-based games;
- `due_ticks` for real-time/MMO runtime;
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

`SimulationRuntime` is the recommended integration surface for game/server code. It bundles engine, routes, caravans, factions, ownership, contracts, wallet, ledger, and runtime time.

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return 1;
}

auto& runtime = bootstrap.runtime;
```

Runtime workflow helpers include explicit cargo delivery through `deliver_runtime_arrived_caravan_cargo_to_destination()` and `deliver_all_runtime_arrived_caravan_cargo_to_destinations()`.

### Tick-based runtime API

`0.9.9` adds a full tick-based orchestration layer for real-time games, MMOs, and servers where events happen after seconds, minutes, or hours instead of only after days.

Main functions:

```cpp
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(10));

clc::sim::run_runtime_ticks(
    runtime,
    clc::hours_to_ticks(2),
    clc::minutes_to_ticks(30)
);

clc::sim::run_runtime_until_first_caravan_arrival_by_ticks(
    runtime,
    clc::hours_to_ticks(6),
    clc::minutes_to_ticks(15)
);

clc::sim::run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(
    runtime,
    clc::hours_to_ticks(6),
    clc::minutes_to_ticks(15),
    "riverwatch"
);
```

Key result types:

- `SimulationRuntimeTickReport`
- `SimulationRuntimeTickRunSummary`
- `SimulationRuntimeTickRunResult`
- `SimulationRuntimeTickRunUntilArrivalResult`
- `SimulationRuntimeTickArrivalContractResult`

Day-based APIs remain available for turn-based/daily games, but tick-based APIs are the preferred integration path for real-time/MMO runtimes.

### Runtime Events / Diagnostics API

Event log helpers:

- `append_runtime_day_report_events(...)`
- `append_runtime_run_events(...)`
- `append_runtime_arrival_contract_events(...)`
- `append_runtime_tick_report_events(...)`
- `append_runtime_tick_run_events(...)`
- `append_runtime_tick_arrival_contract_events(...)`
- `append_runtime_caravan_cargo_delivery_event(...)`
- `append_runtime_bulk_caravan_cargo_delivery_events(...)`

Runtime events:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

All event timestamps are absolute runtime ticks, not day numbers.

### Persistence API

Main headers:

```cpp
#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Runtime persistence stores runtime time, route/caravan tick progress, contract tick deadlines, settlement tick remainders, and the existing world/runtime systems.

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

Compatibility:

- old saves without explicit `time` synchronize runtime clock from `current_day`;
- old contract rows without `due_ticks` derive tick deadlines from `due_day`.

### Stability notes before 1.0.0

Ready for 0.9.9 audit:

- data definitions;
- registry lookup;
- storage operations;
- day and tick time primitives;
- runtime scenario bootstrap;
- runtime workflow helpers;
- explicit runtime cargo delivery;
- tick-based runtime reports;
- day/tick runtime event diagnostics;
- runtime persistence validation.

Still evolving:

- public SDK naming freeze;
- broader settlement-side arrival effects;
- external C ABI;
- binary distribution layout;
- final 1.0 documentation pass.
