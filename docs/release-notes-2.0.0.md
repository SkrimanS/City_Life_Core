# Release Notes 2.0.0

City Life Core 2.0.0 is the stable multi-game SDK foundation milestone after the initial 1.0.0 release.

## Highlights

- Expanded C++ runtime workflows for tick-based simulation, persistence validation and replay-sensitive comparisons.
- Added local Action Bridge helpers for transport-agnostic action validation and dispatch.
- Added game integration profile catalog, adoption reports, checklist helpers and scenario guidance.
- Added platform integration guidance for native C++, C ABI, Unity/C#, Browser/WASM planning, servers and tools.
- Added deep-system diagnostics for market, ledger, production, logistics, contracts and faction access.
- Added server-authoritative foundations with ordered action envelopes, host-owned session metadata, shard descriptors, audit records and sequence validation.
- Added economy-depth helpers for market-aware contract assessment, storage/caravan flow planning and portfolio summaries.
- Added persistence/replay/migration helpers for save-format review, supported legacy world-state migration, replay diagnostics and checkpoint planning.
- Added scale diagnostics and expanded benchmark coverage for release artifact review.
- Updated the project version and public version APIs to `2.0.0`.

## Public Surface

The intended v2.0.0 public surface is summarized in [`public-surface-2.0.0.md`](public-surface-2.0.0.md).

The primary C++ include remains:

```cpp
#include <clc/CityLifeCore.hpp>
```

The C ABI remains intentionally smaller than the C++ API. The C interface version is `4`.

## Validation

Release validation should include:

- configure/build with tests and examples;
- full CTest pass;
- benchmark build/run and artifact review;
- installed C++ consumer;
- installed C ABI consumer;
- CPack ZIP creation and unpacked consumer checks where possible.

Record final validation in [`release-manifest-2.0.0.md`](release-manifest-2.0.0.md).

## Compatibility Notes

2.0.0 is a public major milestone. Source-level C++ compatibility is the primary integration path. C++ binary ABI stability is not guaranteed.

Save/load and replay users should review [`persistence-replay-migration.md`](persistence-replay-migration.md) and run product-specific migration checks.

## Known Limitations

- Unity/C# support remains example-level through P/Invoke and the C ABI, not a full Unity package.
- Browser/WASM support remains planned guidance unless a downstream project uses the minimal C ABI directly.
- No networking, replication, matchmaking, auth, database, sharding or hosting layer is included.
- Game-specific economy, diplomacy, AI, balancing and UI rules remain outside the core.

## Related Documents

- [`release-manifest-2.0.0.md`](release-manifest-2.0.0.md)
- [`public-surface-2.0.0.md`](public-surface-2.0.0.md)
- [`migration.md`](migration.md)
- [`verifying-releases.md`](verifying-releases.md)
- [`scale-performance.md`](scale-performance.md)
