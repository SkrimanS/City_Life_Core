# Public Surface 2.0.0

Status: frozen intended public surface for the v2.0.0 release candidate.

This document records the source-level and integration surface intended for the public `v2.0.0` milestone. It is a release review aid, not a promise that every internal helper is ABI-stable.

## Primary C++ Include

Recommended native consumers should use:

```cpp
#include <clc/CityLifeCore.hpp>
```

The umbrella header exposes the stable-candidate source-level SDK surface for native C++ integrations.

## Public C++ Areas

The v2.0.0 public C++ surface includes:

- core version, ids, result, time and event-log utilities;
- data registry, definitions and validation;
- market, trade and ledger helpers;
- settlements, storage, routes and caravans;
- factions, ownership, contracts and contract rewards;
- runtime workflows, tick runtime, event-log helpers and persistence validation;
- Action Bridge local command dispatch;
- game profile catalog, adoption, checklist and scenario guidance;
- platform/deep-system diagnostics;
- server-authoritative action envelopes and audit helpers;
- economy-depth assessment and flow-planning helpers;
- persistence/replay/migration review and diagnostics helpers;
- scale diagnostics helpers.

## C ABI Surface

The C ABI remains intentionally smaller than the C++ API. In v2.0.0 it exposes:

- core version functions;
- C interface version;
- tick conversion helpers;
- opaque world create/destroy;
- basic world tick advancement;
- read-only world state/event inspection.

The current C interface version is `4`.

## Documentation Surface

The v2.0.0 documentation set includes:

- public API and public API status;
- C ABI docs and C ABI expansion plan;
- integration targets and validation;
- Unity/C# and Browser/WASM guidance;
- game profiles;
- server-authoritative/MMO foundation;
- economy-depth foundation;
- persistence/replay/migration foundation;
- scale/performance guidance;
- packaging, SDK ZIP, CMake package and release verification docs.

## Non-Goals

v2.0.0 does not freeze:

- C++ binary ABI;
- full C ABI parity with C++;
- Unity package format;
- Browser/WASM adapter;
- network transport, matchmaking, auth or replication;
- product database or save-service infrastructure;
- sharding/hosting infrastructure;
- game-specific economy, diplomacy, AI, UI or balancing rules.

## Post-v2 Direction

After v2.0.0, the project can deepen gameplay mechanics and scale work while preserving clear integration boundaries. Breaking changes should be documented in changelog, migration guidance and future public release notes.
