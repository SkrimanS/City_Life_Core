# Changelog

All notable changes to City Life Core are tracked here.

## Unreleased

Changes merged into `main` after the latest public release are recorded here.

### Added

- Added `docs/roadmap.md` with the post-1.0.0 project roadmap, internal milestone plan and v2/v3/v4 direction.
- Added the main post-`v2.0.0` development line to the roadmap: global gameplay-mechanics deepening through `v2.x` and `v3.x`, followed by maximum mechanics depth and large-project readiness starting with `v4.0.0`.
- Added `docs/c-abi-expansion-plan.md` with a staged plan for expanding the C ABI for Unity/C#, browser/WASM, tools and other foreign-language integrations.
- Added `docs/integration-targets.md` to clarify supported, initial and planned integration targets across C++, C ABI, Unity/C#, browser/WASM, other engines, backend/MMO and tooling scenarios.
- Added `docs/browser-wasm.md` with the planned browser and WebAssembly integration path for web games, web tools and browser-hosted demos.
- Added `docs/csharp-unity.md` with initial C# and Unity integration guidance through the C ABI and P/Invoke, including native plug-in troubleshooting notes.
- Added `examples/csharp_unity/` with a C# P/Invoke wrapper, Unity `MonoBehaviour` smoke test and Unity-oriented usage notes for the current minimal C ABI.
- Added a custom GitHub issue template for general tasks, planning items, investigations, release/packaging work and maintenance notes.
- Added GitHub issue template configuration to disable blank issues and point users to documentation, roadmap and release policy references.

### Changed

- Updated `docs/versioning.md` to document the current release policy: public Git tags and GitHub Releases are planned only for `v1.0.0`, `v2.0.0`, `v3.0.0` and `v4.0.0`; minor version labels are internal development milestones.
- Updated roadmap and versioning branch policy to use broad internal stage branches named directly after the milestone, such as `v1.1.0`, `v1.2.0` and `v1.3.0`.
- Updated the C ABI documentation to point non-C++ integrations toward the staged C ABI expansion plan.
- Updated the C# / Unity smoke-test script to use Unity Inspector-friendly serialized fields.
- Updated the C# / Unity documentation to reference the optional Unity smoke-test script.
- Updated the roadmap to track C# / Unity integration as part of multi-game and platform integration support.
- Updated root README, documentation index, Russian documentation index and examples index with integration target, C ABI expansion, C# / Unity and browser/WebAssembly links.
- Updated SDK install rules so C# example sources are included with installed examples.
- Updated GitHub bug report and feature request issue templates with clearer affected-area, version, compatibility, roadmap and reproduction sections.
- Linked the roadmap and release policy from the root README, documentation index and Russian documentation index.

## 1.0.0 - 2026-05-23

Status: **released / v1.0.0**.

### Added

- Added C++ SDK umbrella header `clc/CityLifeCore.hpp` as the recommended first include for C++ consumers.
- Added C ABI header `clc/c/CityLifeCoreC.h` with version/time utilities, minimal opaque `clc_world` world-handle functions and read-only world event accessors.
- Added C ABI bridge implementation with C interface version `3` for the `clc_world` and read-only event accessor surface.
- Added C ABI smoke coverage for version/time utilities, null-safe `clc_world` access, create/destroy, state access, tick advancement, event ids, event ticks, event types and event payloads.
- Added standalone installed-package C ABI consumer coverage through `examples/c_abi_consumer/`.
- Added standalone installed-package C++ `find_package(CityLifeCore CONFIG REQUIRED)` consumer coverage through `examples/find_package_consumer/`.
- Added CPack ZIP SDK package generation with `SHA256SUMS.txt` checksum output.
- Added CI validation shape for tests, benchmarks, installed SDK consumers, unpacked ZIP consumers, SDK ZIP artifacts, and benchmark artifacts.
- Added benchmark runner and benchmark artifact upload flow.
- Added manual release-validation scripts:
  - `scripts/manual_release_validation.sh`;
  - `scripts/manual_release_validation.ps1`.
- Added release-gate documentation and tracking:
  - `docs/readiness-status.md`;
  - `docs/release-notes-1.0.0.md`;
  - `docs/ci-artifact-review.md`;
  - `docs/release-manifest-1.0.0.md`.
- Added public SDK/release documentation for CMake packages, SDK ZIP packages, C ABI, compatibility, versioning, migration, release verification, protection strategy, release manifests and release checklist.
- Added hardening tests for market edge cases, data registry validation, faction/ownership mutation guards, storage, ledger, settlement arithmetic, runtime workflows, and persistence failure paths.
- Added tick-based runtime clock persistence through `SimulationRuntime::time` and `SimulationWorldState::time`.
- Added tick-based route, caravan, and contract deadline support for real-time, MMO, and non-turn-based games.
- Added high-level tick runtime helpers:
  - `run_runtime_ticks(...)`;
  - `run_runtime_until_first_caravan_arrival_by_ticks(...)`;
  - `run_runtime_until_first_caravan_arrival_by_ticks_and_fulfill_contract(...)`.
- Added tick-based contract deadline APIs and tests for `due_ticks` without daily engine advancement.
- Added runtime save/load coverage for `runtime.time`, `due_ticks`, caravan tick progress, and settlement tick remainders.
- Added legacy save compatibility that restores runtime clock from saved `current_day` when explicit `time` is missing.
- Added tick-based runtime diagnostics and event-log helpers:
  - `runtime.tick.completed`;
  - tick-run event-log append helpers;
  - absolute tick checks for fulfilled and failed contract events.
- Added safer cargo event-log overloads that read timestamps from `SimulationRuntime::time` instead of requiring callers to pass a raw tick.
- Added regression tests for absolute event ticks, runtime clock drift, tick-only caravan loading, tick-run arrival, tick-run fulfillment, and tick event logs.

### Changed

- Bumped project version to `1.0.0` in `CMakeLists.txt` and `include/clc/core/Version.hpp`.
- Updated C++ and C ABI smoke tests to expect `1.0.0`.
- Updated public package documentation to reflect the C ABI v3 `clc_world` handle and read-only event accessors instead of describing the C ABI as version/time-only.
- Updated release checklist, PR gate and blocker issue references to require release documentation and validation review before merge/release.
- Updated CODEOWNERS coverage for release-validation scripts and release-governance docs.
- Updated contribution documentation to keep contributions restricted until the final license and contribution model are chosen.
- Changed runtime event log timestamps from day numbers to absolute runtime ticks.
- Changed runtime save/load overloads to preserve `SimulationRuntime::time` through the runtime-specific world-state bridge.
- Changed runtime caravan loading checks to use tick progress instead of day-only progress, preventing cargo loading after tick-based departure.
- Kept day-based APIs as compatibility wrappers while making tick-based APIs the preferred path for real-time games.
- Removed obsolete version-specific release notes for pre-1.0 preparation layers.
- Converted public documentation to an English-primary structure with Russian entry point under `docs/ru/` and lower-kebab-case document filenames.

### Compatibility

- Existing day-based route, caravan, and contract APIs remain available.
- Existing C ABI version/time functions remain available under C interface version `3`.
- The `clc_world` and read-only world event C ABI surface is intentionally minimal and does not expose full runtime, registries, containers, save/load, callbacks, mutable event payload APIs, caravans, contracts, or economy workflows.
- Older world-state saves without explicit runtime `time` are still accepted and synchronize runtime clock from saved `current_day`.
- Older contract rows without `due_ticks` are still accepted and derive `due_ticks` from `due_day`.
- C++ API compatibility is source-first; binary ABI stability is not promised for the C++ API.

Pre-1.0 development history is intentionally not expanded here. The 1.0.0 release notes and public documentation are the source of truth for the supported SDK surface.
