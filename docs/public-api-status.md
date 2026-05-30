# Public API Status

Version: **1.0.0**

This document classifies the installed API surface for the 1.0.0 line and the current v1.x integration planning work.

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
- local Action Bridge APIs for transport-agnostic external action dispatch.

Source compatibility is prioritized for the 1.x line where practical. C++ binary ABI stability is not the primary compatibility contract.

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
- read-only world event inspection.

The C ABI is intentionally small today. It does not yet expose the full runtime, registry, validation, persistence, economy, faction, contract or Action Bridge systems.

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
```

Status:

- `examples/find_package_consumer/` demonstrates installed C++ package usage.
- `examples/c_abi_consumer/` demonstrates minimal C ABI usage.
- `examples/csharp_unity/` demonstrates initial C# / Unity P/Invoke usage.
- `examples/action_bridge.cpp` demonstrates local C++ Action Bridge dispatch.

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
- Prefer rebuilding consumers against the installed SDK package.

---

## Related documents

- [Public API](public-api.md)
- [Action Bridge](action-bridge.md)
- [C ABI](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Integration targets](integration-targets.md)
- [Compatibility](compatibility.md)
- [Build and linking policy](build-and-linking-policy.md)
