# City Life Core Architecture

Status: **architecture overview after v1.0.0**

City Life Core is a headless simulation SDK. Its architecture keeps domain simulation, data definitions, validation, persistence, packaging and external integration boundaries separate.

The core should remain portable and engine-agnostic while allowing different games, tools, servers and clients to embed the same simulation logic.

---

## High-level shape

```text
Game / tool / server / editor / client
  -> integration profile
  -> integration boundary
      -> public C++ API
      -> C ABI
          -> C consumer
          -> C# / Unity wrapper
          -> future Browser/WASM adapter
          -> future language/engine bindings
  -> City Life Core simulation systems
```

The core should not depend on the consuming engine. Unity, browser applications, servers, tools and custom engines should depend on City Life Core through documented boundaries.

---

## Core layers

### Public API layer

The public C++ API is the richest integration path for native C++ users.

Recommended include:

```cpp
#include "clc/CityLifeCore.hpp"
```

The public API should remain source-first. C++ binary ABI stability is not promised.

### C ABI layer

The C ABI is the smaller and more stable foreign-function boundary.

Recommended include:

```c
#include "clc/c/CityLifeCoreC.h"
```

The current C ABI is intentionally minimal and exposes version/time utilities, an opaque world handle and read-only world event access. It is the planned base for C#, Unity, Browser/WASM and other future bindings.

Expansion should follow [`c-abi-expansion-plan.md`](c-abi-expansion-plan.md).

### Data layer

The data layer stores definitions and validates references before runtime use.

It covers game definitions such as resources, buildings, professions, settlements and other content that runtime systems consume.

### Simulation layer

The simulation layer owns the main gameplay primitives:

- settlements;
- storage;
- routes;
- caravans;
- factions;
- ownership;
- contracts;
- events;
- runtime time.

It should stay deterministic where possible and prefer explicit state transitions.

### Economy layer

The economy layer provides:

- market primitives;
- wallets;
- trade operations;
- ledgers.

It should gradually connect more deeply with settlements, resources, factions, contracts and logistics.

### Persistence and replay layer

The persistence and replay layer saves, loads, validates and compares runtime state.

This layer is important for:

- shipped games;
- server-authoritative runtime flows;
- debugging;
- long-running simulations;
- deterministic replay;
- migration between save shapes.

### Packaging layer

The packaging layer exports the SDK through:

- CMake package config files;
- installed headers;
- native library artifacts;
- documentation;
- examples;
- optional SDK ZIP archives.

Installed examples include native C++ consumers, C ABI consumers and initial C# / Unity wrapper examples.

---

## Integration layers

### Native C++ integrations

Native C++ integrations should use the public C++ API and CMake package.

This is the best-supported path today.

### C and foreign-language integrations

C and foreign-language integrations should use the C ABI. They should not bind directly to unstable C++ implementation details.

### Unity / C# integrations

Unity/C# integrations should use P/Invoke over the C ABI:

```text
Unity C# code
  -> CityLifeCoreNative.cs
  -> C ABI
  -> native City Life Core shared library
```

The C++ core should not include Unity-specific code.

### Browser / WebAssembly integrations

Browser integrations are planned as a future WebAssembly adapter:

```text
Browser app
  -> JavaScript or TypeScript adapter
  -> WebAssembly module
  -> C ABI / exported functions
  -> native core
```

Browser APIs should not enter the native core.

### Server-authoritative and MMO-like integrations

Server-authoritative and MMO-like integrations should initially use the C++ API and runtime workflows.

Over time, stable parts may be exposed through the C ABI where useful for tooling or foreign-language services.

The core should not become a networking framework. Transport, authentication, replication, matchmaking and client prediction belong outside the core unless a future policy explicitly changes that boundary.

---

## Architectural principles

- Keep the SDK headless and renderer/UI/network agnostic.
- Prefer deterministic runtime operations and explicit validation.
- Keep the C++ API source-first.
- Use the C ABI as the stable foreign-function boundary.
- Keep handles opaque across the C ABI.
- Keep native memory ownership explicit.
- Do not expose C++ exceptions, templates, STL containers or raw internals through the C ABI.
- Let downstream games, servers, tools and adapters own product-specific rules.
- Keep Unity, browser and engine-specific code outside the native core.
- Keep installed documentation English-primary, with Russian documentation under `docs/ru`.

---

## Non-goals

City Life Core should not become:

- a game client;
- a renderer;
- a UI framework;
- a Unity-only SDK;
- a browser-only SDK;
- a networking framework;
- an engine-specific gameplay framework.

The goal is a reusable simulation core with clear integration boundaries.

---

## Related documents

- [Core concepts](core-concepts.md)
- [Public API](public-api.md)
- [C ABI](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [Game integration profiles](game-profiles.md)
- [Integration targets](integration-targets.md)
- [Integration validation](integration-validation.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [SDK structure](sdk-structure.md)
- [Packaging](packaging.md)
