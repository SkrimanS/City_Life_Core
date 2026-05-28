# City Life Core Documentation

Version: **1.0.0**  
Status: **released / v1.0.0**

This directory contains developer-facing documentation for integrating City Life Core into a C++ game, server, backend service, editor, or simulation tool.

Russian documentation starts at [`ru/README.md`](ru/README.md).

---

## Start here

| Document | Purpose |
| --- | --- |
| [`core-concepts.md`](core-concepts.md) | Main systems: registry, runtime, ticks, settlements, caravans, economy, contracts, events and persistence. |
| [`architecture.md`](architecture.md) | Architecture overview for core layers, public C++ API, C ABI, integration layers and non-goals. |
| [`public-api.md`](public-api.md) | Public SDK API, recommended include, validation model, workflows and usage rules. |
| [`sdk-structure.md`](sdk-structure.md) | Repository layout, installed SDK layout, CMake options, integration modes and modules. |
| [`game-profiles.md`](game-profiles.md) | Recommended integration profiles for native games, Unity/C#, Browser/WASM, servers, MMO-like systems and tools. |
| [`packaging.md`](packaging.md) | Build, install and consume the SDK through CMake. |
| [`cmake-package.md`](cmake-package.md) | Exported CMake target and installed package directory variables. |
| [`sdk-zip-package.md`](sdk-zip-package.md) | Build and consume a CPack ZIP SDK archive. |
| [`build-and-linking-policy.md`](build-and-linking-policy.md) | Static/shared, source-first and binary compatibility policy. |

---

## API and compatibility

| Document | Purpose |
| --- | --- |
| [`public-api-status.md`](public-api-status.md) | Header-by-header usage classification. |
| [`c-abi.md`](c-abi.md) | Minimal C interface for version/time utilities, an opaque world handle and read-only world event access. |
| [`c-abi-expansion-plan.md`](c-abi-expansion-plan.md) | Staged plan for expanding the C ABI for Unity/C#, browser/WASM, tools and other foreign-language integrations. |
| [`integration-targets.md`](integration-targets.md) | Supported, initial and planned integration targets, including C++, C ABI, Unity/C#, browser/WASM, servers and tools. |
| [`integration-validation.md`](integration-validation.md) | Checklist for validating C++ package, C ABI, Unity/C#, SDK ZIP and future Browser/WASM integration changes. |
| [`csharp-unity.md`](csharp-unity.md) | C# and Unity integration guide using the C ABI and P/Invoke. |
| [`browser-wasm.md`](browser-wasm.md) | Planned browser and WebAssembly integration path for web games, web tools and demos. |
| [`compatibility.md`](compatibility.md) | Time, events, save/load, validation and source/binary compatibility rules. |
| [`migration.md`](migration.md) | Instructions for updating older integrations. |

---

## Roadmap and release documents

| Document | Purpose |
| --- | --- |
| [`roadmap.md`](roadmap.md) | Project direction, internal milestone plan and v2/v3/v4 roadmap. |
| [`versioning.md`](versioning.md) | Versioning, branch, tag and release policy. |
| [`release-plan-1.1.0.md`](release-plan-1.1.0.md) | Internal v1.1.0 Integration Foundation release plan and merge gate. |
| [`release-notes-1.1.0.md`](release-notes-1.1.0.md) | Internal v1.1.0 Integration Foundation release notes. |
| [`release-manifest-1.1.0.md`](release-manifest-1.1.0.md) | Internal v1.1.0 Integration Foundation manifest and validation expectations. |
| [`release-notes-1.0.0.md`](release-notes-1.0.0.md) | Final 1.0.0 release notes. |
| [`release-manifest-1.0.0.md`](release-manifest-1.0.0.md) | Final 1.0.0 release manifest and reviewed local validation data. |
| [`readiness-status.md`](readiness-status.md) | Subsystem readiness estimate for the 1.0.0 line. |
| [`verifying-releases.md`](verifying-releases.md) | Release verification guide. |
| [`ci-artifact-review.md`](ci-artifact-review.md) | Manual CI, benchmark artifact and SDK ZIP artifact review guide. |
| [`release-manifest-template.md`](release-manifest-template.md) | Template for future release manifests. |
| [`protection-strategy.md`](protection-strategy.md) | Project protection and distribution strategy. |

GitHub Actions and Linux GCC/Clang validation for `v1.0.0` are tracked as post-release verification in issue #41. Windows/MSVC local validation was accepted as the release gate for this release.

Normal SDK users usually start with `core-concepts.md`, `architecture.md`, `public-api.md`, `sdk-structure.md`, `game-profiles.md` and `packaging.md`. Unity/C# users should start with `integration-targets.md`, `game-profiles.md` and `csharp-unity.md` after reviewing the C ABI documentation. Browser/WebAssembly users should start with `integration-targets.md`, `game-profiles.md` and `browser-wasm.md`. Integration maintainers should also use `integration-validation.md` before merging integration-related changes.

---

## Quick CMake integration

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

Recommended C++ include:

```cpp
#include "clc/CityLifeCore.hpp"
```

Minimal C interface include:

```c
#include "clc/c/CityLifeCoreC.h"
```

---

## Main runtime path

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
if (!bootstrap.ok()) {
    return;
}

auto& runtime = bootstrap.runtime;
clc::sim::advance_runtime_ticks(runtime, clc::minutes_to_ticks(5));
```
