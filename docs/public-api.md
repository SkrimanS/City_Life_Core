# Public API

Version: **4.0.0**

City Life Core exposes a source-first public C++ API and a smaller C ABI for foreign-language integrations.

The public API should be understood by boundary:

- native C++ consumers use the public C++ API;
- C and foreign-language consumers use the C ABI;
- C# / Unity wrappers call the C ABI through P/Invoke;
- Browser/WebAssembly support is planned as an adapter over exported C-compatible functions;
- examples demonstrate usage but are not separate core API contracts.

---

## Recommended C++ API entry point

Use the aggregate C++ header for normal native integrations:

```cpp
#include "clc/CityLifeCore.hpp"
```

This is the recommended entry point for:

- native C++ games;
- simulation servers;
- backend services;
- editors and tools;
- offline validation and balancing workflows.

The C++ API is the richest API surface today. It is source-first: downstream projects should rebuild against the SDK version they consume.

---

## Game profile C++ API

v1.4.0 adds a descriptive game-profile API for integration planning and SDK adoption review.

Use:

```cpp
#include "clc/sim/GameProfiles.hpp"
#include "clc/sim/GameProfileValidation.hpp"
#include "clc/sim/GameProfileValidationMarkdown.hpp"
#include "clc/sim/GameProfileScenarios.hpp"
#include "clc/sim/GameProfileAdoption.hpp"
#include "clc/sim/GameProfileChecklist.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The profile API exposes:

- profile descriptors for native C++, Unity/C#, Browser/WASM, backend-service, MMO-like and tooling workflows;
- support status and integration-boundary metadata;
- required/optional system lists and non-goals;
- catalog summary and markdown helpers for support, C ABI, Action Bridge and server-authoritative counts;
- query helpers for C ABI, Action Bridge, server-authoritative and required-system filtering;
- catalog validation helpers with digest and markdown output for SDK review;
- scenario recommendations backed by `SimulationScenarioPreset`;
- scenario summary helpers with recommendation counts and day-window totals;
- adoption report helpers with profile-specific digest and markdown output;
- profile-specific adoption summary helpers with scenario day-window counts;
- integration checklist helpers with profile-specific digest, markdown and summary helpers for host-side review.

The profile API is descriptive. It does not add a runtime mode, engine adapter, networking layer, UI framework or product-specific gameplay system.

See [`game-profiles.md`](game-profiles.md).

---

## Server-authoritative C++ API

v1.6.0 adds a source-level C++ foundation for server-authoritative and MMO-like runtime flows.

Use:

```cpp
#include "clc/sim/ServerAuthoritative.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The server-authoritative surface exposes host-owned session metadata, shard descriptors, ordered runtime action envelopes, dispatch through the local Action Bridge model, audit records, sequence summaries, sequence validation and digest output.

This surface is descriptive and local. It does not implement networking, sessions, auth, matchmaking, replication, persistence storage or MMO infrastructure.

See [`server-authoritative-mmo.md`](server-authoritative-mmo.md).

---

## Economy depth C++ API

v1.7.0 adds a source-level C++ foundation for deeper economy, faction and contract review.

Use:

```cpp
#include "clc/sim/EconomyDepth.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The economy depth surface exposes market-aware contract assessments, storage/caravan resource-flow plans, portfolio summaries, validation helpers and digest output. It helps games and tools review deliverability, reputation gates, reward-versus-market value and resource shortages without mutating the simulation.

This surface is a composable foundation. It does not implement taxes, diplomacy systems, AI priorities, inflation, settlement policy, UI, persistence storage or network services.

See [`economy-depth.md`](economy-depth.md).

---

## Persistence, replay and migration C++ API

v1.8.0 adds a source-level C++ foundation for save-format review, supported legacy world-state migration, replay diagnostics and checkpoint planning.

Use:

```cpp
#include "clc/sim/SimulationPersistenceReplay.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The surface exposes save-format manifests, save review reports, migration reports, replay comparison diagnostics, event-log checksum comparison integration, checkpoint plans and digest output.

This surface does not provide product databases, cloud storage, file rotation, arbitrary schema migration, replication or save-service infrastructure.

See [`persistence-replay-migration.md`](persistence-replay-migration.md).

---

## Scale Diagnostics C++ API

v1.9.0 adds a source-level C++ foundation for scale review and release-readiness diagnostics.

Use:

```cpp
#include "clc/sim/ScaleDiagnostics.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The surface exposes runtime scale snapshots, configurable thresholds, validation, digest output and markdown output. It helps tools and servers count large-world pressure points such as settlements, caravans, contracts, ledger entries, event logs and serialized world-state lines.

This surface is diagnostic. It does not provide a memory profiler, scheduler, database, sharding system or production hosting layer.

See [`scale-performance.md`](scale-performance.md).

---

## Settlement Development C++ API

The post-v2 settlement development helper adds a source-level C++ planning surface for deeper settlement, resource and production behavior after the `v2.0.0` foundation.

Use:

```cpp
#include "clc/sim/SettlementDevelopment.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The surface exposes settlement development plans, resource needs, production opportunities, priority/action enums, validation, lookup helpers and digest output. It helps tools, servers and balancing workflows understand missing inputs, market-pressured outputs, idle worker slots and unknown building definitions without mutating the settlement.

This surface does not implement AI, worker assignment, resource purchasing, construction, UI, networking or persistence storage.

See [`settlement-development.md`](settlement-development.md).

---

## Multiplayer and Large-World C++ APIs

The v2.1.0 through v3.0.0 roadmap line adds source-level C++ APIs for authoritative multiplayer-style hosts and the first large-world foundation.

Use:

```cpp
#include "clc/sim/ActionAuthority.hpp"
#include "clc/sim/SnapshotSync.hpp"
#include "clc/sim/MultiplayerReplay.hpp"
#include "clc/sim/MultiplayerEconomySafety.hpp"
#include "clc/sim/MultiplayerLoadDiagnostics.hpp"
#include "clc/sim/Pre3Readiness.hpp"
#include "clc/sim/LargeWorld.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

These surfaces provide actor authority review, snapshot summaries, command-log replay, economy safety reviews, load diagnostics, pre-3 readiness reports and large-world region assignments. They remain local, deterministic and headless.

They do not implement networking, WebSocket/HTTP, auth, accounts, matchmaking, database storage, real shard servers, UI or a game client.

See [`multiplayer-action-authority.md`](multiplayer-action-authority.md), [`snapshot-sync-model.md`](snapshot-sync-model.md), [`multiplayer-persistence-replay.md`](multiplayer-persistence-replay.md), [`multiplayer-economy-safety.md`](multiplayer-economy-safety.md), [`multiplayer-load-diagnostics.md`](multiplayer-load-diagnostics.md), [`pre3-large-world-prep.md`](pre3-large-world-prep.md) and [`large-world-foundation.md`](large-world-foundation.md).

---

## Action Bridge C++ API

v1.2.0 adds a local Action Bridge for external game layers, tools and future server-authoritative adapters:

```text
external action -> validation -> runtime mutation -> result/events
```

Use:

```cpp
#include "clc/sim/ActionBridge.hpp"
```

or the aggregate header:

```cpp
#include "clc/CityLifeCore.hpp"
```

The bridge accepts JSON actions with:

- `action_id`;
- `type`;
- optional `actor_id`;
- action-specific `payload` object.

It returns action results with:

- `accepted`;
- `validation_status`;
- stable `error_code`;
- message;
- validation diagnostics;
- produced events.

The Action Bridge is local and transport-agnostic. It is not HTTP, WebSocket, accounts, auth, matchmaking, multiplayer, MMO or UI.

See [`action-bridge.md`](action-bridge.md).

---

## C ABI entry point

Use the C interface header for C consumers and foreign-language binding layers:

```c
#include "clc/c/CityLifeCoreC.h"
```

The current C ABI exposes:

- version helpers;
- C interface version;
- tick/time utilities;
- opaque `clc_world` create/destroy;
- basic world state access;
- simple tick advancement;
- duration-based world advancement by seconds, minutes, hours or days;
- read-only world event inspection.

The C ABI is intentionally smaller than the C++ API. It is the planned stable boundary for C#, Unity, Browser/WASM and other language or engine bindings.

For the staged expansion plan, see [`c-abi-expansion-plan.md`](c-abi-expansion-plan.md).

---

## Public API rules

- Prefer namespaced C++ APIs under `clc::*` for native gameplay/runtime integrations.
- Treat installed headers under `include/clc` as the supported SDK surface.
- Use `clc/CityLifeCore.hpp` as the normal C++ include.
- Use `clc/c/CityLifeCoreC.h` for C ABI consumers and foreign-language bindings.
- Use the local Action Bridge when an external tool/game layer needs action validation and dispatch without becoming coupled to runtime internals.
- Use the game-profile API as descriptive integration guidance, not as a separate product mode or runtime fork.
- Do not bind C#, Unity, Browser/WASM or scripting integrations directly to C++ implementation details.
- Validate registries and runtime state before relying on loaded content.
- Prefer tick-based helpers for server-authoritative, real-time or MMO-like runtime flows.
- Use day helpers only where turn/day simulation semantics are intended.
- Keep game-specific rules outside the SDK and compose them around the runtime APIs.
- Keep engine-specific adapters outside the native core.

---

## Common C++ workflow

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```

For real integrations, prefer explicit registry and runtime construction over demo bootstrap helpers once the project has its own data and workflows.

---

## Common game profile workflow

```cpp
const auto* profile = clc::sim::game_integration_profile_by_id("backend_service");
if (profile == nullptr) {
    return;
}

const auto summary = clc::sim::game_integration_profile_catalog_summary();
const auto catalog_markdown = clc::sim::game_integration_profile_catalog_markdown();
const auto validation = clc::sim::validate_game_profile_catalog();
const auto validation_markdown = clc::sim::game_profile_catalog_validation_markdown(validation);
const auto adoption_digest = clc::sim::game_profile_adoption_report_digest("backend_service");
const auto adoption_markdown = clc::sim::game_profile_adoption_report_markdown(*profile);
const auto adoption_summary = clc::sim::game_profile_adoption_summary("backend_service");
const auto checklist_digest = clc::sim::game_profile_checklist_digest("backend_service");
const auto checklist_markdown = clc::sim::game_profile_checklist_markdown(*profile);
const auto checklist = clc::sim::make_game_profile_checklist(*profile);
const auto checklist_summary = clc::sim::game_profile_checklist_summary("backend_service");
```

---

## Common Action Bridge workflow

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& engine = bootstrap.runtime.engine;
const auto result = clc::sim::dispatch_runtime_action_json(
    engine,
    R"({"action_id":"a1","type":"advance_days","payload":{"days":1}})"
);
```

---

## Common C ABI workflow

The C ABI workflow is intentionally minimal today:

```c
#include "clc/c/CityLifeCoreC.h"

clc_world* world = clc_world_create_c("Demo World", 42);
clc_world_advance_ticks_c(world, clc_minutes_to_ticks_c(5));
clc_world_destroy_c(world);
```

Use the C ABI when the consumer cannot call C++ directly or when a stable foreign-function boundary is required.

---

## Unity / C# API boundary

Unity/C# code should call the C ABI through P/Invoke, using the example wrapper as the current starting point:

```text
examples/csharp_unity/CityLifeCoreNative.cs
examples/csharp_unity/CityLifeSmokeTest.cs
```

This is not yet a full managed SDK. It is an initial wrapper over the current minimal C ABI.

Unity support should expand only as the C ABI expands and stabilizes.

---

## Browser / WebAssembly API boundary

Browser/WebAssembly support is planned, not implemented.

The intended future shape is:

```text
Browser app
  -> JavaScript or TypeScript adapter
  -> WebAssembly module
  -> C ABI / exported functions
  -> native C++ core
```

Browser-specific APIs should not become part of the native core API.

---

## What is not public API

Do not rely on:

- implementation files under `src/`;
- private helper functions;
- undocumented internal headers;
- C++ object binary layouts;
- STL container layouts;
- C++ symbols as a foreign-language binding surface;
- example code as a long-term compatibility contract beyond its documented purpose.

---

## Related documents

- [Public API status](public-api-status.md)
- [Action Bridge](action-bridge.md)
- [C interface](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Game integration profiles](game-profiles.md)
- [Integration targets](integration-targets.md)
- [Core concepts](core-concepts.md)
- [Compatibility](compatibility.md)
- [Build and linking policy](build-and-linking-policy.md)
