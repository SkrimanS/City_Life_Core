# Release Notes 1.0.0 / Заметки релиза 1.0.0

Status: **release-ready / merged to main**

City Life Core `1.0.0` is the first stable SDK release line for the public C++20 headless simulation core.

The `1.0.0` preparation branch was merged to `main` after accepted Windows/MSVC local release validation. GitHub Actions and Linux GCC/Clang validation were consciously deferred to post-merge verification and are tracked separately.

---

## Release scope

Accepted release gate:

- Windows/MSVC local release validation passed and was accepted as the release gate for this candidate.
- `ctest` passed: 58/58.
- Benchmarks completed.
- `cmake --install` completed.
- Installed C++ `find_package` consumer passed.
- Installed C ABI consumer passed.
- CPack SDK ZIP was generated.
- SHA256 checksum was generated and reviewed.
- Unpacked ZIP C++ consumer passed.
- Unpacked ZIP C ABI consumer passed.

Reviewed local artifact:

```text
city-life-core-sdk-1.0.0-Windows-AMD64.zip
537c2fbd55d2a41cd6a09e24583a4f82a7f62f6e1818e382223c30f862d90230
```

License:

```text
Apache-2.0
```

---

## Highlights

### Public SDK packaging

- Recommended C++ umbrella header:

```cpp
#include "clc/CityLifeCore.hpp"
```

- Installed CMake package support for external consumers:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

- Standalone installed-package consumers:

```text
examples/find_package_consumer/   # C++ consumer
examples/c_abi_consumer/          # C consumer for the minimal C interface
```

- CPack ZIP SDK package flow with checksum generation and unpacked ZIP consumer validation.

### Minimal C ABI v3

`1.0.0` ships a minimal C ABI surface for C/FFI smoke consumption:

```c
#include "clc/c/CityLifeCoreC.h"
```

The C ABI currently covers:

- core version;
- C interface version `3`;
- time constants and conversions;
- opaque `clc_world` create/destroy;
- world name, seed, current tick and event count;
- tick advancement;
- read-only world event id, tick, type and payload accessors.

The C ABI intentionally does not expose the full runtime, registries, save/load, callbacks, caravans, contracts, economy workflows or mutable event payload APIs.

### Runtime tick model

The runtime has tick-based support for real-time and server-authoritative games:

- runtime clock persistence through `SimulationRuntime::time`;
- tick-based route travel;
- tick-based caravan progress;
- tick-based contract deadlines;
- tick-based runtime helpers;
- absolute runtime tick event logs.

Day-based compatibility APIs remain available.

### Persistence and diagnostics

`1.0.0` includes runtime persistence and diagnostics coverage for:

- runtime clock save/load;
- contract `due_ticks` save/load;
- caravan tick progress save/load;
- settlement tick remainder save/load;
- legacy save compatibility from day-based saves;
- deterministic runtime replay diagnostics;
- event-log drift checks.

### Release governance

Release documentation includes:

```text
docs/READINESS_STATUS.md
docs/RELEASE_BLOCKERS.md
docs/CI_ARTIFACT_REVIEW.md
docs/RELEASE_MANIFEST_TEMPLATE.md
docs/RELEASE_MANIFEST_DRAFT_1.0.0.md
docs/RELEASE_CHECKLIST.md
docs/VERIFYING_RELEASES.md
docs/PROTECTION_STRATEGY.md
```

Documentation was cleaned for the 1.0.0 release path. Obsolete pre-1.0 and 0.9.x release-note files are excluded from installed docs, release-facing docs point at 1.0.0, and the changelog is focused on the supported 1.0.0 SDK surface.

Local troubleshooting helpers are available:

```bash
bash scripts/manual_release_validation.sh
```

```powershell
powershell -ExecutionPolicy Bypass -File scripts/manual_release_validation.ps1
```

---

## Compatibility notes

- Existing day-based route, caravan and contract APIs remain available.
- Existing C ABI version/time functions remain available under C interface version `3`.
- Older world-state saves without explicit runtime `time` are still accepted and synchronize runtime clock from saved `current_day`.
- Older contract rows without `due_ticks` are still accepted and derive `due_ticks` from `due_day`.
- C++ source compatibility is source-first; binary ABI stability is not promised for the C++ API.

---

## Known limitations and follow-up

Post-merge validation and hardening are tracked separately:

- GitHub Actions status for `main`;
- Linux GCC validation;
- Linux Clang validation;
- stronger Basic Economy/Market hardening;
- more Data Registry data-pack coverage;
- more Factions/Ownership scenario coverage;
- broader C ABI surface if C/FFI users become a target audience;
- performance baseline history across multiple CI runs.

Primary follow-up issue:

```text
#41 Post-merge CI and Linux validation for 1.0.0
```

---

## Verification path

For local verification on Windows:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/manual_release_validation.ps1
```

For local verification on Linux/macOS-like shells:

```bash
bash scripts/manual_release_validation.sh
```

A successful full validation performs configure/build, tests, benchmarks, install, installed consumers, SDK ZIP creation, checksum generation and unpacked ZIP consumer checks.
