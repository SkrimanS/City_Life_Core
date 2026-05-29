# Release Manifest 1.1.0

Status: **internal development milestone / not a public GitHub Release**

Branch: `v1.1.0`

This manifest summarizes the intended contents and validation expectations for the City Life Core `v1.1.0` Integration Foundation Update.

---

## Release identity

- Project: City Life Core
- Internal milestone: `v1.1.0`
- Public release status: not a public GitHub Release
- Public tag status: no public tag planned
- Base public release line: `v1.0.0`
- Primary language: C++20
- Build system: CMake

Public Git tags and GitHub Releases remain planned only for major public milestones: `v1.0.0`, `v2.0.0`, `v3.0.0` and `v4.0.0`.

---

## Source changes

Primary source changes:

- `.github/workflows/ci.yml`
- `CMakeLists.txt`
- `include/clc/c/CityLifeCoreC.h`
- `src/clc/c/CityLifeCoreC.cpp`
- `tests/c_abi_smoke_tests.c`
- `examples/c_abi_consumer/main.c`
- `examples/csharp_unity/CityLifeCoreNative.cs`
- `examples/csharp_unity/CityLifeSmokeTest.cs`
- `examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj`
- `examples/csharp_unity/README.md`
- `scripts/validate_csharp_wrapper.sh`
- `scripts/validate_csharp_wrapper.ps1`

---

## CI surface

CI should run for:

- `main`;
- `develop`;
- `bootstrap/**`;
- `feature/**`;
- legacy `v/**` branches;
- direct milestone branches such as `v1.1.0`, `v1.2.0` and `v2.1.0` through `v*`.

The CI workflow validates:

- normal configure/build/test;
- benchmark execution;
- installed C++ consumer;
- installed C ABI consumer;
- SDK ZIP generation;
- unpacked SDK ZIP C++ consumer;
- unpacked SDK ZIP C ABI consumer;
- shared-core build with `BUILD_SHARED_LIBS=ON`.

The C# wrapper can be validated locally with:

```bash
scripts/validate_csharp_wrapper.sh
```

or on Windows:

```powershell
scripts/validate_csharp_wrapper.ps1
```

---

## Build surface

Shared native plug-in oriented builds should use:

```bash
cmake -S . -B build-shared -DBUILD_SHARED_LIBS=ON
cmake --build build-shared
```

Build configuration includes:

- explicit `BUILD_SHARED_LIBS` option;
- `POSITION_INDEPENDENT_CODE` for the core target;
- `WINDOWS_EXPORT_ALL_SYMBOLS` for friendlier Windows DLL exports.

---

## C ABI surface

C interface version: `4`

Added C ABI functions:

- `clc_world_advance_seconds_c`
- `clc_world_advance_minutes_c`
- `clc_world_advance_hours_c`
- `clc_world_advance_days_c`

C ABI hardening:

- null world handles return safe fallback values;
- invalid event indexes return safe fallback values;
- extreme invalid event indexes return safe fallback values;
- destroy-null remains safe;
- world destruction, accessors and advance helpers catch exceptions at the C boundary.

---

## C# / Unity contents

Expected C# / Unity example files:

- `examples/csharp_unity/CityLifeCoreNative.cs`
- `examples/csharp_unity/CityLifeSmokeTest.cs`
- `examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj`
- `examples/csharp_unity/README.md`

The example README should cover:

- shared native library build commands;
- expected native library names on Windows, Linux and macOS;
- wrapper compile validation with `dotnet build` or validation scripts;
- Unity `Assets/Plugins` and `Assets/Scripts` layout;
- soft C ABI probing before gameplay calls;
- non-throwing `TryCreate`, `TryAdvanceMinutes` and `TryGetEvent` usage;
- the current minimal C ABI scope and future expansion boundary.

Expected managed wrapper surface includes:

- version helpers;
- C ABI compatibility helpers:
  - `RequiredCInterfaceVersion`;
  - `IsCInterfaceCompatible`;
  - `TryGetCInterfaceVersion`;
  - `TryCheckCInterfaceCompatibility`;
  - `EnsureCompatibleCInterface()`;
- tick conversion helpers;
- throwing helpers:
  - `CityLifeWorld.Create`;
  - `CityLifeWorld.Advance`;
  - `CityLifeWorld.AdvanceSeconds`;
  - `CityLifeWorld.AdvanceMinutes`;
  - `CityLifeWorld.AdvanceHours`;
  - `CityLifeWorld.AdvanceDays`;
  - `CityLifeWorld.GetEvent`;
- hardened non-throwing gameplay helpers:
  - `CityLifeWorld.TryCreate`;
  - `CityLifeWorld.TryAdvance`;
  - `CityLifeWorld.TryAdvanceSeconds`;
  - `CityLifeWorld.TryAdvanceMinutes`;
  - `CityLifeWorld.TryAdvanceHours`;
  - `CityLifeWorld.TryAdvanceDays`;
  - `CityLifeWorld.TryGetEvent`;
- safe native world destruction from `Dispose` and finalizer;
- read-only event access.

The Unity smoke test should use soft native-library C ABI probing before calling other native functions, then use `TryCreate`, `TryAdvanceMinutes` and `TryGetEvent` for gameplay-style non-throwing paths.

This is not a full Unity package.

---

## Browser / WebAssembly status

Browser/WebAssembly support is documented as planned only.

No WebAssembly runtime, JavaScript adapter or browser game is included in this milestone.

---

## Installed package contents

Installed examples should include C# files and C# project files because `CMakeLists.txt` installs both `*.cs` and `*.csproj` files under examples.

Expected integration examples:

- `examples/find_package_consumer/`
- `examples/c_abi_consumer/`
- `examples/csharp_unity/`

Expected C# compile validation files in installed examples:

- `examples/csharp_unity/CityLifeCoreNative.cs`
- `examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj`

---

## Required local validation

Before merge to `main`, run:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

The Unix manual release validation script also runs the shared native build path, C# wrapper compile validation and installed/ZIP C# compile-check project presence checks:

```bash
scripts/manual_release_validation.sh
```

Recommended affected-example checks:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/path/to/installed/sdk
cmake --build build-c-abi-consumer
```

For C# wrapper compile validation, run:

```bash
scripts/validate_csharp_wrapper.sh
```

or on Windows:

```powershell
scripts/validate_csharp_wrapper.ps1
```

For native plug-in oriented builds, also verify:

```bash
cmake -S . -B build-shared -DBUILD_SHARED_LIBS=ON
cmake --build build-shared
```

Unity smoke verification should be performed in Unity when a suitable native shared library is available for the target platform.

---

## Known limitations

- The C ABI is still minimal and does not expose full runtime, registry, validation, persistence, economy, factions or contracts.
- C# / Unity support is wrapper/example support only.
- Browser/WebAssembly support is not implemented.
- No networking, multiplayer or MMO systems are added.
- No UI or renderer is added.

---

## Related documents

- [Release plan 1.1.0](release-plan-1.1.0.md)
- [Release notes 1.1.0](release-notes-1.1.0.md)
- [C ABI](c-abi.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Integration validation](integration-validation.md)
