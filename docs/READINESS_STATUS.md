# Readiness Status / Статус готовности

Status: **v/1.0-rc-prep snapshot / срез v/1.0-rc-prep**

This document captures the current release-readiness estimate for City Life Core before the remaining owner/CI/artifact gates are closed.

Этот документ фиксирует текущую оценку готовности City Life Core до закрытия оставшихся owner/CI/artifact gates.

---

## Current subsystem readiness

```text
Foundation/Core:             96–98%
Data Registry:               83–87%
Settlement/Storage:          91–94%
Basic Economy/Market:        72–78%
Routes:                      89–93%
Caravans:                    92–95%
Factions/Ownership:          82–87%
Contracts:                   91–94%
Persistence:                 94–97%
Runtime/World Integration:   94–97%
Runtime Events/Diagnostics:  93–96%
Public SDK/API:              91–95%
Packaging/Release:           90–94%
Docs for external users:     92–95%
C ABI:                       50–60%
```

---

## Overall interpretation

The project is close to release-candidate readiness from a source/API/docs/package-shape perspective, but it is not release-ready until the hard gates are closed.

Проект близок к release-candidate готовности с точки зрения source/API/docs/package-shape, но не считается release-ready до закрытия hard gates.

Approximate status:

```text
Technical RC readiness:       high, roughly low-to-mid 90% range
Release/merge readiness:      blocked by LICENSE, CI validation and artifact review
Public final 1.0 readiness:   not yet final; weaker areas remain tracked
```

---

## Hard release gates

The active hard blockers are tracked in:

```text
docs/RELEASE_BLOCKERS.md
```

At minimum, the project still needs:

- owner license/contribution-model decision;
- root `LICENSE` file;
- docs updated to name the chosen license;
- supported CI matrix review;
- benchmark artifact review;
- SDK ZIP artifact and checksum review;
- installed and unpacked ZIP C++/C ABI consumer confirmation;
- release manifest fields filled from reviewed data;
- explicit owner release/merge approval.

---

## Strong areas

These areas are currently near RC shape:

- Foundation/Core;
- Settlement/Storage;
- Caravans;
- Contracts;
- Persistence;
- Runtime/World Integration;
- Runtime Events/Diagnostics;
- Public SDK/API;
- Docs for external users.

---

## Areas to keep hardening

These areas can continue improving before or after RC, depending on owner priorities:

- Basic Economy/Market;
- Data Registry;
- Factions/Ownership;
- C ABI breadth;
- performance baseline history;
- final release governance after the license decision.

---

## C ABI status

The C ABI is intentionally minimal and currently covers:

- core version;
- C interface version;
- time constants and conversions;
- opaque `clc_world` create/destroy;
- world name/seed/current tick/event count;
- tick advancement;
- read-only world event id/tick/type/payload accessors.

It does not expose the full runtime, data registry, save/load, callbacks, caravans, contracts, economy workflows or mutable event payload APIs.

---

## Release decision note

Do not merge to `main` or publish official artifacts until:

- issue #40 is closed or unresolved blockers are explicitly accepted by the owner;
- `docs/RELEASE_BLOCKERS.md` is reviewed;
- `docs/CI_ARTIFACT_REVIEW.md` is followed;
- the owner explicitly approves release/merge.
