# Versioning Policy / Политика версионирования

Status: **draft for 1.0.0-rc1 / черновик для 1.0.0-rc1**

City Life Core follows semantic versioning starting with the 1.0.0 line. The current 0.9.x series is a pre-1.0 audit and stabilization series.

City Life Core следует semantic versioning начиная с линейки 1.0.0. Текущая серия 0.9.x является pre-1.0 серией аудита и стабилизации.

---

## Русский

### Текущая модель

- `0.9.x` — pre-1.0 audit/stabilization builds.
- `1.0.0-rcN` — release candidates перед первым stable SDK.
- `1.0.0` — первый stable source-first C++ SDK release.

### Где хранится версия

Обязательные места синхронизации:

- `CMakeLists.txt` — `project(CityLifeCore VERSION X.Y.Z ...)`;
- `include/clc/core/Version.hpp` — `core_version()` и `core_version_string()`;
- `tests/smoke_tests.cpp` — expectation текущей версии;
- `README.md`;
- `CHANGELOG.md`;
- `docs/PUBLIC_API.md`;
- `docs/SDK_STRUCTURE.md`;
- `docs/PACKAGING.md`;
- release notes для конкретной версии.

### Release candidates

CMake `project(VERSION ...)` принимает numeric version. Поэтому для tag `v1.0.0-rc1` рекомендуется:

- держать CMake version как `1.0.0`;
- хранить rc label в release notes, tag, changelog heading и, при необходимости, отдельном API вроде `core_release_label()`;
- не ломать package version comparison из-за non-numeric prerelease suffix.

### Semver после 1.0.0

После `1.0.0`:

- `PATCH` увеличивается для bug fixes без изменения stable API;
- `MINOR` увеличивается для backward-compatible stable API additions;
- `MAJOR` увеличивается для breaking changes в stable API, persistence compatibility или documented event protocol.

Experimental API может меняться в minor/patch версиях, если это явно указано в `PUBLIC_API_STATUS.md` и release notes.

### Что считается breaking change

Breaking changes для stable API:

- удаление или переименование stable public header;
- удаление stable public function/type/field;
- изменение semantics stable runtime workflow;
- изменение stable event name или payload schema;
- изменение save/load compatibility policy;
- изменение tick scale constants;
- изменение CMake target name `CityLifeCore::core`.

### Что не считается stable до отдельного freeze

- C ABI;
- binary ABI;
- exact binary package layout;
- internal serialization implementation details outside documented compatibility policy;
- benchmark thresholds;
- experimental headers and helpers.

---

## English

### Current model

- `0.9.x` — pre-1.0 audit/stabilization builds.
- `1.0.0-rcN` — release candidates before the first stable SDK.
- `1.0.0` — first stable source-first C++ SDK release.

### Version locations

Mandatory synchronized locations:

- `CMakeLists.txt` — `project(CityLifeCore VERSION X.Y.Z ...)`;
- `include/clc/core/Version.hpp` — `core_version()` and `core_version_string()`;
- `tests/smoke_tests.cpp` — current version expectation;
- `README.md`;
- `CHANGELOG.md`;
- `docs/PUBLIC_API.md`;
- `docs/SDK_STRUCTURE.md`;
- `docs/PACKAGING.md`;
- version-specific release notes.

### Release candidates

CMake `project(VERSION ...)` accepts a numeric version. For tag `v1.0.0-rc1`, the recommended policy is:

- keep CMake version as `1.0.0`;
- store the rc label in release notes, tag, changelog heading, and optionally a separate API such as `core_release_label()`;
- avoid breaking package version comparison with a non-numeric prerelease suffix.

### Semver after 1.0.0

After `1.0.0`:

- `PATCH` is for bug fixes without stable API changes;
- `MINOR` is for backward-compatible stable API additions;
- `MAJOR` is for breaking changes in stable API, persistence compatibility, or documented event protocol.

Experimental API may change in minor/patch releases when explicitly marked in `PUBLIC_API_STATUS.md` and release notes.

### Breaking changes

Breaking changes for stable API include:

- removing or renaming a stable public header;
- removing a stable public function/type/field;
- changing stable runtime workflow semantics;
- changing stable event names or payload schemas;
- changing save/load compatibility policy;
- changing tick scale constants;
- changing CMake target name `CityLifeCore::core`.

### Not stable until explicitly frozen

- C ABI;
- binary ABI;
- exact binary package layout;
- internal serialization implementation details outside documented compatibility policy;
- benchmark thresholds;
- experimental headers and helpers.
