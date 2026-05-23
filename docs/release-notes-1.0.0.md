# Release Notes 1.0.0

Status: **released**

City Life Core 1.0.0 is the first released SDK line for the headless settlement simulation core.

## Highlights

- C++20 simulation SDK for settlements, resources, routes, caravans, factions, contracts, economy and persistence.
- Tick-based and day-based runtime helpers.
- Data registry and validation layer for game definitions.
- Runtime workflows for server-authoritative integrations.
- Save/load validation and deterministic replay diagnostics.
- Installed CMake package with `CityLifeCore::core`.
- Minimal C interface for version/time utilities, opaque world handles and read-only world event access.

## Release status

The 1.0.0 line is released under Apache-2.0. Windows/MSVC local validation was accepted as the release gate. Linux GCC/Clang and GitHub Actions validation are tracked as post-release verification where applicable.

## Related documents

- [Release manifest](release-manifest-1.0.0.md)
- [Release verification](verifying-releases.md)
- [SDK ZIP package](sdk-zip-package.md)
