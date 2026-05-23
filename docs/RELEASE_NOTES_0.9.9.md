# Release Notes 0.9.9 / Заметки релиза 0.9.9

Status: **unreleased / blocked for official publication**

These notes describe the current `0.9.9` SDK/release-candidate preparation state on `v/1.0-rc-prep`.

Эти заметки описывают текущее состояние подготовки SDK/release candidate `0.9.9` в ветке `v/1.0-rc-prep`.

---

## Release status

`0.9.9` is a release-preparation snapshot, not an official public release yet.

Hard gates still apply:

- owner must choose the final LICENSE/contribution model;
- root `LICENSE` must be added;
- CI matrix must be reviewed or failures must be explicitly accepted and documented by the owner;
- benchmark artifacts must be reviewed;
- SDK ZIP artifacts and `SHA256SUMS.txt` must be reviewed;
- installed and unpacked ZIP C++/C ABI consumers must be confirmed;
- draft release manifest must be filled from reviewed data or superseded by a final manifest;
- owner must explicitly approve release/merge.

Do not publish official artifacts or merge to `main` until the release gates are closed.

---

## Highlights

### Public SDK packaging

- Added recommended C++ umbrella header:

```cpp
#include "clc/CityLifeCore.hpp"
```

- Added installed CMake package support for external consumers:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE CityLifeCore::core)
```

- Added standalone C++ package consumer:

```text
examples/find_package_consumer/
```

- Added CPack ZIP SDK package flow with checksum generation.

### Minimal C ABI v3

`0.9.9` introduces a minimal C ABI surface for C/FFI smoke consumption:

```c
#include "clc/c/CityLifeCoreC.h"
```

The C ABI currently covers:

- core version;
- C interface version `3`;
- time constants and conversions;
- opaque `clc_world` create/destroy;
- world name/seed/current tick/event count;
- tick advancement;
- read-only world event id/tick/type/payload accessors.

The C ABI intentionally does not expose full runtime, registries, save/load, callbacks, caravans, contracts, economy workflows or mutable event payload APIs.

### Runtime tick model

The runtime now has stronger tick-based support for real-time and server-authoritative games:

- runtime clock persistence through `SimulationRuntime::time`;
- tick-based route travel;
- tick-based caravan progress;
- tick-based contract deadlines;
- tick-based runtime helpers;
- absolute runtime tick event logs.

Day-based compatibility APIs remain available.

### Persistence and diagnostics

`0.9.9` strengthens runtime persistence and diagnostics:

- runtime clock save/load;
- contract `due_ticks` save/load;
- caravan tick progress save/load;
- settlement tick remainder save/load;
- legacy save compatibility from day-based saves;
- deterministic runtime replay diagnostics;
- event-log drift checks.

### Release governance

Release-preparation documentation now includes:

```text
docs/READINESS_STATUS.md
docs/RELEASE_BLOCKERS.md
docs/CI_ARTIFACT_REVIEW.md
docs/RELEASE_MANIFEST_TEMPLATE.md
docs/RELEASE_MANIFEST_DRAFT_0.9.9.md
docs/RELEASE_CHECKLIST.md
docs/VERIFYING_RELEASES.md
docs/PROTECTION_STRATEGY.md
```

Local troubleshooting helpers are available:

```bash
bash scripts/manual_release_validation.sh
```

```powershell
pwsh -File scripts/manual_release_validation.ps1
```

---

## Compatibility notes

- Existing day-based route, caravan and contract APIs remain available.
- Existing C ABI version/time functions remain available under C interface version `3`.
- Older world-state saves without explicit runtime `time` are still accepted and synchronize runtime clock from saved `current_day`.
- Older contract rows without `due_ticks` are still accepted and derive `due_ticks` from `due_day`.
- C++ source compatibility is still source-first; binary ABI stability is not promised for the C++ API.

---

## Known limitations before official release

The current readiness snapshot is tracked in:

```text
docs/READINESS_STATUS.md
```

Areas still worth hardening before or after RC:

- Basic Economy/Market;
- Data Registry;
- Factions/Ownership;
- C ABI breadth;
- benchmark baseline history;
- final release governance after license decision.

---

## Verification path

Before treating `0.9.9` as an official release candidate:

1. Close or explicitly accept hard blockers in issue #40.
2. Review `docs/RELEASE_BLOCKERS.md`.
3. Review `docs/READINESS_STATUS.md`.
4. Run CI or local fallback validation.
5. Review benchmark artifacts.
6. Review SDK ZIP artifacts and `SHA256SUMS.txt`.
7. Confirm installed and unpacked ZIP consumers.
8. Fill `docs/RELEASE_MANIFEST_DRAFT_0.9.9.md` or replace it with a final manifest.
9. Receive explicit owner release/merge approval.
