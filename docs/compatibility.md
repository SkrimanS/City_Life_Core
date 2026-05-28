# Compatibility Policy

Version: **1.0.0**

City Life Core 1.x prioritizes source compatibility for supported public headers and documented workflows.

Compatibility is intentionally split by boundary:

- public C++ API;
- C ABI;
- C# / Unity wrapper;
- future Browser/WebAssembly adapter;
- save/load and replay data;
- documentation and packaging workflows.

---

## Public C++ API compatibility

The public C++ API is source-first.

Expectations:

- public headers under `include/clc` should remain usable across compatible 1.x updates where practical;
- documented workflows should avoid unnecessary breaking changes;
- downstream projects should rebuild against the SDK version they consume;
- behavior changes should be documented in `CHANGELOG.md`, migration notes or release notes when relevant.

Not guaranteed by default:

- stable C++ binary ABI across compilers, standard libraries or build configurations;
- compatibility with private implementation details under `src`;
- compatibility for undocumented internal headers or private helper behavior.

---

## C ABI compatibility

The C ABI is the intended stable foreign-function boundary for C, C# / Unity, Browser/WASM and future language/engine bindings.

Current C ABI scope:

- version helpers;
- C interface version;
- tick/time helpers;
- opaque `clc_world` handle;
- basic world state access;
- simple tick advancement;
- read-only world event inspection.

Expectations:

- C ABI functions should avoid C++ exceptions crossing the boundary;
- C ABI handles should remain opaque;
- ownership rules should be explicit;
- returned string lifetimes should be documented;
- C ABI changes should update the C interface version when consumers need to detect them;
- C ABI changes should be reflected in `docs/c-abi.md` and `docs/c-abi-expansion-plan.md`.

The C ABI is not yet a full replacement for the C++ runtime API.

---

## C# / Unity compatibility

C# / Unity support is currently an initial wrapper over the C ABI.

Expectations:

- the C# wrapper should track the C ABI, not private C++ internals;
- native handles should remain hidden behind managed wrapper classes;
- `Dispose()` should be available for native resources;
- Unity examples should remain smoke-test oriented until the ABI is wider and more stable;
- Unity-specific code should stay outside the C++ core.

Not guaranteed yet:

- official Unity package layout;
- prebuilt native plug-ins per platform;
- full managed API coverage for runtime, registry, persistence, economy, factions or contracts;
- automated Unity CI validation.

---

## Browser / WebAssembly compatibility

Browser/WebAssembly support is planned, not implemented.

Expectations for future work:

- browser support should be built as a separate adapter layer;
- the native core should not depend on browser APIs;
- exported functions should prefer a C-compatible boundary where practical;
- JavaScript/WebAssembly memory ownership should be documented;
- adapter compatibility should be tracked separately from C++ source compatibility.

No browser compatibility guarantee exists until a real WebAssembly build and adapter are added.

---

## Save/load compatibility

Save/load compatibility must be treated carefully by downstream products.

Expectations:

- save/load behavior should be covered by tests for supported shapes;
- migration-sensitive changes should be documented;
- older supported data shapes should either load correctly or fail with clear validation errors;
- downstream products should validate save/load compatibility before upgrading production worlds.

Current 1.0.0 compatibility notes include:

- older world-state saves without explicit runtime `time` are accepted and synchronize runtime clock from saved `current_day`;
- older contract rows without `due_ticks` are accepted and derive `due_ticks` from `due_day`.

---

## Replay and deterministic behavior

Replay and deterministic behavior are compatibility-sensitive.

Expectations:

- tick semantics should remain explicit;
- deterministic reports and validation behavior should be documented when changed;
- event timestamps should remain clearly defined;
- replay mismatches should be diagnosable when replay support is used.

Changes to runtime order, event ordering, tick conversion or persistence can affect replay compatibility and should be documented.

---

## Data and validation compatibility

Data registry and validation behavior should remain predictable.

Expectations:

- validation errors should be clear and stable enough for tools where practical;
- changes to `.clcd` expectations should be documented;
- downstream data packs should be validated before production upgrades;
- future C ABI validation surfaces should expose stable diagnostic shapes where possible.

---

## Packaging compatibility

Installed package compatibility is based on CMake package usage.

Recommended consumer shape:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Expectations:

- consumers should use `CityLifeCore::core` instead of hard-coded library paths;
- installed headers, docs and examples should stay discoverable;
- SDK ZIP archives should preserve the install-layout assumptions;
- C# example sources may be included with examples, but they do not replace native plug-in libraries.

---

## Version and release compatibility

Public Git tags and GitHub Releases are planned only for:

- `v1.0.0`
- `v2.0.0`
- `v3.0.0`
- `v4.0.0`

Internal minor milestones such as `v1.1.0` and `v1.2.0` are development branches and planning labels, not public compatibility release tags.

---

## Not guaranteed by default

- Cross-compiler C++ binary compatibility.
- ABI stability for the full C++ API.
- Compatibility for internal implementation details under `src`.
- Stable Unity package layout.
- Browser/WebAssembly compatibility before a real adapter exists.
- Compatibility for undocumented behavior or private helper APIs.

---

## Related documents

- [Public API status](public-api-status.md)
- [Build and linking policy](build-and-linking-policy.md)
- [C ABI](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Integration targets](integration-targets.md)
- [Integration validation](integration-validation.md)
- [Migration](migration.md)
- [Versioning](versioning.md)
