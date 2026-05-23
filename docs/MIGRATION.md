# Migration Guide / Руководство по миграции

Version: **1.0.0**

This guide explains how to update existing City Life Core integrations to the 1.0.0 SDK surface.

Этот документ объясняет, как обновлять существующие интеграции City Life Core до SDK surface 1.0.0.

---

## Who should use this guide

Use this guide if your project used early City Life Core builds and needs to move to the 1.0.0 SDK layout, tick runtime, runtime events, persistence and recommended headers.

New projects can start with:

```cpp
#include "clc/CityLifeCore.hpp"
```

and the documentation:

```text
README.md
docs/PUBLIC_API.md
docs/SDK_STRUCTURE.md
docs/RELEASE_NOTES_1.0.0.md
```

---

## Migration checklist

- [ ] Move to the recommended include `clc/CityLifeCore.hpp` or stable-candidate headers.
- [ ] Check header usage against `docs/PUBLIC_API_STATUS.md`.
- [ ] For real-time/MMO integrations, prefer tick-based APIs over day-only APIs.
- [ ] Check route/caravan/contract day+tick fields.
- [ ] Update runtime event consumers for absolute runtime ticks.
- [ ] Use `buy_resource_with_ledger(...)` and `sell_resource_with_ledger(...)` instead of manual trade+ledger calls.
- [ ] Use reward+ledger helpers for contract rewards.
- [ ] Re-save older save files after loading to materialize explicit time/tick fields.
- [ ] Check pointer/reference lifetime assumptions.
- [ ] Verify external build through `find_package(CityLifeCore CONFIG REQUIRED)`.
- [ ] Validate installed and unpacked SDK ZIP consumers before treating an integration as release-ready.

---

## Includes

Recommended path for most integrations:

```cpp
#include "clc/CityLifeCore.hpp"
```

Selective includes are still supported:

```cpp
#include "clc/core/Time.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/sim/SimulationRuntimeWorkflow.hpp"
#include "clc/economy/Trade.hpp"
```

Use low-level, specialized or experimental headers only when you need that specific surface. Check `docs/PUBLIC_API_STATUS.md` before depending on them.

---

## Time model: days to ticks

The core supports both day-based and tick-based flows.

Base scale:

```cpp
clc::ticks_per_second() == 1;
clc::ticks_per_minute() == 60;
clc::ticks_per_hour()   == 3600;
clc::ticks_per_day()    == 86400;
```

For real-time or server runtime flows, use ticks:

```cpp
auto five_minutes = clc::minutes_to_ticks(5);
auto two_hours = clc::hours_to_ticks(2);
```

Fields to check:

- `SimulationRuntime::time`
- `SettlementRoute::travel_ticks`
- `CaravanState::total_travel_ticks`
- `CaravanState::ticks_remaining`
- `ResourceDeliveryContract::due_ticks`

Day fields remain available for turn/day games and legacy data.

---

## Routes and runtime execution

Day-based route:

```cpp
auto route = clc::sim::make_settlement_route_days(
    "riverwatch_to_hillford",
    "Riverwatch to Hillford",
    "riverwatch",
    "hillford",
    2
);
```

Tick-based route:

```cpp
auto route = clc::sim::make_settlement_route_ticks(
    "riverwatch_to_hillford_3h",
    "Riverwatch to Hillford 3h",
    "riverwatch",
    "hillford",
    clc::hours_to_ticks(3)
);
```

Day-based execution remains available:

```cpp
clc::sim::run_runtime_days(runtime, 2);
```

Recommended tick-based execution:

```cpp
clc::sim::run_runtime_ticks(
    runtime,
    clc::hours_to_ticks(2),
    clc::minutes_to_ticks(30)
);
```

---

## Caravan arrival semantics

`CaravanAdvanceReport::arrived` means the caravan is in arrived state after advance.

For arrived during this advance semantics, use:

```cpp
report.arrived && report.ticks_elapsed > 0
```

This matters if you call `advance_caravan_ticks(...)` or `advance_caravan_day(...)` directly and generate your own events.

---

## Runtime events

Runtime event timestamps use absolute runtime ticks.

Do not interpret event tick as a day number. For day display, use:

```cpp
auto day = event.tick / clc::ticks_per_day();
```

Documented runtime event names:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

Event consumers should ignore unknown event types unless the game requires strict schemas.

---

## Economy and contracts

If your code previously did trade and then wrote the ledger separately, use wrappers:

```cpp
clc::economy::buy_resource_with_ledger(wallet, storage, price, quantity, ledger);
clc::economy::sell_resource_with_ledger(wallet, storage, price, quantity, ledger);
```

Wrappers use staged mutation: wallet/storage/ledger are committed only when the whole operation succeeds.

Recommended contract reward path:

```cpp
clc::sim::fulfill_contract_from_storage_with_reward_and_ledger(
    contracts,
    contract_id,
    delivered_storage,
    wallet,
    ledger
);
```

`clc/sim/ContractRewards.hpp` exists as a public forwarding header. Including either `Contracts.hpp` or `ContractRewards.hpp` is acceptable for reward helpers.

---

## Persistence

1.0.0 save/load behavior:

- saves with explicit runtime `time` restore exact runtime clock;
- legacy saves without `time` derive runtime clock from `current_day`;
- contracts with explicit `due_ticks` restore exact tick deadline;
- legacy contracts without `due_ticks` derive tick deadline from `due_day`;
- failed runtime restore should not intentionally leave partial runtime mutation.

Recommended migration for old saves:

1. Load the old save.
2. Validate runtime state.
3. Re-save with the 1.0.0 SDK.
4. Use the new file going forward.

---

## Pointer/reference invalidation

If your integration stores pointers/references/views returned by registry/catalog/runtime APIs, update it to store IDs instead.

General rule:

- mutating an owner invalidates pointers/references/views into that owner;
- look up again by ID after mutation;
- copied reports/snapshots are safe to keep.

---

## C ABI users

The minimal C ABI is available through:

```c
#include "clc/c/CityLifeCoreC.h"
```

C interface version for 1.0.0 is `3`.

The C ABI covers version/time utilities, an opaque `clc_world` handle, simple tick advancement and read-only world event access. Full runtime integration remains C++ API.

---

## CMake integration

Installed SDK path:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_game PRIVATE CityLifeCore::core)
```

Configure external project:

```bash
cmake -S my_game -B build -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
```

Example consumers:

```text
examples/find_package_consumer/
examples/c_abi_consumer/
```

For release-grade validation, also test the unpacked SDK ZIP package described in `docs/SDK_ZIP_PACKAGE.md`.
