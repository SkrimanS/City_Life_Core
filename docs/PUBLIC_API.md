# City Life Core Public API / Публичный API

Version: **0.9.9**  
Status: **pre-1.0 audit build / сборка для аудита перед 1.0**

This document describes the current public C++ API surface of City Life Core. The API is usable today, but it is still pre-1.0.0: names and structures can still change while the SDK surface is being finalized.

Этот документ описывает текущий публичный C++ API City Life Core. API уже можно использовать, но проект ещё не достиг 1.0.0: имена и структуры могут уточняться до финальной стабилизации SDK.

---

## Recommended SDK include / Рекомендуемый SDK include

For most external SDK users, start with the umbrella header:

```cpp
#include "clc/CityLifeCore.hpp"
```

Для большинства внешних SDK users рекомендуется начинать с umbrella header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The umbrella header includes the recommended stable-candidate runtime/economy/data surface. It intentionally does **not** include every installed header. Experimental, raw persistence, and internal-risk headers may still be included directly when needed, but they should not be treated as the default public SDK entry point.

Umbrella header включает рекомендуемый stable-candidate runtime/economy/data surface. Он намеренно **не** включает каждый установленный header. Experimental, raw persistence и internal-risk headers можно подключать напрямую при необходимости, но их не следует считать главным SDK entry point.

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

Contracts support day and tick deadlines. Reward helpers can fulfill contracts and record ledger entries.

### Runtime API

Основные headers:

```cpp
#include "clc/sim/SimulationRuntime.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
```

Recommended runtime path:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

### Economy API

Основные headers:

```cpp
#include "clc/economy/Market.hpp"
#include "clc/economy/Trade.hpp"
#include "clc/economy/Ledger.hpp"
```

Recommended trade path:

```cpp
clc::economy::buy_resource_with_ledger(wallet, storage, price, quantity, ledger);
clc::economy::sell_resource_with_ledger(wallet, storage, price, quantity, ledger);
```

### Persistence and diagnostics

Raw world-state persistence remains broader than the default recommended SDK surface. Prefer runtime validation/diagnostics APIs unless you explicitly need save/load bridge internals.

---

## English

### Overview

City Life Core is split into several layers:

1. **Core** — version, basic IDs, time, world lifecycle, event log.
2. **Data** — definitions, registry, data pack loading, validation.
3. **Simulation** — settlements, storage, engine, routes, caravans, factions, ownership, contracts, runtime workflows.
4. **Economy** — market, trade, wallet, ledger, orders.
5. **Persistence** — snapshot/world-state/runtime save-load, validation, deterministic equivalence.

Primary namespaces:

```cpp
namespace clc {}
namespace clc::data {}
namespace clc::sim {}
namespace clc::economy {}
```

For the full header-by-header stability table, see:

```text
docs/PUBLIC_API_STATUS.md
```
