# Public API

Version: **1.0.0**

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
