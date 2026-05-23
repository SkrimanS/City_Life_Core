# Changelog

All notable changes to City Life Core are tracked here.

Все заметные изменения City Life Core фиксируются здесь.

## 0.9.9 - Unreleased

### Added

- Added C++ SDK umbrella header `clc/CityLifeCore.hpp` as the recommended first include for C++ consumers.
- Added C ABI header `clc/c/CityLifeCoreC.h` with version/time utilities, minimal opaque `clc_world` world-handle functions and read-only world event accessors.
- Added C ABI bridge implementation and bumped the C interface version to `3` for the `clc_world` and read-only event accessor surface.
- Added C ABI smoke coverage for version/time utilities, null-safe `clc_world` access, create/destroy, state access, tick advancement, event ids, event ticks, event types and event payloads.
- Added standalone installed-package C ABI consumer coverage through `examples/c_abi_consumer/`.
- Added standalone installed-package C++ `find_package(CityLifeCore CONFIG REQUIRED)` consumer coverage through `examples/find_package_consumer/`.
- Added CPack ZIP SDK package generation with `SHA256SUMS.txt` checksum output.
- Added CI validation for tests, benchmarks, installed SDK consumers, unpacked ZIP consumers, SDK ZIP artifacts, and benchmark artifacts.
- Added benchmark runner and benchmark artifact upload flow.
- Added public SDK/release documentation for CMake packages, SDK ZIP packages, C ABI, compatibility, versioning, migration, release verification, protection strategy, and release checklist.
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

- Bumped project version from `0.9.7` to `0.9.9` in `CMakeLists.txt` and `include/clc/core/Version.hpp`.
- Updated smoke tests to expect `0.9.9`.
- Updated public package documentation to reflect the C ABI v3 `clc_world` handle and read-only event accessors instead of describing the C ABI as version/time-only.
- Updated release checklist and packaging docs to require installed and unpacked ZIP C ABI consumer validation for C ABI version, time utilities, `clc_world` behavior and read-only event accessors.
- Updated contribution documentation to keep contributions restricted until the final license and contribution model are chosen.
- Changed runtime event log timestamps from day numbers to absolute runtime ticks.
- Changed runtime save/load overloads to preserve `SimulationRuntime::time` through the runtime-specific world-state bridge.
- Changed runtime caravan loading checks to use tick progress instead of day-only progress, preventing cargo loading after tick-based departure.
- Kept day-based APIs as compatibility wrappers while making tick-based APIs the preferred path for real-time games.

### Compatibility

- Existing day-based route, caravan, and contract APIs remain available.
- Existing C ABI version/time functions remain available under C interface version `3`.
- The new `clc_world` and read-only world event C ABI surface is intentionally minimal and does not expose full runtime, registries, containers, save/load, callbacks, mutable event payload APIs, caravans, contracts, or economy workflows.
- Older world-state saves without explicit runtime `time` are still accepted and synchronize runtime clock from `current_day`.
- Older contract rows without `due_ticks` are still accepted and derive `due_ticks` from `due_day`.

## 0.9.6 - Unreleased

### Added

- Added event-log roundtrip coverage for `runtime.caravan.cargo_delivered` after midpoint runtime save/load.
- Added deterministic cargo delivery event payload validation using `caravan_id->destination_settlement_id:total=N`.
- Added cargo delivery event-log analysis checks for `caravan_cargo_delivered_events` and replayed contract fulfillment events.
- Added negative event-log checksum drift coverage for changed cargo delivery payloads.
- Added `docs/RELEASE_NOTES_0.9.6.md`.

### Changed

- Bumped project version from `0.9.5` to `0.9.6` in `CMakeLists.txt` and `include/clc/core/Version.hpp`.
- Extended `clc_runtime_event_log_roundtrip_tests` to cover explicit cargo delivery event emission after runtime load and contract fulfillment.
- Updated README to describe cargo delivery roundtrip diagnostics.

## 0.9.5 - Unreleased

### Added

- Added `clc_runtime_cargo_delivery_persistence_tests` for explicit cargo delivery save/load coverage.
- Added roundtrip coverage proving delivered destination settlement storage persists and caravan cargo remains empty after load.
- Added negative drift coverage for delivered destination storage and redelivered caravan cargo.
- Added `docs/RELEASE_NOTES_0.9.5.md`.

### Changed

- Bumped project version from `0.9.4` to `0.9.5` in `CMakeLists.txt` and `include/clc/core/Version.hpp`.
- Extended runtime replay persistence tests to deliver remaining cargo after midpoint load and contract fulfillment.
- Updated README persistence status to include cargo delivery replay coverage.

## 0.9.4 - Unreleased

### Added

- Added explicit arrived cargo delivery workflow through `deliver_runtime_arrived_caravan_cargo_to_destination()`.
- Added `RuntimeCargoDeliveryEntry` and `RuntimeCaravanCargoDeliveryResult`.
- Added rollback-safe delivery of all remaining arrived caravan cargo into destination settlement storage.
- Added runtime event type `runtime.caravan.cargo_delivered`.
- Added cargo delivery event summary and analysis counters.
- Added `append_runtime_caravan_cargo_delivery_event()` helper.
- Added workflow and event-log coverage for explicit cargo delivery.
- Added `docs/RELEASE_NOTES_0.9.4.md`.

### Changed

- Bumped project version from `0.9.3` to `0.9.4` in `CMakeLists.txt` and `include/clc/core/Version.hpp`.
- Updated README and Public API documentation for explicit cargo delivery.
- Kept default runtime tick semantics unchanged: cargo delivery is explicit and not auto-unloaded by tick.

## 0.9.3 - Unreleased

### Added

- Added CMake install/export package support for the C++ SDK.
- Added generated `CityLifeCoreConfig.cmake` and `CityLifeCoreConfigVersion.cmake` package files.
- Added exported package target `CityLifeCore::core` for external `find_package(CityLifeCore CONFIG REQUIRED)` consumers.
- Added install rules for public headers, SDK docs, examples, and demo data.
- Added `docs/PACKAGING.md` with bilingual source install and package consumption guidance.
- Added `docs/RELEASE_NOTES_0.9.3.md`.

### Changed

- Bumped project version from `0.9.2` to `0.9.3` in `CMakeLists.txt` and `include/clc/core/Version.hpp`.
- Updated README packaging status and install/find_package usage examples.
- Updated Public API and SDK Structure documentation versions and package-mode guidance.

## 0.9.2 - Unreleased

### Added

- Added bilingual public-facing `README.md` for the current 0.9.x runtime/persistence and SDK-readiness phase.
- Added `docs/PUBLIC_API.md` with a bilingual overview of the current public C++ API surface, subsystem headers, runtime workflow APIs, and persistence validation APIs.
- Added `docs/SDK_STRUCTURE.md` with bilingual SDK layout, release package structure, public include policy, C++ SDK boundary, future C ABI direction, and release readiness checklist.
- Added SDK examples for basic runtime ticks, save/load roundtrip validation, and deterministic replay persistence.
- Added `examples/README.md` with bilingual build and usage guidance for SDK examples.
- Added deterministic runtime replay persistence coverage with midpoint save/load, post-load replay continuation, contract fulfillment, reward flow, ledger validation, and negative replay drift detection.
- Added corrupted runtime save/load coverage for unknown rows, invalid contract status, orphan settlement storage, orphan caravan cargo, and invalid escape sequences.
- Added failed-load mutation guard coverage proving corrupted runtime loads do not mutate the target runtime.
- Added runtime tick consequences for arrived caravan IDs and overdue contract failures in day reports and run summaries.
- Added runtime event log support for `runtime.contract.failed` events, including analysis and validation coverage.

### Changed

- Bumped project version from `0.9.0` to `0.9.2` in `CMakeLists.txt` and `include/clc/core/Version.hpp`.
- Registered `clc_runtime_replay_persistence_tests` and `clc_runtime_corrupted_save_tests` in CMake.
- Registered SDK examples in CMake behind `CLC_BUILD_EXAMPLES`.
- Added explicit compile-safety include coverage for replay persistence tests and SDK examples.
- Extended runtime tick tests to cover arrival consequences, overdue contract status transitions, and contract failure summaries.
- Extended runtime event log tests to cover failed contract event emission and analysis.

## 0.9.1 - Unreleased

### Added

- Added semantic runtime persistence validation hardening.
- Added deterministic post-load continuation validation for runtime save/load roundtrips.
- Added runtime registry count equivalence validation for resources, currencies, buildings, professions, and settlements.
- Added referenced registry semantic validation for resource and settlement definitions actually used by runtime state.
- Added broad negative drift coverage for runtime state:
  - engine current day, settlements, storage, buildings, events, and market demand;
  - routes and route identity;
  - caravans, travel state, route endpoints, ownership, and cargo;
  - factions, faction identity, display names, and reputation;
  - ownership records;
  - contracts, contract identity, parties, resource, quantity, reward, due day, and status;
  - wallet and ledger state.

### Changed

- Refactored runtime persistence validation tests with helper functions for clearer drift assertions.
- Strengthened runtime equivalence validation from shallow persistence roundtrip checks toward semantic runtime comparison.

## 0.9.0 - Unreleased

### Added

- Runtime/world integration layer around `SimulationRuntime`.
- Runtime scenario bootstrap helpers for building a deterministic basic world.
- Runtime workflow helpers for settlements, routes, factions, ownership, contracts, caravans, cargo movement, and contract fulfillment.
- Runtime persistence support for saving and loading complete runtime state.
- Runtime full-roundtrip tests covering integrated world state persistence.
- Runtime tick, run-days, run-until-arrival, auto-contract, arrival-contract, event-log, and diagnostics test coverage.
- Runtime event log roundtrip and event analysis coverage.

### Changed

- Persistence moved from isolated snapshot/world-state coverage toward integrated runtime save/load workflows.
- Runtime systems became the preferred integration surface for game/server code instead of manually wiring every subsystem.

## 0.8.0 - Unreleased

### Added

- Routes between settlements.
- Caravan state, cargo storage, route-based travel, arrival checks, and caravan catalog operations.
- Faction catalog, faction reputation, and ownership primitives for settlements and caravans.
- Resource delivery contracts with issuer/receiver factions, required resource, quantity, reward coins, due day, and status.
- Contract reward and ledger integration.
- Runtime-oriented tests for routes, caravans, factions, ownership, contracts, and reward ledger behavior.

### Changed

- The project expanded from single-engine settlement simulation into multi-system world simulation.
- Economy, logistics, ownership, and obligations became connected through routes, caravans, contracts, and ledger entries.

## 0.7.5 - Unreleased

### Changed

- Refreshed `README.md` so it no longer describes the project as being in the completed `0.1.0 Foundation Core` phase.
- Documented the current `0.7.x Simulation Core Expansion` state and implemented subsystem list.
- Realigned the near-term roadmap away from deeper report/digest layers and toward save/load, routes, caravans, factions, and contracts.

## 0.7.4 - Unreleased

### Added

- `scenario_catalog_digest()` for compact scenario preset catalog summaries.
- `scenario_catalog_preset_lookup_digest()` for stable found/missing preset lookup summaries.
- Scenario digest test coverage for empty catalogs, found preset lookup digests, missing preset lookup digests, and catalog count updates.

## 0.7.3 - Unreleased

### Added

- `clc/sim/ScenarioCatalog.hpp` with `run_scenario_preset_from_catalog()` helper.
- Catalog runner support for running a found preset by ID through `SimulationEngine`.
- Missing preset IDs return unchanged snapshot-range scenario results without advancing the engine.
- Scenario digest test coverage for found and missing catalog runner paths.

## 0.7.2 - Unreleased

### Added

- `SimulationScenarioPresetCatalog` for storing named scenario presets.
- `add_scenario_preset()` for validated catalog insertion with duplicate ID rejection.
- `scenario_preset_count()` and `scenario_preset_by_id()` helpers for catalog inspection and lookup.
- Scenario digest test coverage for empty catalogs, valid inserts, missing lookups, duplicate rejection, and invalid preset rejection.

## 0.7.1 - Unreleased

### Added

- `scenario_preset_result_digest()` for compact preset-aware scenario summaries.
- Preset digest output includes preset ID, display name, configured day count, and the existing scenario result digest.
- Scenario digest test coverage for valid and invalid preset result digests.

## 0.7.0 - Unreleased

### Added

- `SimulationScenarioPreset` for named scenario run configurations.
- `validate_scenario_preset()` for preset ID, display name, and day-count validation.
- `SimulationEngine::run_scenario_preset()` for running validated scenario presets through the existing scenario pipeline.
- Scenario preset test coverage for validation failures, valid no-op behavior, and digest compatibility.

## 0.6.x - Unreleased

### Added

- Multi-day simulation runs through `SimulationEngine::run_days()`.
- `SimulationScenarioSummary` and `SimulationScenarioResult`.
- Initial/final scenario snapshots.
- Scenario event and warning deltas.
- Scenario helper APIs for duration, success, warning/event checks, and digest output.
- Scenario preset support and catalog helpers.

## 0.5.x - Unreleased

### Added

- `SimulationEngine` for coordinating settlements and economy reporting.
- Engine day reports, events, snapshots, command APIs, lookup helpers, command results, cumulative event log, event filters, and scenario runs.
- Engine command validation for settlement, building, resource, and transfer operations.

## 0.4.x - Unreleased

### Added

- Market demand and deterministic integer-only price reports.
- Basic wallet and trade transactions.
- Economy ledger and ledger aggregation helpers.
- Market order API and deterministic order matching.

## 0.3.x - Unreleased

### Added

- Settlement simulation primitives.
- `ResourceStorage` with controlled add/remove/transfer operations.
- Building input consumption and output production.
- Settlement reports with deterministic ordering.

## 0.2.x - Unreleased

### Added

- Data definitions for resources, currencies, buildings, professions, and settlements.
- `ValidationReport` and `DataRegistry`.
- `.clcd` data pack loading and folder loading.
- Cross-reference validation for building definitions.
- Demo fantasy data pack placeholder.

## 0.1.0 - Unreleased

### Added

- Initial CMake C++20 project skeleton.
- Core version API.
- Result/Error model.
- Typed core IDs.
- Game time primitive.
- In-memory event log.
- Minimal world lifecycle and tick advancement.
- Bootstrap CLI runner.
- Smoke tests.
- GitHub Actions CI.
- Roadmap and architecture notes.
