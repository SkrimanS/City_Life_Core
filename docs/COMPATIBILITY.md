# Compatibility Policy / Политика совместимости

Status: **draft for 1.0.0-rc1 / черновик для 1.0.0-rc1**

This document defines what City Life Core intends to keep compatible after the 1.0.0 SDK release.

Этот документ фиксирует, что City Life Core планирует сохранять совместимым после SDK-релиза 1.0.0.

---

## Русский

### Области совместимости

После 1.0.0 compatibility policy покрывает:

- stable public C++ headers;
- stable public C++ types/functions/fields;
- CMake package target `CityLifeCore::core`;
- tick/time model;
- documented runtime event names and payload schemas;
- documented save/load compatibility behavior;
- documented data-pack schema behavior.

Не покрывает, пока не будет явно объявлено stable:

- C ABI;
- binary ABI;
- internal implementation details;
- exact memory layout of C++ structs across compilers;
- experimental headers and helpers;
- benchmark thresholds;
- unfinished network/server protocols.

### Public header policy

All headers under `include/clc/` are installed. However, installed does not automatically mean stable.

Each public header must be classified in `docs/PUBLIC_API_STATUS.md` as one of:

- `stable` — compatibility protected after 1.0.0;
- `experimental` — public but may change;
- `diagnostics` — public tooling/observability API, compatibility protected only where documented;
- `legacy` — kept for compatibility, not recommended for new code;
- `internal-risk` — currently installed but should not be treated as stable until reviewed.

### Tick/time compatibility

The tick model must be frozen before 1.0.0:

- `ticks_per_second()`;
- `ticks_per_minute()`;
- `ticks_per_hour()`;
- `ticks_per_day()`;
- `seconds_to_ticks(...)`;
- `minutes_to_ticks(...)`;
- `hours_to_ticks(...)`;
- `days_to_ticks(...)`;
- `SimulationRuntime::time` semantics;
- `SettlementRoute::travel_ticks` semantics;
- `CaravanState::total_travel_ticks` and `ticks_remaining` semantics;
- `ResourceDeliveryContract::due_ticks` semantics.

Changing this scale after 1.0.0 is a breaking change.

### Day/tick dual fields

Some structures keep both day and tick fields for compatibility:

- route travel days/ticks;
- caravan travel days/ticks;
- contract due day/ticks.

Policy:

- tick fields are the preferred real-time representation;
- day fields are retained for compatibility and daily/turn-based games;
- when both are set, they must be equivalent according to the documented tick scale;
- old saves without tick fields may derive tick fields from day fields.

### Runtime event compatibility

Documented runtime event names:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

Policy:

- event timestamps are absolute runtime ticks;
- event names are stable after 1.0.0 unless a major version changes them;
- payload schemas must be documented before 1.0.0;
- new event names can be added in minor releases;
- changing or removing existing stable event names/payload schemas is breaking.

### Save/load compatibility

Current 0.9.9 compatibility behavior:

- saves with explicit runtime `time` restore exact runtime clock;
- legacy saves without `time` synchronize runtime clock from saved `current_day`;
- contracts with explicit `due_ticks` restore exact tick deadline;
- legacy contracts without `due_ticks` derive tick deadline from `due_day`;
- corrupted save loads must fail validation and must not intentionally mutate target runtime.

Before 1.0.0, the `.clcs` format needs a documented format/version policy. Until then, it is stable enough for audit but not final protocol documentation.

### Pointer/reference invalidation

Many APIs return pointers, references, views, or collections derived from internal containers.

General rule for SDK users:

- treat returned pointers/references/views as invalidated after any mutating operation on the owning object;
- do not store raw pointers returned from catalog/runtime lookup APIs across mutations;
- prefer IDs and lookup again after mutation.

This rule must be repeated in public API docs before 1.0.0.

---

## English

### Compatibility areas

After 1.0.0, compatibility policy covers:

- stable public C++ headers;
- stable public C++ types/functions/fields;
- CMake package target `CityLifeCore::core`;
- tick/time model;
- documented runtime event names and payload schemas;
- documented save/load compatibility behavior;
- documented data-pack schema behavior.

Not covered until explicitly declared stable:

- C ABI;
- binary ABI;
- internal implementation details;
- exact memory layout of C++ structs across compilers;
- experimental headers and helpers;
- benchmark thresholds;
- unfinished network/server protocols.

### Public header policy

All headers under `include/clc/` are installed. Installed does not automatically mean stable.

Each public header must be classified in `docs/PUBLIC_API_STATUS.md` as one of:

- `stable` — compatibility protected after 1.0.0;
- `experimental` — public but may change;
- `diagnostics` — public tooling/observability API, compatibility protected only where documented;
- `legacy` — kept for compatibility, not recommended for new code;
- `internal-risk` — currently installed but should not be treated as stable until reviewed.

### Tick/time compatibility

The tick model must be frozen before 1.0.0:

- `ticks_per_second()`;
- `ticks_per_minute()`;
- `ticks_per_hour()`;
- `ticks_per_day()`;
- `seconds_to_ticks(...)`;
- `minutes_to_ticks(...)`;
- `hours_to_ticks(...)`;
- `days_to_ticks(...)`;
- `SimulationRuntime::time` semantics;
- `SettlementRoute::travel_ticks` semantics;
- `CaravanState::total_travel_ticks` and `ticks_remaining` semantics;
- `ResourceDeliveryContract::due_ticks` semantics.

Changing this scale after 1.0.0 is a breaking change.

### Day/tick dual fields

Some structures keep both day and tick fields for compatibility:

- route travel days/ticks;
- caravan travel days/ticks;
- contract due day/ticks.

Policy:

- tick fields are the preferred real-time representation;
- day fields are retained for compatibility and daily/turn-based games;
- when both are set, they must be equivalent according to the documented tick scale;
- old saves without tick fields may derive tick fields from day fields.

### Runtime event compatibility

Documented runtime event names:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

Policy:

- event timestamps are absolute runtime ticks;
- event names are stable after 1.0.0 unless a major version changes them;
- payload schemas must be documented before 1.0.0;
- new event names can be added in minor releases;
- changing or removing existing stable event names/payload schemas is breaking.

### Save/load compatibility

Current 0.9.9 compatibility behavior:

- saves with explicit runtime `time` restore exact runtime clock;
- legacy saves without `time` synchronize runtime clock from saved `current_day`;
- contracts with explicit `due_ticks` restore exact tick deadline;
- legacy contracts without `due_ticks` derive tick deadline from `due_day`;
- corrupted save loads must fail validation and must not intentionally mutate target runtime.

Before 1.0.0, the `.clcs` format needs a documented format/version policy. Until then, it is stable enough for audit but not final protocol documentation.

### Pointer/reference invalidation

Many APIs return pointers, references, views, or collections derived from internal containers.

General rule for SDK users:

- treat returned pointers/references/views as invalidated after any mutating operation on the owning object;
- do not store raw pointers returned from catalog/runtime lookup APIs across mutations;
- prefer IDs and lookup again after mutation.
