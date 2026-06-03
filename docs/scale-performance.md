# Scale and Performance Guidance

Status: v1.9.0 foundation for v2.0.0 release readiness.

City Life Core is a headless simulation SDK, not a hosted scale service. The v1.9.0 work adds coarse benchmark coverage and runtime scale diagnostics so games, tools and servers can reason about large-world tradeoffs before committing to a production architecture.

## Public API

Use:

```cpp
#include <clc/sim/ScaleDiagnostics.hpp>
```

or:

```cpp
#include <clc/CityLifeCore.hpp>
```

The API provides:

- `RuntimeScaleSnapshot`;
- `ScaleProfileThresholds`;
- `make_runtime_scale_snapshot`;
- `validate_runtime_scale_snapshot`;
- digest and markdown helpers.

## What The Snapshot Counts

The runtime scale snapshot counts:

- settlements;
- buildings;
- storage entries;
- routes;
- caravans;
- caravan cargo entries;
- factions and reputations;
- ownership rows;
- contracts;
- ledger entries;
- engine events;
- optional runtime event-log entries;
- serialized world-state lines;
- estimated entity count.

The result is not a memory profiler. It is a stable diagnostic shape for release review, tools and server dashboards.

## Risk Levels

Default thresholds classify snapshots as:

- `nominal`: normal SDK-scale usage;
- `watch`: review architecture and validation cost;
- `high`: investigate scale assumptions before shipping.

Host projects can pass custom `ScaleProfileThresholds` for their own product limits.

## Benchmarks

The benchmark runner includes coarse baseline scenarios for:

- settlement day advancement;
- scenario report generation;
- world-state serialization/deserialization;
- large caravan runtime runs;
- runtime scale snapshot generation;
- event-log checksum calculation.

Benchmarks are artifact signals, not strict timing gates. Review benchmark names, elapsed times and detail fields when comparing release candidates.

## Practical Tradeoffs

Large-world integrations should:

- keep validation and diagnostics explicit instead of running every expensive check every tick;
- checkpoint long simulations periodically;
- compare replay diagnostics after migration or deterministic command changes;
- keep product databases, sharding, replication and file rotation outside the core;
- measure with representative product data rather than relying only on SDK examples.

## v2.0.0 Boundary

For v2.0.0, scale work is a foundation:

- diagnostics exist;
- benchmarks exist;
- release guidance exists;
- product deployment scale remains host-owned.

Post-v2 work can deepen large-world scenarios, performance profiling and hot-path optimization once real product profiles make the tradeoffs clearer.
