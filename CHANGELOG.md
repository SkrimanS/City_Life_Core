# Changelog

## 0.6.1 - Unreleased

### Added

- `SimulationScenarioSummary` for aggregated multi-day simulation reporting.
- `summarize_day_reports()` helper for summarizing existing `SimulationDayReport` batches.
- Summary fields for days, first/last day, settlement ticks, consumed resources, production, active/skipped building ticks, events, and warnings.
- Simulation test coverage for empty and non-empty scenario summaries.

## 0.6.0 - Unreleased

### Added

- `SimulationEngine::run_days()` for running multiple simulation days in sequence.
- Multi-day scenario runner reports that preserve the same side effects as repeated `advance_day()` calls.
- Simulation test coverage for zero-day runs, sequential day reports, current day advancement, warning propagation, and cumulative day events.

## 0.5.9 - Unreleased

### Added

- `SimulationEngine::clear_events()` for clearing the cumulative engine event log.
- Event clearing test coverage proving event filters, recent events, and snapshot events are empty after clearing.
- Regression coverage that event clearing does not mutate current day or settlement resources.

## 0.5.8 - Unreleased

### Added

- Engine event filtering helpers for activity log consumers.
- `SimulationEngine::recent_events()` for retrieving the latest events while preserving chronological order.
- `SimulationEngine::events_by_type()` for filtering cumulative events by exact event type.
- Test coverage for empty logs, missing event types, success/failure filters, recent event limits, and latest day events.

## 0.5.7 - Unreleased

### Added

- Cumulative engine event log for command and day events.
- `SimulationEngine::events()` for read-only access to accumulated events.
- `SimulationSnapshot::events` for exposing activity log entries alongside state snapshots.
- Command wrapper events for succeeded and failed structured commands.
- Cumulative storage of day events emitted by `advance_day()`.
- Test coverage for command events and snapshot event exposure.

## 0.5.6 - Unreleased

### Added

- `SimulationCommandResult` for structured engine command outcomes.
- Structured command wrappers for settlement, building, resource, and transfer commands.
- Command result fields for command name, success flag, subject ID, target ID, resource ID, amount, and validation details.
- Dedicated engine command result test binary and CTest entry.

## 0.5.5 - Unreleased

### Added

- Engine-level read-only lookup APIs for settlements and settlement resource amounts.
- `SimulationEngine::has_settlement()` for fast presence checks.
- `SimulationEngine::settlement()` for const settlement lookup by ID.
- `SimulationEngine::settlement_resource_amount()` for resource amount lookup without exposing mutable storage.
- Dedicated engine lookup test binary and CTest entry.

## 0.5.4 - Unreleased

### Added

- Engine-level command API for removing resources from settlements by settlement ID.
- Engine-level command API for transferring resources between settlements.
- Validation coverage for empty IDs, same source/target settlements, unknown settlements, zero amounts, and insufficient resources.
- Dedicated resource command test binary and CTest entry.

## 0.5.3 - Unreleased

### Added

- Engine-level command API for creating settlements from registered settlement definitions.
- Engine-level command API for adding buildings to settlements by settlement ID.
- Engine-level command API for adding resources to settlements by settlement ID.
- Validation coverage for empty, unknown, and duplicate settlement/building/resource command paths.
- Simulation test coverage for command-driven setup.

## 0.5.2 - Unreleased

### Added

- `SimulationSnapshot` for read-only engine state inspection without advancing the simulation.
- `SimulationEngine::snapshot()` for current day, settlement snapshots, and aggregate market report.
- Simulation test coverage proving snapshots do not mutate `current_day`.

## 0.5.1 - Unreleased

### Added

- Engine-level `SimulationEvent` entries for day start, settlement advancement, and day completion.
- Engine-level warning aggregation from settlement tick reports.
- Simulation test coverage for engine events and warning collection.

## 0.5.0 - Unreleased

### Added

- Initial `SimulationEngine` for coordinating settlement simulation and economy reporting.
- `SimulationDayReport` with day number, settlement tick reports, settlement snapshots, and market summary.
- Engine-owned registry, market state, settlement list, and current day tracking.
- Duplicate settlement ID validation when adding settlements to the engine.
- Dedicated simulation test binary and CTest entry.

## 0.4.4 - Unreleased

### Added

- Market order API for buy/sell limit orders.
- `MarketOrder`, `TradeExecution`, and `OrderMatchResult` structures.
- Market order validation for empty resources, zero quantities, zero prices, and total value overflow.
- Deterministic order matching by resource and limit price.
- Economy test coverage for matching, no-match, and invalid-order scenarios.

## 0.4.3 - Unreleased

### Added

- `EconomyLedger` for recording successful trade transactions.
- `LedgerEntry` and `LedgerEntryType` structures.
- Sequential immutable ledger entries with trade resource, quantity, unit price, total price, and notes.
- Ledger aggregation helpers for total bought and sold quantities per resource.
- Economy test coverage for recording successful trades and rejecting failed trades.

## 0.4.2 - Unreleased

### Added

- Basic trade transaction API for buying and selling resources.
- `Wallet` and `TradeResult` structures.
- Safe buy/sell operations with checks for empty resource IDs, zero quantities, zero prices, price overflow, insufficient coins, insufficient resources, and wallet overflow.
- Economy test coverage for successful and failed trade transactions.

## 0.4.1 - Unreleased

### Added

- `MarketReport` aggregate snapshot for market state inspection.
- `make_market_report()` for sorted price rows and market summary totals.
- Market report fields for total supply, total demand, average price, min price, and max price.
- Economy test coverage for non-empty and empty market reports.

## 0.4.0 - Unreleased

### Added

- Initial economy market pricing module.
- `MarketState` for resource demand tracking.
- `MarketPrice` reports with supply, demand, price, and reason fields.
- Deterministic integer-only price calculation from base value, supply, and demand.
- Market price calculation for registered resources present in storage.
- Dedicated economy test binary and CTest entry.

## 0.3.3 - Unreleased

### Added

- Settlement report snapshot API for read-only state inspection.
- `ResourceAmount`, `BuildingReport`, and `SettlementReport` structures.
- Deterministic storage and building ordering in settlement reports.
- Smoke test coverage for settlement report generation.

## 0.3.2 - Unreleased

### Added

- Building input resource consumption during settlement ticks.
- Settlement tick reporting for consumed inputs, active buildings, and skipped buildings.
- Production now skips buildings when required input resources are missing.
- Smoke test coverage for successful production and missing-input production skips.

## 0.3.1 - Unreleased

### Added

- `ResourceStorage` for controlled resource storage operations.
- Safe add, try-remove, remove-up-to, amount lookup, and transfer operations.
- Settlement simulation now uses `ResourceStorage` instead of raw resource maps.
- Smoke test coverage for storage operations and settlement storage integration.

## 0.3.0 - Unreleased

### Added

- Initial single settlement simulation module.
- `SettlementState`, `BuildingInstance`, and `SettlementTickReport`.
- Settlement creation from data definitions.
- Building instance validation against registered building definitions.
- One-day settlement tick with basic food consumption and building output production.
- Smoke test coverage for a settlement consuming and producing resources without negative storage.

## 0.2.3 - Unreleased

### Added

- `DataPackLoader::load_directory()` for loading all `.clcd` files from a data pack folder.
- Deterministic lexicographic file ordering for folder loads.
- Smoke test coverage for multi-file data pack loading.

## 0.2.2 - Unreleased

### Added

- Cross-reference validation for building definitions.
- Building references for required professions, input resources, and output resources.
- Data pack support for `required_profession_id`, `input_resource_ids`, and `output_resource_ids`.
- Smoke tests for valid and invalid cross-references.

## 0.2.1 - Unreleased

### Added

- `DataPackLoader` for loading simple schema-versioned data pack files into `DataRegistry`.
- INI-like `.clcd` data pack format with repeatable `[resource]`, `[currency]`, `[building]`, `[profession]`, and `[settlement]` sections.
- Demo fantasy core data pack at `data/demo_fantasy/core.clcd`.
- Smoke tests for valid data pack loading and unsupported schema versions.

## 0.2.0 - Unreleased

### Added

- Data definition types for resources, currencies, buildings, professions, and settlements.
- `ValidationReport` for schema/data validation messages.
- `DataRegistry` for registering and looking up validated definitions.
- Duplicate ID validation for registry inserts.
- Smoke tests covering valid inserts, duplicate IDs, and invalid definitions.
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
