# City Life Core Public API / Публичный API

Version: **0.9.9**  
Status: **pre-1.0 audit build / сборка для аудита перед 1.0**

This document describes the current public C++ API surface of City Life Core. The API is usable today, but it is still pre-1.0.0: names and structures can still change while the SDK surface is being finalized.

Этот документ описывает текущий публичный C++ API City Life Core. API уже можно использовать, но проект ещё не достиг 1.0.0: имена и структуры могут уточняться до финальной стабилизации SDK.

For the full header-by-header stability table, see:

```text
docs/PUBLIC_API_STATUS.md
```

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
5. **Persistence / diagnostics** — snapshot/world-state/runtime save-load, validation, deterministic equivalence.

Главные namespaces:

```cpp
namespace clc {}
namespace clc::data {}
namespace clc::sim {}
namespace clc::economy {}
```

### Stability model

Все headers внутри `include/clc/` устанавливаются, но это не означает одинаковую стабильность.

Перед 1.0.0 API surface делится на:

- `stable-candidate` — ожидается как основной SDK surface;
- `experimental` — public, но может измениться;
- `diagnostics` — observability/testing/replay/validation API;
- `legacy` — сохранено для совместимости, не recommended path;
- `internal-risk` — установлено сейчас, но не должно считаться стабильным SDK API без явного freeze.

Для финального решения см. `docs/PUBLIC_API_STATUS.md`.

### Core API

Основные headers:

```cpp
#include "clc/core/Version.hpp"
#include "clc/core/Time.hpp"
#include "clc/core/EventLog.hpp"
```

Ключевые возможности:

- `clc::core_version()`;
- `clc::core_version_string()`;
- `clc::GameTime`;
- `ticks_per_second()`, `ticks_per_minute()`, `ticks_per_hour()`, `ticks_per_day()`;
- `seconds_to_ticks(...)`, `minutes_to_ticks(...)`, `hours_to_ticks(...)`, `days_to_ticks(...)`;
- `can_convert_*_to_ticks(...)` helpers;
- `GameTime::can_advance(...)`;
- in-memory `EventLog`.

Overflow policy для времени:

- conversion helpers saturate на `UINT64_MAX`;
- `can_convert_*_to_ticks(...)` позволяют отклонить слишком большие значения заранее;
- `GameTime::advance(...)` saturates вместо wraparound.

Пример:

```cpp
static_assert(clc::core_version().minor == 9);
auto two_hours = clc::hours_to_ticks(2);
```

### Data Registry API

Основные headers:

```cpp
#include "clc/data/Definitions.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"
```

Основные definitions:

- `ResourceDefinition`
- `CurrencyDefinition`
- `BuildingDefinition`
- `ProfessionDefinition`
- `SettlementDefinition`

Validation convention:

- большинство операций возвращают `clc::data::ValidationReport`;
- `report.ok()` означает отсутствие errors;
- warnings и errors хранятся как messages;
- API должен предпочитать validation result вместо silent failure.

`DataPackLoader.hpp` пока experimental, потому что schema/data-pack compatibility ещё не frozen.

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

Arithmetic policy:

- tick scaling и report totals защищены от `uint64_t` wraparound saturating arithmetic;
- storage operations возвращают validation report или bool для insufficient resources/overflow.

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

Important semantic note:

- `CaravanAdvanceReport::arrived` means “arrived after this advance”; for event semantics use `arrived && ticks_elapsed > 0`.

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
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
const auto report = clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

Runtime supports:

- settlements;
- routes;
- caravans;
- factions;
- ownership;
- contracts;
- wallet;
- ledger;
- runtime tick clock.

`SimulationRuntime` is currently a public mutable data-bag. This is convenient for SDK integration, but users should prefer workflow helpers where possible because direct mutation can bypass invariants.

### Contracts API

Основные headers:

```cpp
#include "clc/sim/Contracts.hpp"
#include "clc/sim/ContractRewards.hpp"
```

Contracts support:

- day deadlines;
- tick deadlines;
- open/fulfilled/failed/cancelled status;
- fulfillment from storage;
- fulfillment from arrived caravans;
- reward payout;
- reward ledger entry.

Recommended reward path:

```cpp
clc::sim::fulfill_contract_from_storage_with_reward_and_ledger(
    contracts,
    contract_id,
    delivered_storage,
    wallet,
    ledger
);
```

The reward+ledger helpers use staged mutation: contract/cargo/wallet/ledger are committed only after the whole operation succeeds.

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

Trade model note:

- current trade is an abstract market model;
- buy creates resource in local storage and spends wallet coins;
- sell removes resource from local storage and credits wallet coins;
- it is not a closed conservation economy unless the game builds one on top.

Market report note:

- market reports include registered demand-only resources even when supply is zero.

Ledger note:

- ledger entries are append-only;
- sequence uniqueness/monotonicity is validated on restore;
- recommended trade helpers write ledger entries automatically.

### Persistence and diagnostics

Основные diagnostics/runtime validation headers:

```cpp
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Raw world-state persistence bridge:

```cpp
#include "clc/sim/SimulationPersistence.hpp"
```

`SimulationPersistence.hpp` is powerful and installed, but it is broader than the default recommended SDK surface. Use it when you explicitly need save/load bridge internals, world-state export/import, or deterministic replay comparison.

Restore policy:

- restore validates state before mutation;
- runtime restore stages state and commits only after validation/ledger/engine restore succeeds;
- failed restore should not intentionally leave partial runtime mutation.

### Pointer/reference invalidation rules

Many APIs return pointers, references, `std::string_view`, or references/views to internal containers.

General rule:

- treat returned pointers/references/views as invalidated after any mutating operation on the owning object;
- do not store raw pointers returned from registry/catalog/runtime lookup APIs across mutations;
- store IDs instead and look up again after mutation;
- copied reports/snapshots are safe to keep, but live references are not.

Examples:

- pointers returned by `registry.resource(...)` can be invalidated if the registry is mutated;
- pointers returned by `contract_by_id(...)` can be invalidated if contracts are mutated;
- references returned by `ResourceStorage::entries()` should not be kept across storage mutation;
- runtime fields are public, so direct mutation can invalidate pointers into vectors/maps.

### Not the default public entry point

These installed headers are not recommended as the first integration point:

- `clc/data/DataPackLoader.hpp` — experimental until schema compatibility is frozen;
- `clc/economy/Orders.hpp` — experimental until order matching is part of the stable MVP;
- `clc/sim/ScenarioCatalog.hpp` — helper/demo layer;
- `clc/sim/SimulationPersistence.hpp` — raw persistence bridge/internal-risk surface; use deliberately.

---

## English

### Overview

City Life Core is split into several layers:

1. **Core** — version, IDs, time, lifecycle and event log.
2. **Data** — definitions, registry, data-pack loading and validation.
3. **Simulation** — settlements, storage, engine, routes, caravans, factions, ownership, contracts and runtime workflows.
4. **Economy** — market, trade, wallet, ledger and orders.
5. **Persistence / diagnostics** — snapshots, world-state save/load, validation and deterministic equivalence.

Primary namespaces:

```cpp
namespace clc {}
namespace clc::data {}
namespace clc::sim {}
namespace clc::economy {}
```

### Stability model

All headers under `include/clc/` are installed, but installed does not automatically mean equally stable.

Before 1.0.0 the public surface is classified as:

- `stable-candidate` — expected to be the primary SDK surface;
- `experimental` — public but may change;
- `diagnostics` — observability/testing/replay/validation API;
- `legacy` — kept for compatibility, not the preferred path;
- `internal-risk` — installed today, but not stable SDK API unless explicitly frozen.

See `docs/PUBLIC_API_STATUS.md` for the final header table.

### Core API

Primary headers:

```cpp
#include "clc/core/Version.hpp"
#include "clc/core/Time.hpp"
#include "clc/core/EventLog.hpp"
```

Main features:

- `clc::core_version()`;
- `clc::core_version_string()`;
- `clc::GameTime`;
- `ticks_per_second()`, `ticks_per_minute()`, `ticks_per_hour()`, `ticks_per_day()`;
- `seconds_to_ticks(...)`, `minutes_to_ticks(...)`, `hours_to_ticks(...)`, `days_to_ticks(...)`;
- `can_convert_*_to_ticks(...)` helpers;
- `GameTime::can_advance(...)`;
- in-memory `EventLog`.

Time overflow policy:

- conversion helpers saturate at `UINT64_MAX`;
- `can_convert_*_to_ticks(...)` lets callers reject huge values before conversion;
- `GameTime::advance(...)` saturates instead of wrapping.

### Data Registry API

Primary headers:

```cpp
#include "clc/data/Definitions.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"
```

Main definitions:

- `ResourceDefinition`
- `CurrencyDefinition`
- `BuildingDefinition`
- `ProfessionDefinition`
- `SettlementDefinition`

Validation convention:

- most operations return `clc::data::ValidationReport`;
- `report.ok()` means no errors;
- warnings and errors are stored as messages;
- APIs should prefer validation results over silent failure.

`DataPackLoader.hpp` is experimental until schema/data-pack compatibility is frozen.

### Settlement and Storage API

Primary headers:

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

Settlement simulation supports population, food consumption, building inputs/outputs, day advancement, partial tick advancement and deterministic reports.

Arithmetic policy:

- tick scaling and report totals are protected from `uint64_t` wraparound by saturating arithmetic;
- storage operations return validation reports or booleans for insufficient resources/overflow.

### Routes and Caravans API

Primary headers:

```cpp
#include "clc/sim/Routes.hpp"
#include "clc/sim/Caravans.hpp"
```

Routes can be day-based or tick-based:

```cpp
auto route = clc::sim::make_settlement_route_ticks(
    "riverwatch_to_hillford_3h",
    "Riverwatch to Hillford 3h",
    "riverwatch",
    "hillford",
    clc::hours_to_ticks(3)
);
```

Important semantic note:

- `CaravanAdvanceReport::arrived` means “arrived after this advance”; for arrival event semantics use `arrived && ticks_elapsed > 0`.

### Runtime API

Primary headers:

```cpp
#include "clc/sim/SimulationRuntime.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"
```

Recommended runtime path:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
const auto report = clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

`SimulationRuntime` is currently a public mutable data-bag. Prefer workflow helpers where possible because direct mutation can bypass invariants.

### Contracts API

Primary headers:

```cpp
#include "clc/sim/Contracts.hpp"
#include "clc/sim/ContractRewards.hpp"
```

Contracts support day/tick deadlines, statuses, fulfillment from storage/caravans, reward payout and reward ledger entries.

Reward+ledger helpers use staged mutation: contract/cargo/wallet/ledger are committed only after the whole operation succeeds.

### Economy API

Primary headers:

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

Trade is an abstract market model, not a closed conservation economy by default. Market reports include demand-only resources even when supply is zero. Ledger restore validates sequence uniqueness/monotonicity.

### Persistence and diagnostics

Diagnostics/runtime validation headers:

```cpp
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Raw world-state persistence bridge:

```cpp
#include "clc/sim/SimulationPersistence.hpp"
```

`SimulationPersistence.hpp` is powerful and installed, but broader than the default recommended SDK surface. Use it deliberately for save/load bridge internals, world-state export/import, or deterministic replay comparison.

Restore policy:

- restore validates state before mutation;
- runtime restore stages state and commits only after validation/ledger/engine restore succeeds;
- failed restore should not intentionally leave partial runtime mutation.

### Pointer/reference invalidation rules

Many APIs return pointers, references, `std::string_view`, or references/views to internal containers.

General rule:

- treat returned pointers/references/views as invalidated after any mutating operation on the owning object;
- do not store raw pointers returned from registry/catalog/runtime lookup APIs across mutations;
- store IDs instead and look up again after mutation;
- copied reports/snapshots are safe to keep, but live references are not.

### Not the default public entry point

These installed headers are not recommended as the first integration point:

- `clc/data/DataPackLoader.hpp` — experimental until schema compatibility is frozen;
- `clc/economy/Orders.hpp` — experimental until order matching is part of the stable MVP;
- `clc/sim/ScenarioCatalog.hpp` — helper/demo layer;
- `clc/sim/SimulationPersistence.hpp` — raw persistence bridge/internal-risk surface; use deliberately.
