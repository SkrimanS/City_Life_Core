# Public Surface 3.0.0

Status: intended public source-level surface for the `v3.0.0` release candidate.

## New C++ Source-Level Surfaces

- `clc/sim/ActionAuthority.hpp`
- `clc/sim/SnapshotSync.hpp`
- `clc/sim/MultiplayerReplay.hpp`
- `clc/sim/MultiplayerEconomySafety.hpp`
- `clc/sim/MultiplayerLoadDiagnostics.hpp`
- `clc/sim/Pre3Readiness.hpp`
- `clc/sim/LargeWorld.hpp`

These headers are also exposed through:

```cpp
#include "clc/CityLifeCore.hpp"
```

## C ABI

The C ABI interface version is `8` after adding read-only SDK handoff, runtime, regional, replay, completion, package-manifest and platform diagnostics for foreign-language integrations.

## Non-Goals

3.0.0 does not add networking, accounts, auth, matchmaking, database storage, real shard servers, UI or a game client.
