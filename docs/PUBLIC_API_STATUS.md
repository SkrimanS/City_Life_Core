# Public API Status / Статус публичного API

Version: **0.9.9**

This document classifies the installed City Life Core headers by intended SDK usage.

Этот документ классифицирует установленные headers City Life Core по назначению для SDK users.

---

## Status legend / Легенда

| Status | Meaning |
| --- | --- |
| `recommended` | Preferred SDK surface for most integrations. |
| `stable-candidate` | Public SDK surface intended for direct use. |
| `diagnostics` | Public observability, validation, replay or tooling API. |
| `experimental` | Public API that may change more easily. |
| `legacy` | Kept for compatibility; not preferred for new code. |
| `specialized` | Public but low-level or broad; use deliberately. |

Recommended first include:

```cpp
#include "clc/CityLifeCore.hpp"
```

---

## Header table / Таблица headers

| Header | Status | Intended users | Recommendation |
| --- | --- | --- | --- |
| `clc/CityLifeCore.hpp` | `recommended` | all SDK users | Start here for most game/server integrations. |
| `clc/core/Version.hpp` | `stable-candidate` | all SDK users | Use for version checks and diagnostics. |
| `clc/core/Ids.hpp` | `stable-candidate` | SDK users needing typed IDs | Use when integrating ID-heavy systems. |
| `clc/core/Result.hpp` | `stable-candidate` | low-level SDK users | Use for result/error-style helpers. |
| `clc/core/Time.hpp` | `stable-candidate` | runtime/game/server code | Use for ticks, runtime clocks and conversions. |
| `clc/core/EventLog.hpp` | `stable-candidate` | diagnostics/runtime users | Use for in-memory event logs. |
| `clc/core/World.hpp` | `legacy` | simple bootstrap users | Prefer runtime APIs for full simulation integration. |
| `clc/data/Definitions.hpp` | `stable-candidate` | data/model authors | Use to define resources, settlements, buildings and professions. |
| `clc/data/Validation.hpp` | `stable-candidate` | all SDK users | Use to inspect validation errors and warnings. |
| `clc/data/DataRegistry.hpp` | `stable-candidate` | data/runtime users | Use to store and validate definitions. |
| `clc/data/DataPackLoader.hpp` | `experimental` | tool/data-pack users | Use when loading `.clcd` data packs directly. |
| `clc/economy/Market.hpp` | `stable-candidate` | economy users | Use for market prices and demand/supply reporting. |
| `clc/economy/Trade.hpp` | `stable-candidate` | economy users | Prefer trade+ledger wrappers for game flows. |
| `clc/economy/Ledger.hpp` | `stable-candidate` | economy/runtime users | Use to record buy/sell/contract reward history. |
| `clc/economy/Orders.hpp` | `experimental` | market/order users | Use only if you need the order layer. |
| `clc/sim/Storage.hpp` | `stable-candidate` | simulation/runtime users | Use for resource storage and transfer operations. |
| `clc/sim/Settlement.hpp` | `stable-candidate` | simulation/runtime users | Use for settlement state, ticks, reports and buildings. |
| `clc/sim/SimulationEngine.hpp` | `stable-candidate` | advanced simulation users | Use when you need engine-level day simulation. |
| `clc/sim/ScenarioCatalog.hpp` | `experimental` | test/demo/scenario users | Prefer explicit registries for production integrations. |
| `clc/sim/Routes.hpp` | `stable-candidate` | runtime/world users | Use for day/tick settlement routes. |
| `clc/sim/Caravans.hpp` | `stable-candidate` | runtime/world users | Use for caravan state, cargo and travel progress. |
| `clc/sim/Factions.hpp` | `stable-candidate` | runtime/world users | Use for factions and reputation. |
| `clc/sim/Ownership.hpp` | `stable-candidate` | runtime/world users | Use for settlement/caravan ownership. |
| `clc/sim/Contracts.hpp` | `stable-candidate` | runtime/world users | Use for delivery contracts and deadlines. |
| `clc/sim/ContractRewards.hpp` | `stable-candidate` | runtime/economy users | Include reward+ledger helpers explicitly if desired. |
| `clc/sim/SimulationRuntime.hpp` | `stable-candidate` | primary runtime integrators | Use as the runtime state container. Prefer workflow helpers for mutation. |
| `clc/sim/SimulationRuntimeScenario.hpp` | `stable-candidate` | SDK examples/quickstart | Use for quick bootstrap scenarios and examples. |
| `clc/sim/SimulationRuntimeWorkflow.hpp` | `stable-candidate` | primary runtime integrators | Use for invariant-preserving runtime operations. |
| `clc/sim/SimulationRuntimeTick.hpp` | `stable-candidate` | real-time/MMO runtime users | Use for tick/day runtime advancement. |
| `clc/sim/SimulationRuntimeEvents.hpp` | `diagnostics` | diagnostics/replay/backend users | Use for runtime event append, analysis and validation. |
| `clc/sim/SimulationPersistence.hpp` | `specialized` | persistence/tooling users | Use deliberately for raw world-state save/load and bridge APIs. |
| `clc/sim/SimulationRuntimePersistenceValidation.hpp` | `diagnostics` | tests/backend/tooling users | Use for save/load roundtrip and replay validation. |

---

## Recommended integration layers

### Most games and servers

Use:

```cpp
#include "clc/CityLifeCore.hpp"
```

and start from runtime workflows:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

### Data/model tooling

Use:

```cpp
#include "clc/data/Definitions.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"
```

### Economy integrations

Use:

```cpp
#include "clc/economy/Market.hpp"
#include "clc/economy/Trade.hpp"
#include "clc/economy/Ledger.hpp"
```

Prefer:

```cpp
clc::economy::buy_resource_with_ledger(wallet, storage, price, quantity, ledger);
clc::economy::sell_resource_with_ledger(wallet, storage, price, quantity, ledger);
```

### Save/load and replay tooling

Use:

```cpp
#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Raw persistence APIs are intentionally powerful. Treat them as specialized integration APIs, not the first API surface for gameplay code.

---

## Usage rules for SDK users

- Prefer `clc/CityLifeCore.hpp` for general integration.
- Prefer runtime workflow helpers over direct mutation of runtime fields.
- Store IDs instead of long-lived pointers/references into registries, catalogs or runtime containers.
- Use tick-based APIs for real-time, MMO and server-authoritative systems.
- Use day-based APIs for daily/turn-based games.
- Use diagnostics APIs for validation, replay, tooling and backend checks.
- Use experimental APIs only when you accept easier future changes.
