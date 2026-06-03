# City Life Core Documentation

Version: **2.0.0**
Status: **v2.0.0 release candidate / stable multi-game SDK foundation**

This directory contains developer-facing documentation for integrating City Life Core into a C++ game, server, backend service, editor, or simulation tool.

Russian documentation starts at [`ru/README.md`](ru/README.md).

---

## Start here

| Document | Purpose |
| --- | --- |
| [`core-concepts.md`](core-concepts.md) | Main systems: registry, runtime, ticks, settlements, caravans, economy, contracts, events and persistence. |
| [`architecture.md`](architecture.md) | Architecture overview for core layers, public C++ API, C ABI, integration layers and non-goals. |
| [`public-api.md`](public-api.md) | Public SDK API, recommended include, validation model, workflows and usage rules. |
| [`data-packs.md`](data-packs.md) | `.clcd` data pack format, required fields, strict loader validation and diagnostics. |
| [`action-bridge.md`](action-bridge.md) | Local JSON action bridge for external tools, game layers and future server-authoritative adapters. |
| [`sdk-structure.md`](sdk-structure.md) | Repository layout, installed SDK layout, CMake options, integration modes and modules. |
| [`game-profiles.md`](game-profiles.md) | Recommended integration profiles plus catalog validation output, catalog markdown, adoption reports and checklist output for native games, Unity/C#, Browser/WASM, servers, MMO-like systems and tools. |
| [`platform-integration-layer.md`](platform-integration-layer.md) | v1.5.0 platform and adapter boundary guidance plus deep systems foundation API notes for Unity/C#, Browser/WASM, server, editor and SDK embedding work. |
| [`server-authoritative-mmo.md`](server-authoritative-mmo.md) | v1.6.0 server-authoritative and MMO-like foundation: action envelopes, host-owned session identity, shard descriptors, audit records and sequence validation. |
| [`economy-depth.md`](economy-depth.md) | v1.7.0 economy, factions and contracts depth: contract economy assessment, resource flow plans, portfolio summaries and extension points. |
| [`persistence-replay-migration.md`](persistence-replay-migration.md) | v1.8.0 persistence, replay and migration: save-format review, legacy migration, replay diagnostics and checkpoint guidance. |
| [`scale-performance.md`](scale-performance.md) | v1.9.0 scale and performance guidance: runtime scale diagnostics, benchmark coverage and practical tradeoffs. |
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

## Roadmap, changelog and release documents

| Document | Purpose |
| --- | --- |
| [`roadmap.md`](roadmap.md) | Project direction, internal milestone plan and v2/v3/v4 roadmap. |
| [`versioning.md`](versioning.md) | Versioning, branch, tag and release policy. |
| [`../CHANGELOG.md`](../CHANGELOG.md) | Internal milestone history and public release change history. |
| [`release-notes-2.0.0.md`](release-notes-2.0.0.md) | 2.0.0 release notes for the stable multi-game SDK foundation. |
| [`release-manifest-2.0.0.md`](release-manifest-2.0.0.md) | 2.0.0 release manifest and validation checklist. |
| [`public-surface-2.0.0.md`](public-surface-2.0.0.md) | Frozen intended public C++/C/documentation surface for 2.0.0. |
| [`release-notes-1.0.0.md`](release-notes-1.0.0.md) | Final 1.0.0 release notes. |
| [`release-manifest-1.0.0.md`](release-manifest-1.0.0.md) | Final 1.0.0 release manifest and reviewed local validation data. |
| [`readiness-status.md`](readiness-status.md) | Subsystem readiness estimate for the 1.0.0 line. |
| [`verifying-releases.md`](verifying-releases.md) | Release verification guide. |
| [`ci-artifact-review.md`](ci-artifact-review.md) | Manual CI, benchmark artifact and SDK ZIP artifact review guide. |
| [`release-manifest-template.md`](release-manifest-template.md) | Template for future public major release manifests. |
| [`protection-strategy.md`](protection-strategy.md) | Project protection and distribution strategy. |

Internal milestones such as `v1.1.0`, `v1.2.0` and `v1.3.0` are tracked in the changelog and current documentation instead of separate per-milestone release-note files. Versioned release documents are reserved for public major releases such as `v1.0.0`, `v2.0.0`, `v3.0.0` and `v4.0.0`.

GitHub Actions and Linux GCC/Clang validation for `v1.0.0` are tracked as post-release verification in issue #41. Windows/MSVC local validation was accepted as the release gate for this release.

Normal SDK users usually start with `core-concepts.md`, `architecture.md`, `public-api.md`, `data-packs.md`, `action-bridge.md`, `sdk-structure.md`, `game-profiles.md`, `platform-integration-layer.md` and `packaging.md`. Unity/C# users should start with `integration-targets.md`, `game-profiles.md`, `platform-integration-layer.md` and `csharp-unity.md` after reviewing the C ABI documentation. Browser/WebAssembly users should start with `integration-targets.md`, `game-profiles.md`, `platform-integration-layer.md` and `browser-wasm.md`. Integration maintainers should also use `integration-validation.md` before merging integration-related changes.

---

## Quick local validation

On Windows, use the quick validation batch script for the normal developer loop:

```bat
scripts\quick_validation.bat
```

It configures a local build with tests and examples enabled, builds it, runs `ctest`, and smoke-runs the main examples. It intentionally skips `clc_example_replay_persistence` by default; set `CLC_RUN_REPLAY_EXAMPLE=1` before running the script to include that example. Set `CLC_RUN_INSTALL_CONSUMERS=1` to also install the SDK into the quick build directory and build/run the installed C++ and C ABI consumer examples.

For a complete local pass that captures output and creates one shareable ZIP archive, run:

```bat
scripts\full_validation.bat
```

The archive is written to:

```text
build-full-validation\city-life-core-validation-logs.zip
```

Send that ZIP archive for review when validation output is needed.

The full release gate remains the manual release validation scripts.

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
