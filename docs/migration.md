# Migration Guide

Version: **1.0.0**

Use this guide when updating an older City Life Core integration to the 1.0.0 documentation and package layout.

## Documentation path changes

Documentation filenames now use lower-kebab-case. Examples:

- `docs/CORE_CONCEPTS.md` → `docs/core-concepts.md`
- `docs/PUBLIC_API.md` → `docs/public-api.md`
- `docs/SDK_STRUCTURE.md` → `docs/sdk-structure.md`
- `docs/RELEASE_NOTES_1.0.0.md` → `docs/release-notes-1.0.0.md`

Russian documentation is grouped under `docs/ru/`.

## Integration checks

1. Update documentation links to lower-case paths.
2. Use `find_package(CityLifeCore CONFIG REQUIRED)` for installed consumers.
3. Link against `CityLifeCore::core`.
4. Prefer `#include "clc/CityLifeCore.hpp"` for C++ consumers.
5. Prefer tick-based APIs for server-authoritative or real-time simulation flows.

## Related documents

- [CMake package](cmake-package.md)
- [Public API](public-api.md)
- [Compatibility](compatibility.md)
