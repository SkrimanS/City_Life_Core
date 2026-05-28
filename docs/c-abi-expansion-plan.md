# C ABI Expansion Plan

Status: **planning / v1.x integration foundation**

The C ABI is the safest long-term boundary for C, C#, Unity, WebAssembly, scripting languages and other non-C++ integration layers.

City Life Core should keep the C++ API as the richest native API while growing the C ABI carefully and only where the exposed behavior is stable enough.

---

## Why the C ABI matters

The public C++ API is source-first. It is not intended to provide a stable binary ABI for foreign-language bindings.

The C ABI provides a smaller and more controlled boundary that can be consumed by:

- C applications;
- C# / Unity through P/Invoke;
- WebAssembly / JavaScript adapters;
- other engines or languages that can call C-compatible functions;
- tools and editors that need a stable native boundary.

The C ABI should remain boring, explicit and easy to bind.

---

## Current C ABI surface

The current C ABI exposes:

- version helpers;
- C interface version;
- tick/time conversion helpers;
- opaque `clc_world` create/destroy;
- basic world state access;
- simple world tick advancement;
- read-only world event count and indexed event access.

Header:

```c
#include "clc/c/CityLifeCoreC.h"
```

Current C consumer example:

```text
examples/c_abi_consumer/
```

Initial C# wrapper example:

```text
examples/csharp_unity/
```

---

## Design rules

### Keep handles opaque

C ABI consumers should not see C++ classes, STL containers, templates, exceptions or ownership internals.

Use opaque handles such as:

```c
typedef struct clc_world clc_world;
```

Future handles may include:

```c
typedef struct clc_runtime clc_runtime;
typedef struct clc_registry clc_registry;
typedef struct clc_save_buffer clc_save_buffer;
typedef struct clc_validation_result clc_validation_result;
```

Only add a handle when its lifecycle can be documented clearly.

### Keep memory ownership explicit

Every native allocation visible to C ABI consumers should have a matching destroy/free function.

Possible future pattern:

```c
clc_runtime* clc_runtime_create_c(void);
void clc_runtime_destroy_c(clc_runtime* runtime);
```

Returned string pointers should either:

- be owned by the native object and documented as borrowed; or
- be copied into a caller-provided buffer; or
- be returned through an owned string object with an explicit free function.

Do not add ambiguous ownership.

### Avoid callbacks early

Callbacks are useful but complicate C#, Unity, WebAssembly and lifetime rules.

Prefer polling, indexed reads, result handles and explicit query functions before adding callbacks.

### Avoid C++ exceptions across the ABI

C ABI functions should return status codes, null handles or explicit result objects. Exceptions must not cross the ABI boundary.

### Prefer stable data shapes

Bindings become painful when data structures change often. Expose only stable fields and use versioned structs where needed.

---

## Expansion phases

### Phase 1 - Stability and binding safety

Goal: make the current minimal C ABI safer and easier to bind.

Possible work:

- document null behavior for every C ABI function;
- document string lifetime rules;
- define common status/error conventions;
- add explicit ABI smoke tests for invalid handles and invalid indexes;
- keep C#, Unity and future WebAssembly wrappers aligned with the same function set.

Expected result: the existing minimal ABI is clearly documented and safe to wrap.

### Phase 2 - Validation and diagnostics surface

Goal: let tools and external runtimes inspect errors without using the C++ API.

Possible work:

- expose validation result handles;
- expose error count and indexed error messages;
- expose warning count and indexed warning messages;
- expose diagnostic codes in addition to strings;
- make validation output usable from Unity editor tools and browser tools.

Expected result: non-C++ users can understand why data or runtime state is invalid.

### Phase 3 - Data registry surface

Goal: allow external tools and game integrations to load or inspect game definitions.

Possible work:

- create/destroy registry handles;
- load data packs from strings or byte buffers;
- validate registry contents;
- query resources, buildings, professions and settlements by index/id;
- avoid exposing STL containers or internal parser objects.

Expected result: Unity/C#/WASM/tool users can load and validate content through a stable API.

### Phase 4 - Runtime scenario surface

Goal: expose useful runtime operations beyond the minimal world smoke test.

Possible work:

- create/destroy runtime handles;
- create runtime from a registry;
- advance runtime by ticks;
- query runtime time;
- query settlements, resources, storage, routes and caravans through stable indexed APIs;
- expose basic runtime events and diagnostics.

Expected result: non-C++ users can run meaningful simulation loops.

### Phase 5 - Economy, factions and contracts surface

Goal: expose connected gameplay systems after their native APIs are stable enough.

Possible work:

- query and mutate wallets through safe functions;
- inspect ledgers;
- query markets and prices;
- inspect faction reputation and ownership;
- create and fulfill delivery contracts;
- inspect contract status and failure reasons.

Expected result: Unity/C#/browser integrations can use core gameplay systems instead of only smoke-test world handles.

### Phase 6 - Persistence and replay surface

Goal: support saved games, deterministic replay and server/tool workflows from foreign-language layers.

Possible work:

- save runtime state to caller-owned buffers;
- load runtime state from byte buffers or strings;
- expose migration results;
- expose replay comparison results;
- expose deterministic mismatch diagnostics;
- document buffer ownership and versioning rules.

Expected result: external integrations can use persistence and replay without relying on C++ internals.

---

## C# / Unity implications

C# / Unity support should follow the C ABI, not bypass it.

For each new C ABI phase, update:

```text
examples/csharp_unity/CityLifeCoreNative.cs
docs/csharp-unity.md
```

Avoid making Unity the owner of the native design. Unity is one consumer of the ABI, not the core architecture.

---

## Browser / WebAssembly implications

Browser support should also follow the C ABI where practical.

For each new C ABI phase, evaluate:

- whether the functions can be exported cleanly to WebAssembly;
- whether strings and buffers have clear JS/WASM ownership rules;
- whether the function shape is friendly to JavaScript or TypeScript adapters;
- whether the feature needs async data loading at the adapter layer.

Related document:

```text
docs/browser-wasm.md
```

---

## Versioning policy

The C ABI has its own interface version exposed by:

```c
uint32_t clc_c_interface_version_c(void);
```

Increase the C interface version when the ABI surface changes in a way that consumers should detect.

Do not treat internal minor milestone names such as `v1.1.0` or `v1.2.0` as public ABI release tags. Public Git tags and GitHub Releases remain limited to the major public milestones described in `versioning.md`.

---

## Non-goals

The C ABI should not expose:

- C++ templates;
- STL containers;
- exceptions;
- raw internal pointers;
- mutable access to internal containers;
- engine-specific types;
- Unity-specific or browser-specific APIs.

The goal is a stable bridge, not a second implementation of the C++ API.
