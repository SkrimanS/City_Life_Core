# Migration Guide / Руководство по миграции

Status: **draft for 1.0.0-rc1 / черновик для 1.0.0-rc1**

This guide tracks migration-sensitive changes from the 0.9.x audit series toward the 1.0.0 SDK line.

Этот документ фиксирует migration-sensitive изменения от серии аудита 0.9.x к SDK-линейке 1.0.0.

---

## Русский

### 0.9.x → 1.0.0-rc1: основные темы

Главные области, которые нужно проверить интеграторам:

- public header status;
- tick-based runtime semantics;
- day/tick dual fields;
- runtime event timestamps;
- persistence compatibility;
- examples and recommended integration flow;
- package/install consumer flow.

### Public headers

В 0.9.x все headers внутри `include/clc/` устанавливались и фактически могли использоваться как public surface.

Перед 1.0.0 headers классифицируются в `docs/PUBLIC_API_STATUS.md`:

- `stable-candidate`;
- `experimental`;
- `diagnostics`;
- `legacy`;
- `internal-risk`.

Migration action:

- external projects should depend primarily on stable-candidate runtime-level headers;
- avoid depending on `internal-risk` headers unless you accept pre-1.0 churn;
- treat `experimental` APIs as subject to change.

### Tick runtime

0.9.9 introduced first-class tick runtime:

- `SimulationRuntime::time`;
- `travel_ticks`;
- `ticks_remaining`;
- `due_ticks`;
- `advance_runtime_ticks(...)`;
- `run_runtime_ticks(...)`;
- tick-based arrival helpers.

Migration action:

- for real-time/MMO games, prefer tick-based APIs over day-only APIs;
- for turn/day games, day APIs remain available;
- if both day and tick fields are set, ensure they are equivalent according to the documented tick scale.

### Caravan arrival semantics

`CaravanAdvanceReport::arrived` means the caravan is arrived after the advance call.

A new arrival event should be interpreted as:

```cpp
report.arrived && report.ticks_elapsed > 0
```

Migration action:

- direct users of `advance_caravan_ticks(...)` or `advance_caravan_day(...)` should not treat `arrived == true` alone as a new-arrival event;
- runtime orchestration already filters repeated arrivals by `ticks_elapsed > 0`.

### Runtime event timestamps

0.9.9 moves runtime event timestamps to absolute runtime ticks.

Migration action:

- do not interpret event `tick` as a day number;
- use `ticks_per_day()` or conversion helpers if you need day-level display;
- event replay/backends should store absolute ticks.

### Runtime events

Current event names:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

Migration action:

- update consumers to accept `runtime.tick.completed`;
- update consumers to validate payload schemas once frozen before 1.0.0.

### Persistence

0.9.9 persists runtime time and tick fields.

Compatibility behavior:

- old saves without explicit `time` derive runtime clock from `current_day`;
- old contracts without `due_ticks` derive tick deadline from `due_day`.

Migration action:

- after loading old saves, re-save them with 0.9.9+ to materialize explicit `time` and tick fields;
- treat `.clcs` as compatibility-sensitive before 1.0.0 format policy is frozen.

### Contract rewards header

0.9.x docs referenced `clc/sim/ContractRewards.hpp`. The implementation existed, and declarations were in `Contracts.hpp`.

Migration action:

- `ContractRewards.hpp` now exists as a forwarding public header;
- including either `Contracts.hpp` or `ContractRewards.hpp` is acceptable for reward+ledger helpers.

### Recommended integration path

For new integrations, prefer:

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Avoid wiring all low-level subsystem catalogs manually unless you are building tools or tests.

---

## English

### 0.9.x → 1.0.0-rc1: main topics

Integrator-sensitive areas:

- public header status;
- tick-based runtime semantics;
- day/tick dual fields;
- runtime event timestamps;
- persistence compatibility;
- examples and recommended integration flow;
- package/install consumer flow.

### Public headers

In 0.9.x, all headers under `include/clc/` were installed and could effectively be used as public surface.

Before 1.0.0, headers are classified in `docs/PUBLIC_API_STATUS.md`:

- `stable-candidate`;
- `experimental`;
- `diagnostics`;
- `legacy`;
- `internal-risk`.

Migration action:

- external projects should depend primarily on stable-candidate runtime-level headers;
- avoid depending on `internal-risk` headers unless you accept pre-1.0 churn;
- treat `experimental` APIs as subject to change.

### Tick runtime

0.9.9 introduced first-class tick runtime:

- `SimulationRuntime::time`;
- `travel_ticks`;
- `ticks_remaining`;
- `due_ticks`;
- `advance_runtime_ticks(...)`;
- `run_runtime_ticks(...)`;
- tick-based arrival helpers.

Migration action:

- for real-time/MMO games, prefer tick-based APIs over day-only APIs;
- for turn/day games, day APIs remain available;
- if both day and tick fields are set, ensure they are equivalent according to the documented tick scale.

### Caravan arrival semantics

`CaravanAdvanceReport::arrived` means the caravan is arrived after the advance call.

A new arrival event should be interpreted as:

```cpp
report.arrived && report.ticks_elapsed > 0
```

Migration action:

- direct users of `advance_caravan_ticks(...)` or `advance_caravan_day(...)` should not treat `arrived == true` alone as a new-arrival event;
- runtime orchestration already filters repeated arrivals by `ticks_elapsed > 0`.

### Runtime event timestamps

0.9.9 moves runtime event timestamps to absolute runtime ticks.

Migration action:

- do not interpret event `tick` as a day number;
- use `ticks_per_day()` or conversion helpers if you need day-level display;
- event replay/backends should store absolute ticks.

### Runtime events

Current event names:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

Migration action:

- update consumers to accept `runtime.tick.completed`;
- update consumers to validate payload schemas once frozen before 1.0.0.

### Persistence

0.9.9 persists runtime time and tick fields.

Compatibility behavior:

- old saves without explicit `time` derive runtime clock from `current_day`;
- old contracts without `due_ticks` derive tick deadline from `due_day`.

Migration action:

- after loading old saves, re-save them with 0.9.9+ to materialize explicit `time` and tick fields;
- treat `.clcs` as compatibility-sensitive before 1.0.0 format policy is frozen.

### Contract rewards header

0.9.x docs referenced `clc/sim/ContractRewards.hpp`. The implementation existed, and declarations were in `Contracts.hpp`.

Migration action:

- `ContractRewards.hpp` now exists as a forwarding public header;
- including either `Contracts.hpp` or `ContractRewards.hpp` is acceptable for reward+ledger helpers.

### Recommended integration path

For new integrations, prefer runtime-level headers:

```cpp
#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"
#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
```

Avoid wiring every low-level subsystem catalog manually unless you are building tools or tests.
