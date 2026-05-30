# Migration Guide

Version: **1.0.0**

Use this guide when updating an older City Life Core integration to the current documentation, package layout and integration strategy.

City Life Core is now documented as an English-primary, headless native SDK with a source-first C++ API, a local Action Bridge, a minimal C ABI, initial Unity/C# guidance and planned Browser/WebAssembly support.

---

## Documentation path changes

Documentation filenames now use lower-kebab-case.

Examples:

| Old path | New path |
| --- | --- |
| `docs/CORE_CONCEPTS.md` | `docs/core-concepts.md` |
| `docs/PUBLIC_API.md` | `docs/public-api.md` |
| `docs/SDK_STRUCTURE.md` | `docs/sdk-structure.md` |
| `docs/RELEASE_NOTES_1.0.0.md` | `docs/release-notes-1.0.0.md` |

Russian documentation is grouped under:

```text
docs/ru/
```

Start with:

```text
docs/README.md
docs/ru/README.md
```

---

## CMake package migration

Installed consumers should use the exported CMake package instead of hard-coded include/library paths.

Recommended CMake shape:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

If you previously copied headers or libraries manually, migrate to the installed SDK layout or SDK ZIP layout where possible.

Related docs:

- [CMake package](cmake-package.md)
- [Packaging](packaging.md)
- [SDK ZIP package](sdk-zip-package.md)

---

## C++ include migration

For C++ consumers, prefer the umbrella include:

```cpp
#include "clc/CityLifeCore.hpp"
```

This is the recommended entry point for normal C++ integrations.

The public C++ API is source-first. Rebuild downstream projects against the SDK version they consume. Do not assume stable C++ binary ABI across compilers, standard libraries or build configurations.

---

## Action Bridge migration

If an older integration sends external commands directly into runtime internals, migrate those flows toward the local Action Bridge when the operation fits the supported action model.

Recommended C++ include:

```cpp
#include "clc/sim/ActionBridge.hpp"
```

or:

```cpp
#include "clc/CityLifeCore.hpp"
```

Use the Action Bridge for:

- local JSON action input;
- pre-mutation validation;
- rejected-action no-mutation behavior;
- stable status and error-code handling;
- produced event and diagnostic output.

Do not treat the Action Bridge as a network protocol. HTTP, WebSocket, accounts, auth, matchmaking, multiplayer, MMO and UI layers remain downstream product code.

Related docs:

- [Action Bridge](action-bridge.md)
- [Public API status](public-api-status.md)
- [Integration targets](integration-targets.md)

---

## Time and runtime migration

Prefer tick-based APIs for real-time, MMO-like and server-authoritative simulation flows.

Day-based helpers remain available for daily or turn-based workflows, but tick-based APIs are the preferred direction for non-turn-based games.

Review runtime usage if your integration depends on:

- route travel duration;
- caravan progress;
- contract deadlines;
- event timestamps;
- save/load state;
- replay comparison.

Compatibility notes:

- older world-state saves without explicit runtime `time` are accepted and synchronize runtime clock from saved `current_day`;
- older contract rows without `due_ticks` are accepted and derive `due_ticks` from `due_day`.

---

## C ABI migration

C and foreign-language integrations should use the C ABI instead of binding directly to the C++ API.

Header:

```c
#include "clc/c/CityLifeCoreC.h"
```

Current C ABI scope:

- version helpers;
- C interface version;
- tick/time helpers;
- opaque `clc_world` handle;
- basic world state access;
- simple tick advancement;
- read-only world event inspection.

If your integration currently calls C++ symbols from another language, migrate toward the C ABI.

The C ABI is intentionally minimal today. Wider runtime, registry, validation, economy, faction, contract, persistence and replay surfaces should be added only through the staged expansion plan.

Related docs:

- [C ABI](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [Integration targets](integration-targets.md)

---

## C# / Unity migration

Unity and C# integrations should not bind directly to the C++ API.

Recommended shape:

```text
Unity C# code
  -> CityLifeCoreNative.cs
  -> C ABI
  -> native City Life Core shared library
```

Use the initial wrapper examples:

```text
examples/csharp_unity/CityLifeCoreNative.cs
examples/csharp_unity/CityLifeWorldSafeAccess.cs
examples/csharp_unity/CityLifeNativeDiagnostics.cs
examples/csharp_unity/CityLifeSmokeTest.cs
```

Migration checklist:

1. Build City Life Core as a shared native library for the target platform.
2. Copy the native library into the Unity `Assets/Plugins` tree.
3. Copy the C# wrapper into the Unity project.
4. Attach the smoke-test script to an empty GameObject.
5. Confirm the Unity Console prints the native version, C ABI version, tick data and events.
6. Keep native handles inside managed wrapper classes and call `Dispose()` when done.

Current limitations:

- no official Unity package layout yet;
- no prebuilt native plug-ins yet;
- no full managed API for registry/runtime/economy/contracts yet;
- Unity support tracks the C ABI and expands as the C ABI expands.

Related docs:

- [C# and Unity integration](csharp-unity.md)
- [Game integration profiles](game-profiles.md)
- [Integration validation](integration-validation.md)

---

## Browser / WebAssembly migration

Browser/WebAssembly support is planned, not implemented.

Do not migrate browser users to a promised adapter yet. Instead, track the planned architecture:

```text
Browser app
  -> JavaScript or TypeScript adapter
  -> WebAssembly module
  -> C ABI / exported functions
  -> native C++ core
```

For now:

- keep browser-specific code outside the native core;
- avoid adding DOM, canvas, WebGL, WebGPU or browser storage dependencies to the core;
- design new C ABI functions with future WebAssembly export in mind where practical;
- document memory, string and buffer ownership when WebAssembly work begins.

Related docs:

- [Browser and WebAssembly integration](browser-wasm.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [Integration targets](integration-targets.md)

---

## Game profile migration

If an older integration assumed that City Life Core is only a C++ library, review the game profile document and choose the intended profile:

- native C++ game;
- native C++ server/tool;
- C consumer;
- Unity/C# client or tool;
- browser/WASM game or tool;
- server-authoritative game;
- MMO-like simulation;
- editor/balancing tool.

Each profile should use the correct integration boundary. Do not add engine-specific dependencies to the core to support a profile.

Related doc:

- [Game integration profiles](game-profiles.md)

---

## Save/load and replay migration

For integrations that ship persistent worlds or rely on deterministic replay:

- validate old saves against the new SDK version;
- confirm tick/time fields load as expected;
- compare replay outputs before and after migration;
- document any changed event ordering or timestamp assumptions;
- update migration notes if save shapes change.

Do not treat save/load and replay compatibility as automatic. Validate them explicitly for your product.

---

## Documentation and changelog migration

When migrating downstream docs or project references:

- update links to lower-kebab-case docs;
- link to `docs/action-bridge.md` when discussing local external action dispatch;
- link to `docs/integration-targets.md` when discussing platform support;
- link to `docs/game-profiles.md` when discussing game type support;
- link to `docs/c-abi-expansion-plan.md` when requesting new bindings;
- update `CHANGELOG.md` for public-facing changes.

---

## Migration checklist

- [ ] Update documentation links to lower-kebab-case paths.
- [ ] Use `find_package(CityLifeCore CONFIG REQUIRED)` for installed consumers.
- [ ] Link against `CityLifeCore::core`.
- [ ] Prefer `#include "clc/CityLifeCore.hpp"` for C++ consumers.
- [ ] Use the local Action Bridge for supported external action-style flows.
- [ ] Prefer tick-based APIs for server-authoritative or real-time simulation flows.
- [ ] Use the C ABI for C, C#, Unity, WebAssembly and other non-C++ integration layers.
- [ ] Do not bind foreign-language wrappers to private C++ internals.
- [ ] Build shared libraries for native plug-in scenarios such as Unity.
- [ ] Validate save/load and replay behavior before upgrading production data.
- [ ] Update docs and changelog for public-facing migration changes.
