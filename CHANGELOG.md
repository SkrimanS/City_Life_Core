# Changelog

All notable changes to City Life Core are tracked here.

Все заметные изменения City Life Core фиксируются здесь.

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
