# Settlement Development

Status: **post-v2 helper / source-level C++ API**

`SettlementDevelopment.hpp` adds a planning layer for deeper settlement and production gameplay after the `v2.0.0` SDK foundation. It is not the canonical `v2.1.0` roadmap milestone; `v2.1.0` is tracked as Multiplayer Action Authority in [`detailed-ai-roadmap.md`](detailed-ai-roadmap.md).

The API combines:

- settlement population, storage and buildings;
- production inputs, outputs, worker slots and blocked buildings;
- market supply, demand, price and pressure;
- a planning horizon in ticks.

It produces a `SettlementDevelopmentPlan` that can be used by tools, balancing scripts, server logs or game-specific decision logic.

## Main Entry Point

```cpp
#include "clc/CityLifeCore.hpp"

auto plan = clc::sim::make_settlement_development_plan(
    settlement,
    registry,
    market_report,
    clc::days_to_ticks(3)
);
```

The plan contains:

- `resource_needs`: required resources, estimated daily need, missing amount and market pressure;
- `opportunities`: suggested actions such as assigning workers, importing missing inputs, expanding shortage outputs or registering missing definitions;
- aggregate counts for buildings, active/blocked/unknown buildings, worker slots, critical needs and high-priority opportunities.

## Priorities

Priorities are intentionally simple and stable:

- `critical`: depleted market output, very large missing resource amount or unknown building definition;
- `high`: missing input or shortage pressure;
- `normal`: useful but not urgent work, such as available worker assignment;
- `low`: keep-running informational rows.

Game-specific AI, UI and balancing layers can reinterpret these priorities, but the source-level API keeps deterministic, string-free enum values for integration code.

## Digest Output

Use `settlement_development_plan_digest(...)` for server logs, tooling snapshots and regression-friendly output:

```text
settlement_development;settlement=riverwatch;horizon_ticks=259200;population=120;buildings=2;active=1;blocked=1;unknown=0;worker_slots=7;assigned_workers=3;idle_worker_slots=4;resource_needs=2;critical_needs=1;opportunities=4;high_opportunities=2;top_need=bread:critical;top_action=expand_output
```

The digest is not a save format. It is a compact integration diagnostic.

## Non-Goals

This API does not mutate settlements, assign workers, buy resources, build structures or run AI. It reports the current development pressure so a host game, server or tool can decide what to do next.

It also does not add UI, networking, account systems, matchmaking or engine-specific integration.
