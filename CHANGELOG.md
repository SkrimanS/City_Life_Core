# Changelog

All notable changes to City Life Core are tracked here.

Internal milestones such as `v1.1.0`, `v1.2.0`, `v1.3.0` and `v1.4.0` are tracked in this changelog and the current documentation. Separate versioned release notes and release manifests are reserved for public major releases such as `v1.0.0`, `v2.0.0`, `v3.0.0` and `v4.0.0`.

## Unreleased

Changes merged into `main` after the latest public release are recorded here.

### Added

- Added the v1.4.0 game integration profile catalog API with supported/planned profile descriptors, integration boundaries, required/optional systems and non-goals.
- Added `examples/game_profiles.cpp` to show profile discovery, backend-service recommendations and profile digest output.
- Added game profile catalog tests for profile lookup, support status, C ABI needs, Action Bridge usage and server-authoritative boundaries.
- Added `docs/data-packs.md` with `.clcd` schema, required fields, strict loader validation rules and diagnostics guidance.
- Added `examples/validation_hardening.cpp` to demonstrate rejected data-pack input, rejected Action Bridge actions and no-mutation behavior.
- Added data-pack loader hardening tests for missing schema, unknown sections, duplicate keys, missing/zero numeric fields and broken references.
- Added Action Bridge deterministic sequence tests to verify identical action sequences produce identical result/state/event digests.
- Added Action Bridge zero-day rejection coverage to ensure `advance_days` with `days=0` is invalid and does not mutate runtime time.
- Added diagnostics consistency tests for data-pack and Action Bridge rejection diagnostics with severity/path/message coverage.
- Added runtime load failure no-mutation tests for malformed and incomplete runtime save files.
- Added world-state validation and deserialization hardening tests for zero-amount settlement storage and caravan cargo rows.
- Added the v1.2.0 local Action Bridge API for transport-agnostic `external action -> validation -> runtime mutation -> result/events` flows.
- Added stable Action Bridge type constants for `add_resource`, `remove_resource`, `transfer_resource` and `advance_days`.
- Added stable Action Bridge status constants for `accepted`, `invalid` and `rejected`.
- Added stable Action Bridge error-code constants for `malformed_json`, `invalid_action`, `action_rejected` and `unsupported_action_type`.
- Added `clc::sim::RuntimeAction`, `RuntimeActionParseResult`, `RuntimeActionResult`, `parse_runtime_action_json`, `validate_runtime_action`, `dispatch_runtime_action`, `dispatch_runtime_action_json` and `runtime_action_result_to_json`.
- Added Action Bridge support for `add_resource`, `remove_resource`, `transfer_resource` and `advance_days` actions.
- Added Action Bridge result JSON fields for `command_detail`, `events_detail` and `diagnostics_detail`.
- Added `examples/action_bridge.cpp` as the C++ Action Bridge example.
- Added `tests/action_bridge_tests.cpp` with valid, invalid, malformed, malformed payload, malformed numeric value, runtime rejected, no-mutation, parser edge-case and result JSON coverage.
- Added `docs/action-bridge.md` as the current Action Bridge integration guide.
- Added non-throwing C# / Unity `TryCreate` and `TryGetEvent` helpers for gameplay-oriented world creation and event inspection flows.
- Added non-throwing C# / Unity `TryAdvance*` helpers for native world advancement in gameplay-oriented flows.
- Added C# / Unity C ABI compatibility guard helpers so the wrapper can report and reject native libraries older than the required C interface version.
- Added C ABI world time-advance helpers for advancing opaque world handles by seconds, minutes, hours or days, and bumped the C interface version to `4`.
- Added C and C# / Unity example coverage for the new C ABI world time-advance helpers.
- Added C ABI smoke-test coverage for the new world time-advance helpers, including null-handle, extreme invalid-index and zero-duration failure behavior.
- Added `docs/roadmap.md` with the post-1.0.0 project roadmap, internal milestone plan and v2/v3/v4 direction.
- Added the main post-`v2.0.0` development line to the roadmap: global gameplay-mechanics deepening through `v2.x` and `v3.x`, followed by maximum mechanics depth and large-project readiness starting with `v4.0.0`.
- Added `docs/game-profiles.md` with recommended integration profiles for native C++ games, Unity/C#, Browser/WASM, server-authoritative games, MMO-like services and editor/tooling workflows.
- Added `docs/c-abi-expansion-plan.md` with a staged plan for expanding the C ABI for Unity/C#, browser/WASM, tools and other foreign-language integrations.
- Added `docs/integration-targets.md` to clarify supported, initial and planned integration targets across C++, C ABI, Unity/C#, browser/WASM, other engines, backend/MMO and tooling scenarios.
- Added `docs/integration-validation.md` with validation checklists for C++ package, installed consumer, C ABI, C# / Unity, SDK ZIP and future Browser/WASM integration work.
- Added `docs/browser-wasm.md` with the planned browser and WebAssembly integration path for web games, web tools and browser-hosted demos.
- Added `docs/csharp-unity.md` with initial C# and Unity integration guidance through the C ABI and P/Invoke, including native plug-in troubleshooting notes.
- Added `examples/csharp_unity/` with a C# P/Invoke wrapper, Unity `MonoBehaviour` smoke test and Unity-oriented usage notes for the current minimal C ABI.
- Added a GitHub pull request template with branch/release-policy, integration-boundary, compatibility, validation and documentation/changelog checks.
- Added a custom GitHub issue template for general tasks, planning items, investigations, release/packaging work and maintenance notes.
- Added GitHub issue template configuration to disable blank issues and point users to documentation, roadmap and release policy references.

### Changed

- Exposed the game integration profile catalog through the recommended SDK umbrella header.
- Hardened `.clcd` data-pack loading so resources require positive `base_value` and buildings require positive `worker_slots`.
- Kept source-level `DataRegistry::add(...)` compatible for zero numeric defaults by reporting warnings instead of errors for direct C++ construction.
- Hardened `ResourceStorage` so zero-amount add, direct remove and transfer operations are rejected without mutation.
- Hardened contract insertion so new contracts must be open and terminal contract states cannot be inserted as direct input mutations.
- Hardened `SimulationEngine` resource mutations so unknown resource ids are rejected before settlement storage mutation.
- Hardened `SimulationEngine::restore_state(...)` validation for settlement storage and market demand resources, with staged market restore before commit.
- Hardened world-state validation so settlement storage and caravan cargo entries reject empty resource ids and zero amounts before restore/load commit.
- Hardened runtime load failure coverage so malformed or incomplete runtime saves preserve the existing runtime state.
- Updated examples and documentation indexes to include the v1.3.0 validation-hardening example and data-pack documentation.
- Updated the Action Bridge parser and parse-level rejection path so rejected results preserve parsed `action_id` and `type` when those fields are available.
- Updated the SDK umbrella header to expose the Action Bridge C++ API through `clc/CityLifeCore.hpp`.
- Updated the documentation policy so internal milestones use current docs plus `CHANGELOG.md`; versioned release documents are reserved for public major releases.
- Updated the documentation index and Russian documentation index to point to current Action Bridge docs and changelog-based internal milestone history.
- Updated the C# / Unity smoke-test script to use non-throwing `TryCreate`, `TryAdvanceMinutes` and `TryGetEvent` helpers and log Unity errors on failed operations.
- Updated the C# / Unity smoke-test script to log actual and required C ABI versions before creating a native world.
- Hardened C ABI world destruction, accessors and time-advance helpers so exceptions are caught at the C boundary and converted to safe fallback results.
- Updated `CONTRIBUTING.md` to document restricted contributions, current `v1.x` branch policy, major-only public releases, integration boundary rules, PR expectations and security-sensitive integration areas.
- Updated `docs/versioning.md` to document the current release policy: public Git tags and GitHub Releases are planned only for `v1.0.0`, `v2.0.0`, `v3.0.0` and `v4.0.0`; minor version labels are internal development milestones.
- Updated roadmap and versioning branch policy to use broad internal stage branches named directly after the milestone, such as `v1.1.0`, `v1.2.0` and `v1.3.0`.
- Updated `docs/architecture.md` to describe core layers, public C++ API, C ABI, Unity/C# wrapper, future Browser/WASM adapter, server-authoritative/MMO boundaries and non-goals.
- Updated `docs/build-and-linking-policy.md` to document C++ source-first linking, C ABI foreign-language boundaries, shared-library native plug-in builds, Unity/C# P/Invoke usage and planned Browser/WASM linking policy.
- Updated `docs/ci-artifact-review.md` to expand CI, benchmark, SDK ZIP, installed package, C ABI consumer, C# / Unity example and planned Browser/WASM artifact review guidance.
- Updated `docs/cmake-package.md` to describe the exported target, C++ and C ABI consumers, installed package variables, installed examples, shared-library native plug-in builds and planned Browser/WASM package notes.
- Updated `docs/compatibility.md` to split compatibility expectations across C++ source compatibility, C ABI compatibility, C# / Unity wrapper expectations, planned Browser/WASM support, save/load, replay, data validation and packaging.
- Updated `docs/migration.md` with migration guidance for lower-kebab-case docs, CMake package usage, source-first C++ API, C ABI adoption, Unity/C# wrapper migration, planned Browser/WASM support, game profiles, save/load and replay checks.
- Updated `docs/protection-strategy.md` to document canonical source verification, major-only public release protection, archive provenance, API boundary protection, integration claim protection and downstream responsibility.
- Updated `docs/public-api.md` to explain the C++ API, C ABI, Unity/C# wrapper path, planned Browser/WASM boundary, examples/adapters, Action Bridge and unsupported internals.
- Updated `docs/public-api-status.md` to classify the public C++ API, local Action Bridge, minimal C ABI, examples/adapters, planned integration adapters and private unsupported internals.
- Updated `docs/readiness-status.md` to separate the `v1.0.0` release snapshot from post-1.0.0 integration readiness for C ABI, Unity/C#, Browser/WASM, game profiles, server-authoritative runtime, MMO-like support and tooling workflows.
