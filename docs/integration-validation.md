# Integration Validation

Status: **active checklist for v1.x integration work**

This document describes how to validate City Life Core integration surfaces before merging integration-related changes.

It is not a replacement for automated tests. It is a checklist for reviewing SDK packaging, C ABI consumers, C# / Unity wrapper examples and future Browser/WebAssembly work.

---

## Validation goals

Integration validation should confirm that:

- the native C++ SDK still builds and installs;
- installed CMake consumers can still use `CityLifeCore::core`;
- the C ABI remains callable from a C consumer;
- C# / Unity examples remain aligned with the C ABI header;
- SDK ZIP packages include the expected docs, examples and data;
- future Browser/WebAssembly work does not add browser-specific dependencies to the native core;
- documentation and changelog entries are updated with any public-facing changes.

---

## Branch and release-policy checks

Before validating integration work, confirm:

- the work is on the correct internal stage branch, for example `v1.1.0`;
- `main` remains aligned with the latest public release until the stage is ready to merge;
- no public tag is created for an internal minor milestone;
- no GitHub Release is created except for the planned public milestones: `v1.0.0`, `v2.0.0`, `v3.0.0`, `v4.0.0`.

---

## Native C++ SDK validation

Recommended smoke commands:

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

This validates the normal source-tree flow.

---

## Installed CMake package validation

Recommended install flow:

```bash
cmake -S . -B build-sdk -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF -DCMAKE_INSTALL_PREFIX=/tmp/city-life-core-sdk
cmake --build build-sdk --config Release
cmake --install build-sdk --config Release
```

Then build the installed package consumer:

```bash
cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-consumer --config Release
```

Expected result: the external consumer finds `CityLifeCore::core` and links successfully.

---

## C ABI validation

The C ABI is the stable boundary for C, C# / Unity, Browser/WASM and other future bindings.

Recommended installed C ABI consumer check:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/tmp/city-life-core-sdk
cmake --build build-c-abi-consumer --config Release
```

Review checklist:

- `include/clc/c/CityLifeCoreC.h` contains the expected declarations;
- C ABI functions do not expose C++ templates, STL containers or exceptions;
- new handles have clear create/destroy functions;
- returned strings have documented lifetime rules;
- the C interface version is updated when needed;
- `docs/c-abi.md` and `docs/c-abi-expansion-plan.md` are updated.

---

## C# / Unity validation

Current Unity/C# support is an initial wrapper over the C ABI.

Review checklist:

- `examples/csharp_unity/CityLifeCoreNative.cs` matches the C ABI function names and signatures;
- `examples/csharp_unity/CityLifeSmokeTest.cs` remains a simple optional smoke test;
- the wrapper does not call C++ API directly;
- the wrapper owns native handles through managed classes such as `CityLifeWorld`;
- `Dispose()` is available for native handle cleanup;
- Unity documentation explains native plug-in placement and troubleshooting.

Suggested manual Unity smoke test:

1. Build City Life Core as a shared library for the target platform.
2. Copy the native library into the Unity project's `Assets/Plugins` tree.
3. Copy `CityLifeCoreNative.cs` and `CityLifeSmokeTest.cs` into the Unity project.
4. Attach `CityLifeSmokeTest` to an empty GameObject.
5. Enter Play Mode.
6. Confirm the Unity Console prints the native version, C ABI version, tick data and world events.

This step is manual for now. It should not be treated as full Unity package validation.

---

## Browser / WebAssembly validation

Browser/WebAssembly support is planned, not implemented.

For now, validation means checking that changes do not make browser support harder:

- the native core remains headless;
- no browser-specific dependency enters the core;
- C ABI additions are suitable for WebAssembly export where practical;
- strings and buffers have clear ownership rules;
- `docs/browser-wasm.md` stays aligned with any implementation progress.

When WebAssembly work begins, add a real smoke test for:

- Emscripten configuration;
- exported C ABI functions;
- JavaScript or TypeScript adapter loading;
- world creation and tick advancement;
- event reading from JS/WASM.

---

## SDK ZIP validation

Build ZIP package:

```bash
cmake -S . -B build-sdk-zip -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk-zip --config Release
cpack --config build-sdk-zip/CPackConfig.cmake -G ZIP
cmake -E sha256sum city-life-core-sdk-*.zip > SHA256SUMS.txt
```

Review expected contents:

- public headers;
- native library artifacts for the chosen build configuration;
- CMake package config files;
- documentation;
- examples;
- example data packs;
- C ABI consumer example;
- C# / Unity example source files.

For Unity/native plug-in scenarios, also test a shared-library package configuration:

```bash
cmake -S . -B build-sdk-zip-shared -DBUILD_SHARED_LIBS=ON -DCLC_BUILD_TESTS=OFF -DCLC_BUILD_EXAMPLES=OFF -DCLC_BUILD_TOOLS=OFF
cmake --build build-sdk-zip-shared --config Release
cpack --config build-sdk-zip-shared/CPackConfig.cmake -G ZIP
```

---

## Documentation checks

For integration-related changes, review these documents as needed:

```text
docs/integration-targets.md
docs/c-abi.md
docs/c-abi-expansion-plan.md
docs/csharp-unity.md
docs/browser-wasm.md
docs/packaging.md
docs/sdk-structure.md
docs/sdk-zip-package.md
CHANGELOG.md
```

Also check:

- root `README.md` if public entry points changed;
- `docs/README.md` if document discoverability changed;
- `docs/ru/README.md` if the Russian entry point needs a new link.

---

## Pull request checklist alignment

The pull request template should be used for integration changes.

PRs should identify:

- target branch and internal milestone;
- integration boundary;
- compatibility impact;
- validation commands or manual checks;
- documentation and changelog updates.

If a PR changes integration behavior but does not update validation notes, request a follow-up before merge.
