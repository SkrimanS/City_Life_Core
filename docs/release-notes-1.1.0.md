# Release Notes 1.1.0

Status: **internal development milestone / not a public GitHub Release**

`v1.1.0` is the Integration Foundation Update for City Life Core.

This milestone improves the SDK boundary for non-C++ consumers, especially C# / Unity, while keeping the native core headless and engine-agnostic.

---

## Highlights

- Added initial C# / Unity P/Invoke wrapper examples over the C ABI.
- Added C# / Unity C ABI compatibility guard helpers so the wrapper can detect native libraries older than the required C interface version.
- Added non-throwing C# / Unity native-library C ABI probing helpers for detecting missing or incompatible native plug-ins before gameplay calls.
- Added hardened non-throwing C# / Unity `TryCreate`, `TryAdvance*` and `TryGetEvent` helpers for gameplay-oriented flows.
- Added shared-library build friendliness for native plug-in scenarios through explicit `BUILD_SHARED_LIBS`, position-independent code and Windows symbol export settings.
- Added Unity-style smoke-test component for creating, advancing and disposing a world.
- Added Browser/WebAssembly planning documentation without claiming implemented WASM support.
- Added integration target and game-profile guidance for native C++, C ABI, Unity/C#, Browser/WASM, servers, MMO-like systems and tools.
- Expanded C ABI documentation and planning around staged foreign-language support.
- Added C ABI world time-advance helpers for seconds, minutes, hours and days.
- Bumped C interface version from `3` to `4` for the new C ABI helper surface.
- Hardened C ABI world destruction, accessors and advance helpers so exceptions are caught at the C boundary.
- Expanded C ABI smoke coverage for null handles, invalid indexes, extreme invalid indexes, zero-duration failures and time-advance helpers.

---

## Added

- `examples/csharp_unity/CityLifeCoreNative.cs`.
- `examples/csharp_unity/CityLifeSmokeTest.cs`.
- `examples/csharp_unity/README.md`.
- `docs/csharp-unity.md`.
- `docs/browser-wasm.md`.
- `docs/c-abi-expansion-plan.md`.
- `docs/integration-targets.md`.
- `docs/integration-validation.md`.
- `docs/game-profiles.md`.
- C ABI functions:
  - `clc_world_advance_seconds_c`;
  - `clc_world_advance_minutes_c`;
  - `clc_world_advance_hours_c`;
  - `clc_world_advance_days_c`.
- C# / Unity wrapper helpers:
  - `RequiredCInterfaceVersion`;
  - `IsCInterfaceCompatible`;
  - `TryGetCInterfaceVersion`;
  - `TryCheckCInterfaceCompatibility`;
  - `EnsureCompatibleCInterface()`;
  - `TryCreate`;
  - `TryAdvance`;
  - `TryAdvanceSeconds`;
  - `TryAdvanceMinutes`;
  - `TryAdvanceHours`;
  - `TryAdvanceDays`;
  - `TryGetEvent`.

---

## Changed

- Hardened C# / Unity non-throwing wrapper paths so `TryAdvance*`, `TryGetEvent` and native world destruction catch interop exceptions and return safe results where applicable.
- Updated CMake build configuration to make shared native plug-in builds easier through `BUILD_SHARED_LIBS`, `POSITION_INDEPENDENT_CODE` and `WINDOWS_EXPORT_ALL_SYMBOLS`.
- Updated `clc_c_interface_version_c()` to return `4`.
- Updated C ABI smoke tests for the expanded C ABI surface.
- Updated the C ABI consumer example to use time-based world advance helpers.
- Updated the C# / Unity wrapper to expose managed `AdvanceSeconds`, `AdvanceMinutes`, `AdvanceHours`, `AdvanceDays`, soft C ABI probing and non-throwing `TryCreate`, `TryAdvance*` and `TryGetEvent` methods.
- Updated the Unity smoke test to use soft native-library C ABI probing before calling other native functions, then use `TryCreate`, `TryAdvanceMinutes` and `TryGetEvent`.
- Updated CMake install rules so C# example sources are included with installed examples.
- Updated public API, compatibility, migration, packaging, build/linking, readiness and release verification documentation to reflect the integration foundation.

---

## Compatibility

- This is not a public GitHub Release.
- No public tag should be created for this internal milestone unless release policy changes.
- The C++ project version remains `1.0.0` until the next approved public release policy decision.
- C ABI version detection should use `clc_c_interface_version_c()`.
- The C# / Unity wrapper requires C interface version `4` or newer.
- The C ABI is still intentionally minimal and does not expose the full runtime, registry, persistence, economy, faction or contract systems.
- Unity/C# support is initial wrapper/example support, not a full Unity package.
- Browser/WebAssembly support remains planned, not implemented.

---

## Validation status

Before merging this branch to `main`, run local validation:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Also verify affected examples, especially:

```text
examples/c_abi_consumer/
examples/csharp_unity/
```

---

## Non-goals

`v1.1.0` does not add:

- full Unity package;
- full managed SDK;
- WebAssembly runtime;
- browser game;
- multiplayer;
- networking;
- MMO systems;
- UI;
- renderer.
