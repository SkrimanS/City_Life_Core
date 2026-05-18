# Changelog

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
