# Core Concepts / Основные концепции ядра

Version: **1.0.0**

This document explains the main City Life Core systems and how external C++ projects should interact with them.

Этот документ объясняет основные системы City Life Core и то, как внешние C++ проекты должны с ними взаимодействовать.

---

## 1. What City Life Core is

City Life Core is a headless C++20 simulation SDK. It does not render UI and does not own the game loop. A game, server, editor or tool calls the SDK to mutate and validate simulation state.

The recommended include is:

```cpp
#include "clc/CityLifeCore.hpp"
```

Main layers:

| Layer | Purpose |
| --- | --- |
| Core | Version, IDs, result helpers, time and event log. |
| Data | Definitions, registry and validation. |
| Simulation | Settlements, storage, routes, caravans, factions, ownership, contracts and runtime workflows. |
| Economy | Market prices, wallet, trade, ledger and orders. |
| Persistence / Diagnostics | Save/load, world-state restore, validation and replay checks. |

---

## 2. Data registry

The registry stores static game definitions. Runtime state references these definitions by id.

```cpp
clc::data::DataRegistry registry;

registry.add(clc::data::ResourceDefinition{
    .id = "grain",
    .display_name = "Grain",
    .category = "food",
    .base_value = 10,
});

registry.add(clc::data::SettlementDefinition{
    .id = "riverwatch",
    .display_name = "Riverwatch",
    .starting_population = 100,
});

const auto report = registry.validate_references();
```

Use definitions for static data and runtime objects for changing simulation state.

---

## 3. Validation model

Most mutating operations return `clc::data::ValidationReport` or a result object containing validation.

Rules:

- `report.ok()` means no errors;
- warnings do not make `ok()` false;
- errors make `ok()` false;
- inspect `report.messages()` for details.

```cpp
auto report = runtime.engine.add_resource_to_settlement("riverwatch", "grain", 50);
if (!report.ok()) {
    for (const auto& message : report.messages()) {
        // message.path, message.message, message.severity
    }
}
```

---

## 4. Time model

City Life Core supports both day-based and tick-based simulation.

```cpp
clc::ticks_per_second() == 1;
clc::ticks_per_minute() == 60;
clc::ticks_per_hour()   == 3600;
clc::ticks_per_day()    == 86400;
```

Use ticks for real-time, MMO-like, backend and live delivery systems:

```cpp
auto five_minutes = clc::minutes_to_ticks(5);
auto two_hours = clc::hours_to_ticks(2);
```

Use day APIs for daily or turn-based games.

Important runtime fields:

- `SimulationRuntime::time`
- `SettlementRoute::travel_ticks`
- `CaravanState::total_travel_ticks`
- `CaravanState::ticks_remaining`
- `ResourceDeliveryContract::due_ticks`

---

## 5. Runtime state

`clc::sim::SimulationRuntime` is the main state container. It groups:

- `engine` — settlements, storage, daily simulation and market reports;
- `time` — absolute runtime tick clock;
- `routes` — settlement routes;
- `caravans` — caravan states;
- `factions` — factions and reputation;
- `ownership` — settlement/caravan ownership;
- `contracts` — delivery contracts;
- `wallet` — runtime wallet;
- `ledger` — economy ledger.

Quick start:

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
```

Prefer workflow helpers for mutation. Direct mutation is possible but can bypass invariants.

---

## 6. Settlements and storage

`ResourceStorage` stores resources by id.

```cpp
clc::sim::ResourceStorage storage;
storage.add("grain", 100);
storage.try_remove("grain", 25);
auto amount = storage.amount("grain");
```

Settlement simulation supports:

- population;
- food consumption;
- buildings;
- worker slots;
- input resources;
- output resources;
- daily advancement;
- partial tick advancement;
- deterministic tick remainders;
- deterministic reports.

---

## 7. Routes and caravans

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

Caravan travel:

```cpp
auto caravan = clc::sim::create_caravan_for_route(route, "caravan_01", "Caravan 01");
clc::sim::advance_caravan_ticks(caravan, clc::minutes_to_ticks(30));
```

Arrival event semantics:

```cpp
report.arrived && report.ticks_elapsed > 0
```

---

## 8. Runtime workflows

Workflow helpers validate and mutate runtime state in recommended ways.

Common operations:

```cpp
clc::sim::create_runtime_settlement(runtime, "riverwatch");
clc::sim::add_runtime_route(runtime, route);
clc::sim::create_runtime_caravan_for_route(runtime, route_id, caravan_id, display_name);
clc::sim::load_runtime_caravan_at_origin(runtime, caravan_id, "grain", 50);
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(30));
clc::sim::deliver_runtime_arrived_caravan_cargo_to_destination(runtime, caravan_id);
```

Use workflows for gameplay/server logic. Use direct catalog mutation mainly for tools, tests or custom integration layers.

---

## 9. Runtime execution

Advance once:

```cpp
auto report = clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

Run with fixed steps:

```cpp
auto result = clc::sim::run_runtime_ticks(
    runtime,
    clc::hours_to_ticks(2),
    clc::minutes_to_ticks(30)
);
```

Day wrappers remain available:

```cpp
clc::sim::run_runtime_days(runtime, 3);
```

---

## 10. Economy and ledger

Market reports provide prices for registered supplied resources and registered demand-only resources.

```cpp
auto report = clc::economy::make_market_report(registry, storage, market);
const auto* grain_price = clc::economy::market_price_by_resource(report, "grain");
auto fallback_price = clc::economy::market_price_or(report, "missing", 10);
```

Recommended trade path:

```cpp
clc::economy::buy_resource_with_ledger(wallet, storage, price, quantity, ledger);
clc::economy::sell_resource_with_ledger(wallet, storage, price, quantity, ledger);
```

Trade model:

- buy spends coins and adds resource to local storage;
- sell removes resource from local storage and credits coins;
- ledger records the operation;
- the model is abstract by default, not a closed conservation economy.

---

## 11. Contracts and rewards

Contracts describe delivery goals:

- issuer faction;
- receiver faction;
- resource id;
- quantity;
- reward coins;
- due day/ticks;
- status.

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

Reward+ledger helpers commit contract/cargo/wallet/ledger only when the whole operation succeeds.

---

## 12. Factions and ownership

Factions and ownership can be used to:

- assign settlements to factions;
- assign caravans to factions;
- check whether a faction is allowed to fulfill a contract;
- store reputation values;
- query faction-owned entities.

Use ownership helpers instead of editing ownership vectors manually when possible.

---

## 13. Events and diagnostics

Runtime event APIs are useful for logs, replay diagnostics, backend validation and UI adapters.

Documented runtime event names:

- `runtime.day.completed`
- `runtime.tick.completed`
- `runtime.caravan.progress`
- `runtime.caravan.arrived`
- `runtime.caravan.cargo_delivered`
- `runtime.contract.fulfilled`
- `runtime.contract.failed`

Runtime event timestamps use absolute runtime ticks.

---

## 14. Persistence

Persistence APIs can capture, serialize, deserialize and restore simulation world state.

Use persistence for:

- save/load;
- runtime snapshots;
- replay checks;
- migration tools;
- backend/debug comparison.

Roundtrip validation:

```cpp
auto result = clc::sim::validate_simulation_runtime_save_load_roundtrip(
    runtime,
    loaded_runtime,
    path
);
```

Games that expose saves as a player-facing format should wrap core persistence in their own game-level save versioning policy.

---

## 15. Pointer/reference lifetime

Many APIs expose pointers, references, views or references to containers.

Safe rule:

- store IDs, not raw pointers;
- after mutating a registry/catalog/runtime/storage object, look up again by ID;
- copied reports and snapshots are safe to keep;
- live references/views into containers are not stable across mutations.

---

## 16. Common integration patterns

### Server or backend runtime

1. Load definitions into `DataRegistry`.
2. Create `SimulationRuntime`.
3. Create settlements, routes, factions and contracts.
4. Apply game actions through workflow helpers.
5. Advance runtime by ticks.
6. Emit reports/events.
7. Save runtime state.

### Caravan/trader system

1. Create tick-based routes.
2. Create caravans for routes.
3. Load cargo at origin.
4. Advance runtime by ticks.
5. Detect arrivals.
6. Deliver cargo and fulfill contracts.

### Economy simulation

1. Create resources and settlements.
2. Configure market demand.
3. Generate market reports.
4. Use trade+ledger wrappers.
5. Use contract reward+ledger helpers.
6. Persist ledger with runtime state.

### Editor/tool integration

1. Build or edit definitions.
2. Validate registry references.
3. Create sample runtime.
4. Run short simulations.
5. Inspect reports and event logs.
6. Save/export data or snapshots.

---

## 17. Related documents

- `README.md` — overview and examples.
- `docs/PUBLIC_API.md` — API surface and usage rules.
- `docs/PUBLIC_API_STATUS.md` — header usage classification.
- `docs/SDK_STRUCTURE.md` — repository/install layout and CMake integration.
- `docs/COMPATIBILITY.md` — compatibility rules.
- `docs/MIGRATION.md` — migration instructions.
- `docs/PACKAGING.md` — install and external consumer flow.
- `docs/RELEASE_NOTES_1.0.0.md` — current release notes.
