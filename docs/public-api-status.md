# Public API Status

Version: **2.0.0**

This document classifies the installed API surface for the 2.0.0 stable multi-game SDK foundation.

City Life Core has multiple public-facing surfaces. They do not all have the same status.

---

## Stable for 1.x source use

The primary public C++ API is intended for source-level use by native C++ integrations.

Recommended include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Stable-for-source-use areas include:

- version and time utilities;
- data registry and validation APIs;
- settlement, route, caravan and runtime simulation APIs;
- economy primitives: market, wallet, trade and ledger;
- faction, ownership and contract APIs where documented;
- persistence and save/load validation helpers;
- event and replay-related helpers where documented;
- local Action Bridge APIs for transport-agnostic external action dispatch;
- game profile catalog, catalog markdown output, summary, validation digest/markdown output, scenario recommendation, scenario summary, adoption report, profile-specific adoption report output, adoption summary, adoption scenario day-window summary, checklist, profile-specific checklist digest/markdown output, checklist summary and profile-specific checklist summary helpers.

Source compatibility is prioritized for the 1.x line where practical. C++ binary ABI stability is not the primary compatibility contract.

---

## Game profile C++ surface

The game profile API is a descriptive C++ SDK surface for integration planning, profile discovery and adoption review.

Headers:

```cpp
#include "clc/sim/GameProfiles.hpp"
#include "clc/sim/GameProfileValidation.hpp"
#include "clc/sim/GameProfileValidationMarkdown.hpp"
#include "clc/sim/GameProfileScenarios.hpp"
#include "clc/sim/GameProfileAdoption.hpp"
#include "clc/sim/GameProfileChecklist.hpp"
```

Recommended aggregate include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Status:

- public for C++ source use;
- descriptive and adoption-oriented;
- useful for native games, Unity/C# planning, Browser/WASM planning, backend services, MMO-like services and editor/tooling workflows;
- not a runtime mode switch;
- not engine-specific adapter code;
- not part of the C ABI.

The profile surface exposes profile descriptors, support status, required/optional systems, non-goals, catalog summary/markdown helpers, query helpers, catalog validation digest/markdown helpers, scenario recommendations, scenario summaries, adoption reports with profile-specific digest/markdown output, adoption summaries with scenario day-window counts, integration checklists and profile-specific checklist digest/markdown/summary helpers.

---

## Server-authoritative C++ surface

The server-authoritative foundation API is a source-level C++ SDK surface for host-owned server command review and audit flows.

Header:

```cpp
#include "clc/sim/ServerAuthoritative.hpp"
```

Recommended aggregate include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Status:

- public for C++ source use;
- local and transport-agnostic;
- useful for ordered server action envelopes, audit records, shard descriptors and replay-sensitive command sequence checks;
- not a networking, session, account, auth, replication, persistence-storage or MMO framework;
- not part of the C ABI.

See [`server-authoritative-mmo.md`](server-authoritative-mmo.md).

---

## Economy depth C++ surface

The economy depth foundation API is a source-level C++ SDK surface for connected economy, faction and contract review.

Available through:

```cpp
#include "clc/sim/EconomyDepth.hpp"
```

and:

```cpp
#include "clc/CityLifeCore.hpp"
```

Status:

- source-level C++ API;
- internal v1.7.0 milestone;
- market-aware contract assessment;
- storage and caravan flow planning;
- portfolio summary and digest helpers;
- validation helpers for blocked or risky contract states.

It is not a C ABI surface yet and does not define product-specific economy rules such as taxes, auctions, diplomacy, AI priorities, inflation, UI or persistence storage.

See [`economy-depth.md`](economy-depth.md).

---

## Persistence, replay and migration C++ surface

The persistence/replay/migration foundation API is a source-level C++ SDK surface for save-format review, supported legacy world-state migration, replay mismatch diagnostics and checkpoint planning.

Available through:

```cpp
#include "clc/sim/SimulationPersistenceReplay.hpp"
```

and:

```cpp
#include "clc/CityLifeCore.hpp"
```

Status:

- source-level C++ API;
- internal v1.8.0 milestone;
- save-format manifest and review helpers;
- migration helper for supported older version-1 world-state shapes;
- replay diagnostics with serialized state mismatch counts;
- optional event-log checksum comparison;
- checkpoint planning for long-running simulations.

It is not a C ABI surface yet and does not provide product save services, databases, cloud storage, file rotation, arbitrary schema migration or replication infrastructure.

See [`persistence-replay-migration.md`](persistence-replay-migration.md).

---

## Scale diagnostics C++ surface

The scale diagnostics foundation API is a source-level C++ SDK surface for runtime scale review and release-readiness diagnostics.

Available through:

```cpp
#include "clc/sim/ScaleDiagnostics.hpp"
```

and:

```cpp
#include "clc/CityLifeCore.hpp"
```

Status:

- source-level C++ API;
- internal v1.9.0 milestone;
- runtime scale snapshots;
- configurable watch/high thresholds;
- validation, digest and markdown helpers;
- benchmark coverage for snapshot and event-log checksum paths.

It is not a memory profiler, scheduler, database, sharding system or hosting layer.

See [`scale-performance.md`](scale-performance.md).

---

## Action Bridge C++ surface

The Action Bridge is a C++ SDK surface for local action dispatch:

```text
external action -> validation -> runtime mutation -> result/events
```

Header:

```cpp
#include "clc/sim/ActionBridge.hpp"
```

Recommended aggregate include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Status:

- public for C++ source use;
- local and transport-agnostic;
- not a network API;
- not a multiplayer/session/account system;
- not part of the C ABI yet.

The canonical action input format uses `action_id`, `type`, optional `actor_id` and an action-specific JSON `payload` object. Results expose `accepted`, `validation_status`, stable `error_code`, `command_detail`, `events_detail` and `diagnostics_detail`.

---

## Minimal C ABI

The C ABI is the intended stable foreign-function boundary for C, C# / Unity, Browser/WASM and future language/engine bindings.

Header:

```c
#include "clc/c/CityLifeCoreC.h"
```

Current C ABI scope:

- version helpers;
- C interface version;
- tick/time utilities;
- opaque `clc_world` handle;
- basic world state access;
- simple tick advancement;
- duration-based world advancement by seconds, minutes, hours or days;
- read-only world event inspection.

The C ABI is intentionally small today. It does not yet expose the full runtime, registry, validation, persistence, economy, faction, contract, profile catalog or Action Bridge systems.

Expansion should follow:

```text
docs/c-abi-expansion-plan.md
```

---

## Examples and adapters

Examples and adapter code are public-facing, but they are not the same as core API guarantees.

Current examples include:

```text
examples/find_package_consumer/
examples/c_abi_consumer/
examples/csharp_unity/
examples/action_bridge.cpp
examples/game_profiles.cpp
```

Status:

- `examples/find_package_consumer/` demonstrates installed C++ package usage.
- `examples/c_abi_consumer/` demonstrates minimal C ABI usage.
- `examples/csharp_unity/` demonstrates initial C# / Unity P/Invoke usage.
- `examples/action_bridge.cpp` demonstrates local C++ Action Bridge dispatch.
- `examples/game_profiles.cpp` demonstrates game profile discovery, catalog summary/markdown output, catalog validation digest/markdown output, scenario recommendations, scenario summaries, profile-specific adoption report output, adoption summaries with scenario day-window output, checklists, profile-specific checklist digest/markdown output and profile-specific checklist summaries.

The C# / Unity wrapper is an initial integration example. It should track the C ABI and should not be treated as a complete managed SDK yet.

---

## Planned integration adapters

The following are planned or early-stage integration surfaces, not stable public APIs yet:

- full Unity package layout;
- high-level managed C# API;
- Browser/WebAssembly adapter;
- JavaScript or TypeScript wrapper;
- other engine or language bindings;
- future server-authoritative action queues and permissions layered on top of the local Action Bridge.

These should be developed through the C ABI or another deliberately stable foreign-function boundary.

---

## Private or unsupported internals

Do not treat these as stable public API:

- implementation files under `src/`;
- internal helper functions not included from documented public headers;
- private C++ types not exposed in installed public headers;
- STL container layouts or binary object layouts;
- C++ symbols as a foreign-language binding surface;
- examples as compatibility contracts beyond their documented purpose.

---

## Compatibility expectations

- Source compatibility is prioritized for supported C++ headers in the 1.x line.
- C++ binary ABI stability is not promised across arbitrary compilers, standard libraries or build configurations.
- C ABI compatibility should be treated separately and versioned through the C interface version when needed.
- C# / Unity and future Browser/WASM wrappers should follow the C ABI rather than private C++ internals.
- The Action Bridge is a local C++ SDK surface; networking, sessions, auth and multiplayer behavior are future layers, not part of the bridge.
- The game profile API is a source-level C++ guidance surface; it does not define a C ABI or foreign-language contract.
- Prefer rebuilding consumers against the installed SDK package.

---

## Related documents

- [Public API](public-api.md)
- [Game integration profiles](game-profiles.md)
- [Action Bridge](action-bridge.md)
- [C ABI](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Integration targets](integration-targets.md)
- [Compatibility](compatibility.md)
- [Build and linking policy](build-and-linking-policy.md)
