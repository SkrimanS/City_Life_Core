# Compatibility Policy / Политика совместимости

Version: **1.0.0**

This document explains what City Life Core treats as compatibility-sensitive API and behavior for the 1.0.0 source-first SDK.

Этот документ объясняет, какие части API и поведения City Life Core считаются важными для совместимости в source-first SDK 1.0.0.

---

## What compatibility covers

Compatibility policy covers:

- documented public C++ headers;
- documented public C++ types/functions/fields;
- CMake package target `CityLifeCore::core`;
- installed CMake package variables;
- tick/time model;
- documented runtime event names and payload conventions;
- documented save/load behavior;
- documented data-pack behavior;
- documented validation behavior;
- documented minimal C ABI v3 behavior.

---

## What is not stable API

Do not treat these as stable API unless explicitly documented:

- C++ binary ABI;
- internal implementation details;
- exact memory layout of C++ structs across compilers;
- private helper functions inside `.cpp` files;
- benchmark timings;
- unfinished network/server protocols;
- third-party package-manager recipes not published by the official project.

---

## Public header classes

Headers under `include/clc/` are installed. Their intended use is documented in:

```text
docs/PUBLIC_API_STATUS.md
```

Header classes:

- `recommended` — preferred SDK surface for most integrations;
- `stable-candidate` — recommended public SDK surface;
- `experimental` — public but may change more easily;
- `diagnostics` — public observability/testing/replay/validation APIs;
- `legacy` — kept for compatibility, not preferred for new code;
- `specialized` — public but low-level or broad; use deliberately;
- `c-abi-minimal` — documented minimal C ABI surface.

Recommended first include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Recommended C ABI include:

```c
#include "clc/c/CityLifeCoreC.h"
```

---

## Tick/time compatibility

City Life Core uses one tick scale:

```cpp
ticks_per_second() == 1
ticks_per_minute() == 60
ticks_per_hour()   == 3600
ticks_per_day()    == 86400
```

The following API is compatibility-sensitive:

- `clc::GameTime`;
- `ticks_per_second()`;
- `ticks_per_minute()`;
- `ticks_per_hour()`;
- `ticks_per_day()`;
- `seconds_to_ticks(...)`;
- `minutes_to_ticks(...)`;
- `hours_to_ticks(...)`;
- `days_to_ticks(...)`;
- `can_convert_seconds_to_ticks(...)`;
- `can_convert_minutes_to_ticks(...)`;
- `can_convert_hours_to_ticks(...)`;
- `can_convert_days_to_ticks(...)`;
- `GameTime::can_advance(...)`;
- `SimulationRuntime::time`;
- `SettlementRoute::travel_ticks`;
- `CaravanState::total_travel_ticks`;
- `CaravanState::ticks_remaining`;
- `ResourceDeliveryContract::due_ticks`.

Overflow policy:

- conversion helpers use saturating arithmetic and return `UINT64_MAX` on overflow;
- `can_convert_*_to_ticks(...)` helpers allow callers to reject overflowing values before conversion;
- `GameTime::advance(...)` saturates at `UINT64_MAX` instead of wrapping;
- `GameTime::can_advance(...)` lets callers detect whether an advance would overflow.

---

## Day/tick dual fields

Some structures keep both day and tick fields:

- route travel days/ticks;
- caravan travel days/ticks;
- contract due day/ticks.

Policy:

- tick fields are the preferred real-time representation;
- day fields are retained for daily/turn-based games and legacy data;
- when both are set, they should represent the same duration according to the documented tick scale;
- loaders may derive missing tick fields from day fields for legacy saves.

---

## Runtime event compatibility

Documented runtime event names:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

Event policy:

- runtime event timestamps use absolute runtime ticks;
- new event names may be added;
- existing documented event names should not change without a compatibility note;
- event consumers should ignore unknown event types unless their game requires strict schemas.

---

## Save/load compatibility

Documented save/load behavior:

- saves with explicit runtime `time` restore exact runtime clock;
- legacy saves without `time` synchronize runtime clock from saved `current_day`;
- contracts with explicit `due_ticks` restore exact tick deadline;
- legacy contracts without `due_ticks` derive tick deadline from `due_day`;
- corrupted save loads fail validation;
- failed runtime restore should not intentionally leave partial runtime mutation.

Save/load files are intended for City Life Core runtime persistence and diagnostics. Games that expose save files as long-term public protocols should version their own game-level save format around the core data they use.

---

## Minimal C ABI compatibility

The documented 1.0.0 C ABI surface has C interface version `3` and covers:

- core version;
- C interface version;
- tick constants and conversions;
- opaque `clc_world` create/destroy;
- world name/seed/current tick/event count;
- tick advancement;
- read-only world event id/tick/type/payload accessors.

C ABI v3 does not expose full runtime, registries, containers, save/load, callbacks, caravans, contracts or economy workflows.

Future C interface expansion should preserve existing functions where possible and increase the C interface version when the C-facing surface changes meaningfully.

---

## Validation compatibility

Most mutating SDK operations report errors through:

```cpp
clc::data::ValidationReport
```

General convention:

- `report.ok()` means no errors;
- warnings do not make `ok()` false;
- errors make `ok()` false;
- failed operations should avoid intentional partial mutation when the API documents transactional behavior.

---

## Pointer/reference invalidation

Many APIs return pointers, references, views, or collections derived from internal containers.

General rule for SDK users:

- treat returned pointers/references/views as invalidated after any mutating operation on the owning object;
- do not store raw pointers returned from catalog/runtime lookup APIs across mutations;
- store IDs and look up again after mutation;
- copied reports/snapshots can be stored independently.

---

## Source vs binary compatibility

The 1.0.0 public SDK is source-first C++.

Source compatibility means external projects can include headers and link against `CityLifeCore::core` when built with a compatible C++20 toolchain.

C++ binary ABI compatibility is not promised. Projects that need ABI boundaries should isolate City Life Core behind their own application/plugin ABI or use the documented minimal C ABI where it is sufficient.

See also:

```text
docs/BUILD_AND_LINKING_POLICY.md
docs/C_ABI.md
docs/VERSIONING.md
docs/RELEASE_NOTES_1.0.0.md
```
