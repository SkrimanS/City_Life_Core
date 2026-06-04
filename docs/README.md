# City Life Core Documentation

Version: **4.0.0**
Status: **v4.0.0 release candidate / multi-game platform foundation**

This directory contains developer-facing documentation for integrating City Life Core into a C++ game, server, backend service, editor, or simulation tool.

Russian documentation starts at [`ru/README.md`](ru/README.md).

---

## Start here

| Document | Purpose |
| --- | --- |
| [`core-concepts.md`](core-concepts.md) | Main systems: registry, runtime, ticks, settlements, caravans, economy, contracts, events and persistence. |
| [`core-completion-definition.md`](core-completion-definition.md) | Definition of the completed handoff-ready headless simulation SDK core. |
| [`core-completion-readiness.md`](core-completion-readiness.md) | Public readiness API for tracking completed-core progress across architecture, deep systems, runtime, replay, C ABI, handoff and packaging. |
| [`core-systems-audit.md`](core-systems-audit.md) | Completed-core audit for hidden coupling, deep/regional coverage and authority sweeps. |
| [`architecture.md`](architecture.md) | Architecture overview for core layers, public C++ API, C ABI, integration layers and non-goals. |
| [`public-api.md`](public-api.md) | Public SDK API, recommended include, validation model, workflows and usage rules. |
| [`data-packs.md`](data-packs.md) | `.clcd` data pack format, required fields, strict loader validation and diagnostics. |
| [`action-bridge.md`](action-bridge.md) | Local JSON action bridge for external tools, game layers and future server-authoritative adapters. |
| [`sdk-structure.md`](sdk-structure.md) | Repository layout, installed SDK layout, CMake options, integration modes and modules. |
| [`game-profiles.md`](game-profiles.md) | Recommended integration profiles plus catalog validation output, catalog markdown, adoption reports and checklist output for native games, Unity/C#, Browser/WASM, servers, MMO-like systems and tools. |
| [`game-developer-handoff.md`](game-developer-handoff.md) | Practical SDK handoff guide for game teams, including C++/C ABI entry points, examples, profiles and non-goals. |
| [`game-developer-handoff-catalog.md`](game-developer-handoff-catalog.md) | Catalog-level handoff gate across all supported game profiles and installed SDK manifest artifacts. |
| [`sdk-handoff-manifest.md`](sdk-handoff-manifest.md) | Release/package manifest API for checking that installed SDK handoff artifacts are present. |
| [`platform-integration-layer.md`](platform-integration-layer.md) | v1.5.0 platform and adapter boundary guidance plus deep systems foundation API notes for Unity/C#, Browser/WASM, server, editor and SDK embedding work. |
| [`platform-core.md`](platform-core.md) | v4.0.0 static module registry, genre profiles, content pipeline validation, editor/admin queries and unified diagnostics. |
| [`module-architecture.md`](module-architecture.md) | Static module kinds, dependencies and compatibility model for the platform core. |
| [`genre-profiles.md`](genre-profiles.md) | Headless genre profile metadata for idle/tycoon, city builder, strategy, settlement RPG and survival economy. |
| [`content-pipeline.md`](content-pipeline.md) | Content pack manifest validation and module/profile compatibility checks. |
| [`editor-admin-api.md`](editor-admin-api.md) | Headless editor/admin query API guidance. |
| [`diagnostics-api.md`](diagnostics-api.md) | Unified validation and digest diagnostics surface. |
| [`simulation-processing-doctrine.md`](simulation-processing-doctrine.md) | Deep simulation doctrine: core processing, deterministic processors, explicit dependencies and no hidden product gameplay ownership. |
| [`simulation-processors.md`](simulation-processors.md) | Processor registry, feature toggles, dependency toggles, deterministic ordering and readiness reports. |
| [`deep-simulation-systems.md`](deep-simulation-systems.md) | Compact v1.x deep domain reports for population, weather, policies, ecology, crises, autonomous decisions, schemas and scenario presets. |
| [`deep-replay-coverage.md`](deep-replay-coverage.md) | Save/load, replay, migration, fixture and diagnostics coverage matrix for deep/regional systems. |
| [`save-replay-fixtures.md`](save-replay-fixtures.md) | Golden/invalid save-replay fixture catalog for deep and regional systems. |
| [`feature-toggles.md`](feature-toggles.md) | Feature toggle model for optional/configurable deep simulation systems. |
| [`dependency-registry.md`](dependency-registry.md) | Named dependency-toggle model for cross-system effects and diagnostics. |
| [`server-authoritative-mmo.md`](server-authoritative-mmo.md) | v1.6.0 server-authoritative and MMO-like foundation: action envelopes, host-owned session identity, shard descriptors, audit records and sequence validation. |
| [`economy-depth.md`](economy-depth.md) | v1.7.0 economy, factions and contracts depth: contract economy assessment, resource flow plans, portfolio summaries and extension points. |
| [`persistence-replay-migration.md`](persistence-replay-migration.md) | v1.8.0 persistence, replay and migration: save-format review, legacy migration, replay diagnostics and checkpoint guidance. |
| [`scale-performance.md`](scale-performance.md) | v1.9.0 scale and performance guidance: runtime scale diagnostics, benchmark coverage and practical tradeoffs. |
| [`settlement-development.md`](settlement-development.md) | Post-v2 settlement development planning helper: resource needs, production opportunities, market pressure and digest output. |
| [`multiplayer-action-authority.md`](multiplayer-action-authority.md) | v2.1.0 actor-scoped action authority, rejection reasons, no-mutation dispatch and audit output. |
| [`snapshot-sync-model.md`](snapshot-sync-model.md) | v2.2.0 snapshot and client-visible state summaries for future sync layers. |
| [`multiplayer-persistence-replay.md`](multiplayer-persistence-replay.md) | v2.3.0 command-log validation and replay through action authority. |
| [`multiplayer-economy-safety.md`](multiplayer-economy-safety.md) | v2.4.0 no-mutation economy safety reviews for buy, sell and contract reward flows. |
| [`multiplayer-load-diagnostics.md`](multiplayer-load-diagnostics.md) | v2.5.0 multiplayer load snapshots and risk thresholds. |
| [`pre3-large-world-prep.md`](pre3-large-world-prep.md) | v2.6.0 readiness gates for the transition to large-world work. |
| [`large-world-foundation.md`](large-world-foundation.md) | v3.0.0 region descriptors, assignments, validation and digest output. |
| [`regional-simulation-systems.md`](regional-simulation-systems.md) | v3.x regional economy, logistics, climate, territory, migration, audit stream and maintenance reports. |
| [`runtime-core-systems.md`](runtime-core-systems.md) | Runtime bridge that evaluates deep and regional systems from `SimulationRuntime`. |
| [`runtime-processor-orchestration.md`](runtime-processor-orchestration.md) | Deterministic runtime processor orchestration plus bounded deep/regional diagnostic event stream. |
| [`platform-readiness-4.0.0.md`](platform-readiness-4.0.0.md) | v4.0.0 platform/tooling readiness expectations for the completed core. |
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
| [`detailed-ai-roadmap.md`](detailed-ai-roadmap.md) | AI-ready detailed version plan with scope, non-goals and readiness criteria for future work. |
| [`deep-simulation-roadmap-addendum.md`](deep-simulation-roadmap-addendum.md) | Owner-approved deep simulation addendum expanding v1.x-v3.x and preparing v4.0.0 readiness. |
| [`v1-deep-simulation-completeness.md`](v1-deep-simulation-completeness.md) | v1.x deep simulation completeness gate using the processor contract. |
| [`pre-v2-readiness-checklist.md`](pre-v2-readiness-checklist.md) | Checklist for validating processor, feature/dependency, save/load, replay and diagnostics readiness before v2.0.0. |
| [`versioning.md`](versioning.md) | Versioning, branch, tag and release policy. |
| [`../CHANGELOG.md`](../CHANGELOG.md) | Internal milestone history and public release change history. |
| [`release-notes-3.0.0.md`](release-notes-3.0.0.md) | 3.0.0 release notes for the large-world foundation. |
| [`release-notes-4.0.0.md`](release-notes-4.0.0.md) | 4.0.0 release notes for the multi-game platform foundation. |
| [`release-manifest-4.0.0.md`](release-manifest-4.0.0.md) | 4.0.0 platform foundation manifest and validation checklist. |
| [`release-manifest-3.0.0.md`](release-manifest-3.0.0.md) | 3.0.0 release manifest and validation checklist. |
| [`public-surface-3.0.0.md`](public-surface-3.0.0.md) | Intended public C++/C/documentation surface for 3.0.0. |
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
