# Release Manifest 1.0.0

Status: **final / released**

This manifest summarizes the released City Life Core 1.0.0 SDK line.

## Package identity

- Project: City Life Core
- Version: 1.0.0
- License: Apache-2.0
- Primary language: C++20
- Build system: CMake

## Release contents

- Public headers under `include/clc`.
- Core implementation under `src/clc`.
- Example data packs under `data`.
- Examples, tests, benchmarks and manual validation scripts.
- English documentation under `docs`.
- Russian documentation under `docs/ru`.

## Validation scope

The release-validation flow covers configure/build, tests, benchmarks, install, installed C++ and C ABI consumers, CPack ZIP packaging, checksum generation and unpacked ZIP consumer checks.

## Related documents

- [Release notes](release-notes-1.0.0.md)
- [Release verification](verifying-releases.md)
- [CI artifact review](ci-artifact-review.md)
