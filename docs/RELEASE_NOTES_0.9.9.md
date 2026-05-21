# City Life Core 0.9.9 Release Notes / Примечания к релизу

Status: **pre-1.0 audit build / сборка для аудита перед 1.0**

`0.9.9` is the real-time runtime and persistence hardening build before the final 1.0 audit. It keeps the existing day-based APIs while adding first-class tick-based runtime flows for games where caravans, traders, contracts, and world events may resolve after seconds, minutes, or hours.

`0.9.9` — это сборка укрепления real-time runtime и persistence перед финальным аудитом 1.0. Она сохраняет существующие day-based API и добавляет полноценные tick-based runtime flows для игр, где караваны, торговцы, контракты и события мира могут завершаться через секунды, минуты или часы.

---

## Highlights / Главное

- Runtime now persists `SimulationRuntime::time`.
- World state now stores explicit runtime `time` rows.
- Routes and caravans support tick-based travel.
- Contracts support `due_ticks` deadlines.
- Runtime can advance by arbitrary ticks without daily engine advancement.
- High-level tick-run helpers can wait for caravan arrival and fulfill contracts.
- Event logs now use absolute runtime tick timestamps instead of day numbers.
- Tick-run diagnostics are available through `runtime.tick.completed` events.
- Cargo delivery event helpers can timestamp from `SimulationRuntime::time` directly.
- Legacy saves remain compatible when `time` or `due_ticks` are missing.

---

## New runtime APIs

### Tick advancement

```cpp
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(10));
```

### Tick run

```cpp
clc::sim::run_runtime_ticks(
    runtime,
    clc::hours_to_ticks(2),
    clc::minutes_to_ticks(30)
);
```

### Wait for arrival by ticks

```cpp
clc::sim::run_runtime_until_first_caravan_arrival_by_ticks(
    runtime,
    clc::hours_to_ticks(6),
    clc::minutes_to_ticks(15)
);
```

### Wait for arrival and fulfill a contract by ticks

```cpp
clc::sim::run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(
    runtime,
    clc::hours_to_ticks(6),
    clc::minutes_to_ticks(15),
    "riverwatch"
);
```

---

## Persistence changes

`0.9.9` persists:

- runtime clock;
- route/caravan tick travel state;
- contract tick deadlines;
- settlement partial tick remainders;
- runtime event/replay relevant state.

Compatibility behavior:

- older saves without explicit `time` synchronize runtime clock from `current_day`;
- older contract rows without `due_ticks` derive `due_ticks` from `due_day`.

---

## Diagnostics changes

Runtime events now use absolute runtime tick timestamps.

Supported event types:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

New helpers:

- `append_runtime_tick_report_events(...)`
- `append_runtime_tick_run_events(...)`
- `append_runtime_tick_arrival_contract_events(...)`

---

## Notes for auditors / Заметки для аудита

Important areas to verify before 1.0:

- full CMake configure/build/test on target platforms;
- no accidental day-only assumptions in public runtime APIs;
- persistence compatibility for existing saves;
- deterministic replay after midpoint save/load;
- event-log timestamp semantics;
- C++ public API naming before freezing for 1.0;
- package install/export behavior.

Known not-final areas:

- C ABI is still not implemented;
- binary package artifacts are not finalized;
- final 1.0 public naming freeze is pending audit.
