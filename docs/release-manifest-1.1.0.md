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

- `include/clc/c/CityLifeCoreC.h`
- `src/clc/c/CityLifeCoreC.cpp`
- `tests/c_abi_smoke_tests.c`
- `examples/c_abi_consumer/main.c`
- `examples/csharp_unity/CityLifeCoreNative.cs`
- `examples/csharp_unity/CityLifeSmokeTest.cs`
- `examples/csharp_unity/README.md`

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
- `examples/csharp_unity/README.md`

Expected managed wrapper surface includes:

- version helpers;
- C ABI compatibility helpers:
  - `RequiredCInterfaceVersion`;
  - `IsCInterfaceCompatible`;
  - `EnsureCompatibleCInterface()`;
- tick conversion helpers;
- `CityLifeWorld.Create`;
- `CityLifeWorld.Dispose`;
- throwing advancement helpers:
  - `CityLifeWorld.Advance`;
  - `CityLifeWorld.AdvanceSeconds`;
  - `CityLifeWorld.AdvanceMinutes`;
  - `CityLifeWorld.AdvanceHours`;
  - `CityLifeWorld.AdvanceDays`;
- non-throwing advancement helpers:
  - `CityLifeWorld.TryAdvance`;
  - `CityLifeWorld.TryAdvanceSeconds`;
  - `CityLifeWorld.TryAdvanceMinutes`;
  - `CityLifeWorld.TryAdvanceHours`;
  - `CityLifeWorld.TryAdvanceDays`;
- read-only event access.

The Unity smoke test should log actual and required C ABI versions before creating a native world and use `TryAdvanceMinutes` for the gameplay-style advance path.

This is not a full Unity package.

---

## Browser / WebAssembly status

Browser/WebAssembly support is documented as planned only.

No WebAssembly runtime, JavaScript adapter or browser game is included in this milestone.

---

## Installed package contents

Installed examples should include C# files because `CMakeLists.txt` installs `*.cs` files under examples.

Expected integration examples:

- `examples/find_package_consumer/`
- `examples/c_abi_consumer/`
- `examples/csharp_unity/`

---

## Required local validation

Before merge to `main`, run:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Recommended affected-example checks:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/path/to/installed/sdk
cmake --build build-c-abi-consumer
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
