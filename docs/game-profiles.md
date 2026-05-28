# Game Integration Profiles

Status: **planning / v1.x adoption guidance**

City Life Core is intended to support different game types and integration styles without becoming tied to one engine, platform, renderer or network stack.

This document describes the main profiles that the SDK should support or prepare for.

---

## Profile principle

A profile is not a separate product or fork. A profile is a recommended way to combine the same headless simulation core with a specific host environment.

```text
City Life Core simulation systems
  -> game profile
      -> native game
      -> Unity client
      -> browser/WASM game
      -> server-authoritative backend
      -> MMO-like simulation service
      -> editor/tooling workflow
```

The same core systems should remain reusable across profiles:

- time and ticks;
- data registry and validation;
- settlements;
- resources and storage;
- routes and caravans;
- factions and ownership;
- contracts;
- economy and ledgers;
- events;
- persistence and replay.

---

## Current profile support summary

| Profile | Current status | Main integration boundary | Notes |
| --- | --- | --- | --- |
| Native C++ game | Supported | C++ API | Richest integration path today. |
| Native C++ server/tool | Supported | C++ API and CMake package | Good fit for backend services and tools. |
| C consumer | Minimal supported | C ABI | Useful as a stable FFI baseline. |
| Unity / C# client or tool | Initial support | C ABI + P/Invoke | Wrapper and smoke test exist; full Unity package does not. |
| Browser / WebAssembly game/tool | Planned | Future WASM + JS/TS adapter | Planning documented; not implemented. |
| Server-authoritative game | Partially supported | C++ API and runtime workflows | More command/replay/persistence hardening planned. |
| MMO-like simulation | Planned / early foundation | C++ API first, later stable boundaries | Needs shard/partition, command and replay depth. |
| Editor / balancing tool | Partially supported | C++ API, C ABI, validation docs | Needs richer diagnostics and data-authoring APIs. |

---

## Native C++ game profile

Use when the game or engine layer is written in C++ and can link directly to the SDK.

Recommended boundary:

```cpp
#include "clc/CityLifeCore.hpp"
```

Best suited for:

- city builders;
- settlement simulations;
- strategy games;
- logistics games;
- colony/survival games;
- offline simulation tools.

Recommended systems:

- registry and validation;
- tick runtime;
- settlements and resources;
- routes and caravans;
- factions and ownership;
- contracts;
- economy, wallet and ledger;
- persistence and replay when save/debug workflows are needed.

---

## Unity / C# profile

Use when Unity is the gameplay client, editor environment or visualization layer.

Recommended boundary:

```text
Unity C# code
  -> CityLifeCoreNative.cs
  -> C ABI
  -> native City Life Core shared library
```

Current files:

```text
examples/csharp_unity/CityLifeCoreNative.cs
examples/csharp_unity/CityLifeSmokeTest.cs
docs/csharp-unity.md
```

Current scope:

- native version access;
- C ABI version access;
- tick helpers;
- opaque world create/destroy;
- simple tick advancement;
- read-only event access.

Not yet included:

- full Unity package layout;
- generated managed API;
- prebuilt native plug-ins;
- Unity editor validation tools;
- full runtime, registry, persistence, economy, faction or contract coverage.

Unity should remain a consumer of the core. The C++ core should not include Unity-specific code.

---

## Browser / WebAssembly profile

Use when the simulation must run in a browser-hosted game, demo, web editor or visualization tool.

Recommended future boundary:

```text
Browser app
  -> JavaScript or TypeScript adapter
  -> WebAssembly module
  -> C ABI / stable exported functions
  -> native C++ core
```

Current status: planned.

Related document:

```text
docs/browser-wasm.md
```

Important constraints:

- no browser APIs should enter the native core;
- data loading must be explicit and browser-friendly;
- JS/WASM memory ownership must be documented;
- useful browser support depends on a wider stable C ABI.

---

## Server-authoritative profile

Use when the authoritative simulation runs on a server and clients only submit commands or receive state/events.

Recommended boundary today:

```text
Server code
  -> C++ API
  -> SimulationRuntime
  -> validation / persistence / replay
```

Current fit:

- deterministic tick runtime;
- world events;
- persistence and replay foundations;
- runtime validation;
- economy, faction and contract systems as server-owned logic.

Needed depth:

- command validation model;
- player/session separation from simulation state;
- stable replay and audit workflows;
- better diagnostics for invalid commands;
- clearer shard/partition assumptions;
- stronger persistence/migration guarantees.

---

## MMO-like profile

Use when the project needs long-running worlds, server-side simulation and many connected users or agents.

Current status: early foundation.

The core should support MMO-like scenarios by deepening:

- deterministic command processing;
- long-running tick simulation;
- persistence and migrations;
- replay and audit logs;
- world partitioning assumptions;
- economy/logistics/faction depth;
- diagnostics and observability.

The core should not become a networking framework. Network transport, matchmaking, authentication, replication and client prediction belong outside City Life Core unless a future policy explicitly changes that boundary.

---

## Editor and balancing tool profile

Use when City Life Core is embedded in a tool for content authoring, balancing, validation or simulation analysis.

Recommended boundary:

- C++ API for native tools;
- C ABI for foreign-language tools;
- future managed/JS adapters when stable enough.

Useful systems:

- data registry loading;
- validation diagnostics;
- simulation tick stepping;
- event inspection;
- economy and resource reporting;
- persistence/replay comparison;
- benchmarks and scale diagnostics.

Needed depth:

- richer validation results;
- stable C ABI diagnostics surface;
- better data-authoring feedback;
- reporting APIs that are easy to bind.

---

## Roadmap alignment

### v1.x

Before `v2.0.0`, profiles should remain mostly adoption guidance and integration hardening.

Focus:

- keep native C++ support strong;
- keep C ABI safe and small;
- document Unity/C# and Browser/WASM paths;
- identify missing APIs for real external integrations;
- avoid engine-specific coupling.

### v2.x / v3.x

After `v2.0.0`, deeper mechanics should make profiles more useful:

- richer settlements and resources;
- deeper economy and factions;
- contract lifecycle depth;
- runtime diagnostics;
- validation and persistence improvements;
- replay and migration workflows;
- larger-world support.

### v4.0.0+

After `v4.0.0`, profiles should become useful for large production projects, complex worlds and MMO-like simulations while keeping the core headless and portable.

---

## Related documents

- [Integration targets](integration-targets.md)
- [Integration validation](integration-validation.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Roadmap](roadmap.md)
