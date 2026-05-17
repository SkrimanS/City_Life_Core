# Changelog

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
