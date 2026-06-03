# Persistence, replay and migration

Status: v1.8.0 foundation.

This document describes the v1.8.0 helpers for save-format review, supported world-state migration, replay diagnostics and checkpoint guidance for long simulations.

The API lives in:

```cpp
#include <clc/sim/SimulationPersistenceReplay.hpp>
```

It is also included by the umbrella header:

```cpp
#include <clc/CityLifeCore.hpp>
```

## Save Format Manifest

`current_simulation_save_format_manifest` describes the current source-level world-state format:

- format id: `CLC_SIM_WORLD_STATE`;
- version: `1`;
- explicit tick time row;
- explicit tick travel rows for routes/caravans;
- explicit contract due ticks;
- ledger entries.

Use `simulation_save_format_manifest_digest` in release review, test output or tool logs.

## Save Review And Migration

`review_simulation_world_state_save_format` classifies save content as:

- `current`;
- `migratable_legacy`;
- `unsupported`.

The v1.8.0 migration helper supports older version-1 shapes that are already accepted by the deserializer:

- missing explicit `time` row;
- route rows without explicit travel ticks;
- caravan rows without explicit travel ticks;
- contract rows without explicit due ticks.

`migrate_simulation_world_state_content` inserts a missing `time` row by deriving ticks from the saved day. Legacy day-based route, caravan and contract rows remain loadable through the existing deserializer, and the migration report records that legacy row groups were detected.

This helper does not migrate arbitrary product saves, unknown schema versions, external databases or game-specific metadata.

## Replay Diagnostics

`compare_simulation_replay_diagnostics` wraps runtime comparison with extra replay-oriented fields:

- expected and actual current tick;
- expected and actual current day;
- serialized world-state line counts;
- serialized line mismatch count;
- optional event-log checksum comparison;
- first mismatch category.

The goal is to make replay failures easier to triage than a single boolean mismatch.

## Checkpoint Plans

`make_simulation_checkpoint_plan` gives tools and servers a lightweight schedule hint:

- current runtime tick;
- checkpoint interval;
- next checkpoint tick;
- event count;
- checkpoints due so far;
- whether a checkpoint is due now.

The core does not store checkpoints, rotate files or manage databases. Hosts decide where and how checkpoints are written.

## Long-Running Simulations

Recommended host workflow:

1. Save periodic world-state checkpoints.
2. Keep event logs or command logs between checkpoints.
3. Use replay diagnostics when a resumed simulation diverges.
4. Record save-format review and migration digests with release validation artifacts.
5. Treat unsupported save formats as product migration work, not as silent load success.

For server-authoritative hosts, pair this document with [`server-authoritative-mmo.md`](server-authoritative-mmo.md). For product upgrade guidance, pair it with [`migration.md`](migration.md).
