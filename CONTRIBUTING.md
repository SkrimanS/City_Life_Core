# Contributing / Участие в разработке

City Life Core is currently in pre-1.0 SDK preparation. Until the project owner explicitly chooses the final license and contribution model, external code contributions are limited and require maintainer approval before review or merge.

City Life Core сейчас находится на этапе подготовки pre-1.0 SDK. Пока владелец проекта явно не выбрал финальную лицензию и модель contribution, внешние code contributions ограничены и требуют одобрения maintainer перед review или merge.

---

## Current contribution status

Current status: **restricted contributions**.

This means:

- unsolicited large code drops may be closed without review;
- maintainers may accept issues, audit reports and documentation feedback;
- code changes from external contributors require explicit maintainer approval before review;
- no contribution is accepted as official until it is merged by an authorized maintainer;
- no fork, mirror, package or rebuild may claim to be the official City Life Core distribution unless it is published through the official project release channel.

This policy protects the project while the license, public SDK boundary and release process are being finalized.

---

## Legal and authorship requirements

Before submitting code, contributors must be able to confirm that:

- they wrote the contribution themselves or have the right to submit it;
- the contribution does not include copied code without permission;
- the contribution does not include malware, backdoors, credential theft, telemetry hidden from maintainers, or intentionally obfuscated behavior;
- the contribution does not intentionally weaken validation, persistence, release verification, package checks, or project protection mechanisms;
- the contribution does not impersonate the project owner or another maintainer.

A final DCO/CLA policy is not selected yet. If external code contributions are accepted later, the maintainer may require a DCO, CLA, signed-off commits or another explicit contributor certification process before merge.

---

## Branches

- `main` — stable integration branch.
- `develop` — future development integration branch.
- `v/*` — version/release preparation branches.
- `feature/*` — feature work.
- `bootstrap/*` — early repository/bootstrap work.

Protected branches should not be force-pushed. Release tags and artifacts should be created only through the official maintainer release process.

---

## Pull request expectations

Every PR should include:

- a clear summary;
- tests or a reason tests are not applicable;
- notes about public API changes;
- notes about simulation invariants affected by the change;
- notes about persistence/save/load compatibility if relevant;
- notes about event payload compatibility if relevant;
- notes about package/install/CI impact if relevant.

Every non-documentation PR should pass the normal build and test workflow before merge.

---

## Security-sensitive changes

Security-sensitive changes require extra review. This includes changes to:

- CI workflows;
- release artifact generation;
- checksums/manifests;
- CMake install/export/package config;
- persistence and deserialization;
- runtime restore;
- event validation;
- ownership/permission checks;
- ledger/reward flows;
- CODEOWNERS or branch protection related files.

Do not submit vulnerability reports as public issues if they include exploitable details. Follow `SECURITY.md`.

---

## C++ style baseline

- C++20 minimum.
- Prefer explicit types at API boundaries.
- Prefer typed IDs over raw integers in public APIs.
- Prefer workflow/helper APIs over direct runtime state mutation in integration code.
- Avoid floating point for money/resource ledgers.
- Validate overflow-sensitive arithmetic.
- Keep save/load and event payload changes explicit and documented.
- Keep public headers stable once marked stable-candidate.

---

## Documentation style

Developer documentation should explain what the SDK is and how to use it. Avoid roadmap-only language in user-facing docs unless the document is explicitly a release checklist, migration guide or maintainer policy document.

Preferred docs focus:

- what the core provides;
- how to include and link it;
- how to create data and runtime state;
- how to use workflows;
- how to validate errors;
- how to save/load and verify packages;
- what is stable, experimental or specialized.
