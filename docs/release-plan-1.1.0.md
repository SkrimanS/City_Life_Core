# Release Plan 1.1.0

Status: **internal development milestone / not a public GitHub Release**

Branch: `v1.1.0`

City Life Core `v1.1.0` is the Integration Foundation Update. It prepares the SDK for C# / Unity consumers and future Browser/WebAssembly work through a safer and clearer C ABI boundary.

`v1.1.0` is an internal development branch. Public Git tags and GitHub Releases remain planned only for major public milestones such as `v1.0.0`, `v2.0.0`, `v3.0.0` and `v4.0.0`.

---

## Goals

- Improve the C ABI foundation for non-C++ consumers.
- Provide initial C# / Unity wrapper and smoke-test examples.
- Keep Unity-specific code outside the native C++ core.
- Keep Browser/WebAssembly support as documented planning only.
- Clearly separate implemented, initial and planned integration surfaces.
- Keep `main` unchanged until the branch is explicitly approved for merge.

---

## Scope

### Code and examples

- C ABI safety and smoke-test improvements.
- C ABI world time-advance helpers.
- C ABI consumer example updates.
- C# / Unity P/Invoke wrapper example.
- Unity-style smoke-test component.
- Installed example inclusion for C# files.

### Documentation and release gate

- C ABI documentation updates.
- C ABI expansion plan.
- C# / Unity integration guide.
- Browser/WASM planning guide.
- Integration targets and validation docs.
- Public API, compatibility, migration and build/linking updates.
- Release notes and release manifest for this internal milestone.

---

## Non-goals

Do not add:

- a full Unity package;
- a full managed C# SDK;
- a WebAssembly runtime;
- a browser game or browser framework;
- multiplayer;
- MMO systems;
- networking;
- UI;
- renderer;
- unrelated refactors.

---

## Required validation before merge to main

Local validation should include:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Recommended extra checks:

```bash
cmake -S examples/find_package_consumer -B build-find-package-consumer -DCMAKE_PREFIX_PATH=/path/to/installed/sdk
cmake --build build-find-package-consumer

cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/path/to/installed/sdk
cmake --build build-c-abi-consumer
```

For Unity/C#, verify that the native shared library can be loaded by Unity and that the smoke-test component can create, advance and dispose a world.

---

## Merge criteria

- C ABI tests pass.
- C++ tests pass.
- C ABI consumer example builds.
- C# wrapper is syntactically reviewable and aligned with the C ABI header.
- Unity smoke example uses the managed wrapper instead of private native details.
- Documentation clearly marks Browser/WASM as planned.
- Changelog is updated.
- Release notes and manifest are ready.
- No unrelated refactor is included.
