# Integration Targets

Status: **active planning after v1.0.0**

City Life Core is intended to be usable from many kinds of games, tools and runtime environments. The core itself should stay headless, native, portable and engine-agnostic.

This document separates what is already supported from what is only initially supported or planned.

---

## Integration principle

City Life Core should not become a Unity SDK, browser SDK, Unreal plug-in, renderer, UI framework or networking framework.

The project should instead provide a stable native simulation core and thin integration layers around it:

```text
City Life Core native simulation core
  -> C++ API for native C++ games, tools, servers and editors
  -> local Action Bridge for transport-agnostic action validation and dispatch
  -> C ABI as the foreign-function boundary
      -> C consumers
      -> C# / Unity P/Invoke wrapper
      -> future WebAssembly / JavaScript adapter
      -> future engine or language bindings
```

The staged plan for growing the C ABI is documented in `c-abi-expansion-plan.md`. The local Action Bridge is documented in `action-bridge.md`.

This keeps the core reusable while allowing different platforms to use the same simulation logic through platform-specific adapters.

---

## Current support levels

| Target | Status | Current path | Notes |
| --- | --- | --- | --- |
| Native C++ game/tool/server | Supported | Public C++ API and CMake package | Best-supported integration path. |
| Local external action dispatch | Supported | C++ Action Bridge and `examples/action_bridge.cpp` | Transport-agnostic local validation and mutation; not networking or multiplayer. |
| Native C consumer | Minimal supported | C ABI and `examples/c_abi_consumer/` | Good foreign-function base, but intentionally small. |
| Unity / C# | Initial support | C ABI + P/Invoke wrapper in `examples/csharp_unity/` | Usable as a smoke-test wrapper; not yet a full Unity package. |
| Browser / WebAssembly | Planned | Future Emscripten/WASM build and JavaScript adapter; see `browser-wasm.md` | Not yet implemented. |
| Other engines | Planned | Future adapters through C ABI or engine-specific bindings | Godot, Unreal or custom engines should not call unstable C++ ABI directly. |
| Backend service / MMO server | Partially supported | Native C++ API, Action Bridge and runtime workflows | More server-authoritative command/replay/persistence work is planned. |
| Editor / balancing tools | Partially supported | Native C++ API, Action Bridge, C ABI, validation docs | More diagnostics and data-authoring support is planned. |

---

## Supported now

### Native C++ integrations

C++ integrations can use the public umbrella header:

```cpp
#include "clc/CityLifeCore.hpp"
```

Recommended for:

- native C++ games;
- dedicated simulation servers;
- backend services;
- editors and balancing tools;
- offline simulation or validation tools.

This is the richest API surface today.

### Local Action Bridge integrations

External game layers, tools, editors and future server-authoritative adapters can use the local Action Bridge:

```cpp
#include "clc/sim/ActionBridge.hpp"
```

It provides:

- JSON action input;
- validation before mutation;
- stable action statuses and error codes;
- runtime command dispatch;
- result JSON with command, event and diagnostic details.

It does not provide HTTP, WebSocket, accounts, auth, matchmaking, multiplayer, MMO or UI behavior.

Guidance:

```text
docs/action-bridge.md
```

### Native C ABI integrations

C and foreign-language integrations should use the C ABI:

```c
#include "clc/c/CityLifeCoreC.h"
```

The current C ABI exposes:

- version utilities;
- C interface version;
- tick conversion helpers;
- an opaque world handle;
- basic world state access;
- simple tick advancement;
- read-only world event inspection.

The C ABI is intentionally minimal. It is the safest boundary for languages that should not bind directly to the C++ API.

Expansion plan:

```text
docs/c-abi-expansion-plan.md
```

---

## Initial support

### Unity / C#

Unity support starts with a small C# wrapper over the C ABI:

```text
examples/csharp_unity/CityLifeCoreNative.cs
```

Guidance:

```text
docs/csharp-unity.md
```

Current scope:

- P/Invoke declarations for the existing C ABI;
- managed wrapper for the opaque world handle;
- version and tick helper access;
- basic world advancement;
- read-only event inspection;
- Unity native plug-in layout notes.

Not yet included:

- official Unity package layout;
- prebuilt native plug-ins per platform;
- C# tests in CI;
- editor import validation;
- high-level managed gameplay API;
- full runtime, persistence, replay, registry, settlement, economy, faction or contract coverage.

Unity should consume the native library as a plug-in. The C++ core should not include Unity headers or Unity-specific code.

---

## Planned targets

### Browser / WebAssembly

Browser support should be built as a separate WebAssembly integration layer.

Guidance and implementation plan:

```text
docs/browser-wasm.md
```

Planned work:

- define an Emscripten/WASM build profile;
- identify which C ABI functions are safe and useful for browser usage;
- add a JavaScript or TypeScript adapter example;
- document asset/data loading constraints for browser builds;
- document memory ownership and string handling across the JS/WASM boundary;
- add a smoke-test browser example when the ABI is stable enough.

Browser support should not make the core depend on browser APIs.

### Other engines and languages

Future adapters may target:

- Godot;
- Unreal;
- custom engines;
- scripting or tooling languages;
- editor and content-pipeline tools.

These integrations should normally build on the C ABI or another intentionally stable foreign-function boundary.

---

## Roadmap alignment

### v1.x

Before `v2.0.0`, integration work should focus on making the core easier to embed:

- keep the C++ API clean;
- keep the local Action Bridge transport-agnostic and clearly separated from networking/auth/multiplayer layers;
- keep the C ABI safe and small;
- document the C ABI expansion plan;
- document C# / Unity integration through P/Invoke;
- document the browser/WebAssembly integration path before adding a full adapter;
- identify missing ABI functions needed by real Unity, browser and tool users;
- avoid engine-specific coupling inside the native core.

### v2.x and v3.x

After `v2.0.0`, deeper mechanics should be exposed through stable integration boundaries where possible. The priority is not only more bindings, but bindings that expose useful, connected simulation systems.

Likely focus:

- richer C ABI coverage;
- stable managed wrapper design;
- browser/WASM adapter design;
- validation and diagnostics usable from tools and editors;
- persistence and replay APIs suitable for server and tool workflows;
- safer data loading and content iteration for external environments.

### v4.0.0 and beyond

From `v4.0.0` onward, integration layers should make the deeper simulation core useful for large projects, MMO-like simulations and complex worlds without sacrificing the headless design.

---

## Non-goals

City Life Core should not become:

- a renderer;
- a UI framework;
- a Unity-only SDK;
- a browser-only SDK;
- a networking framework;
- an engine-specific gameplay framework;
- an account, auth, matchmaking or multiplayer service.

The core should remain a portable simulation SDK with clear integration boundaries.
