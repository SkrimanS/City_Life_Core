# Versioning Policy / Политика версионирования

Status: **1.0.0 versioning policy / политика версионирования 1.0.0**

City Life Core follows semantic versioning starting with the 1.0.0 line.

City Life Core следует semantic versioning начиная с линейки 1.0.0.

---

## Current model

- `1.0.0` — first stable source-first C++ SDK release.
- `1.x.y` — compatible source-level SDK evolution unless release notes explicitly mark an experimental/specialized change.
- `2.0.0+` — reserved for breaking stable API, persistence compatibility or documented event protocol changes.

Pre-1.0 development history is intentionally not expanded in release-facing docs. The supported SDK surface is described by the 1.0.0 documentation and release notes.

---

## Version locations

Mandatory synchronized locations:

- `CMakeLists.txt` — `project(CityLifeCore VERSION X.Y.Z ...)`;
- `include/clc/core/Version.hpp` — `core_version()` and `core_version_string()`;
- `src/clc/c/CityLifeCoreC.cpp` — C ABI core version string;
- `tests/smoke_tests.cpp` — C++ smoke expectation;
- `tests/c_abi_smoke_tests.c` — C ABI smoke expectation;
- `README.md`;
- `CHANGELOG.md`;
- `docs/PUBLIC_API.md`;
- `docs/PUBLIC_API_STATUS.md`;
- `docs/CORE_CONCEPTS.md`;
- `docs/C_ABI.md`;
- `docs/CMAKE_PACKAGE.md`;
- `docs/BUILD_AND_LINKING_POLICY.md`;
- `docs/SDK_STRUCTURE.md`;
- `docs/PACKAGING.md`;
- `docs/SDK_ZIP_PACKAGE.md`;
- version-specific release notes;
- release manifest.

---

## Release tags

Official stable release tags should use:

```text
vMAJOR.MINOR.PATCH
```

For 1.0.0:

```text
v1.0.0
```

If release candidates are used before a future stable release, keep CMake `project(VERSION ...)` numeric and store the RC label in the tag, release notes and manifest rather than in the CMake project version.

---

## Semantic versioning after 1.0.0

After `1.0.0`:

- `PATCH` is for bug fixes without stable API changes.
- `MINOR` is for backward-compatible stable API additions.
- `MAJOR` is for breaking changes in stable API, persistence compatibility or documented event protocol.

Experimental API may change in minor/patch releases when explicitly marked in `PUBLIC_API_STATUS.md` and release notes.

---

## Breaking changes

Breaking changes for stable API include:

- removing or renaming a stable public header;
- removing a stable public function/type/field;
- changing stable runtime workflow semantics;
- changing stable event names or payload schemas;
- changing save/load compatibility policy;
- changing tick scale constants;
- changing CMake target name `CityLifeCore::core`;
- breaking documented C ABI v3 behavior without increasing the C interface version.

---

## Compatibility boundaries

The 1.0.0 public contract is source-first:

- C++ source compatibility is the primary compatibility target.
- C++ binary ABI compatibility is not promised.
- The minimal C ABI has its own interface version, currently `3`.
- Exact benchmark timings are not compatibility promises.
- Experimental and specialized APIs should be used deliberately.

See also:

```text
docs/COMPATIBILITY.md
docs/BUILD_AND_LINKING_POLICY.md
docs/C_ABI.md
docs/RELEASE_NOTES_1.0.0.md
```
