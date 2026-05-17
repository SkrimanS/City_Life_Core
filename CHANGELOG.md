# Changelog

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
