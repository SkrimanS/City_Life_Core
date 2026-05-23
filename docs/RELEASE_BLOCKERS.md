# Release Blockers / Блокеры релиза

Status: **active 1.0.0 release gate / активный release gate 1.0.0**

This document lists blockers that must be resolved before City Life Core `1.0.0` can be merged to `main` or published as an official public release.

Этот документ перечисляет блокеры, которые нужно закрыть перед merge City Life Core `1.0.0` в `main` или публикацией official public release.

Current readiness snapshot:

```text
docs/READINESS_STATUS.md
```

Draft release manifest:

```text
docs/RELEASE_MANIFEST_DRAFT_1.0.0.md
```

---

## Completed validation

### Windows/MSVC local release validation

Status: **passed**

Validated locally from `v/1.0-rc-prep` with:

- Visual Studio 18 2026 generator;
- MSVC 19.50.35729.0;
- Windows SDK 10.0.26100.0.

Confirmed flow:

- Release build completed;
- `ctest` passed: 58/58;
- benchmarks completed;
- install completed;
- installed C++ consumer passed;
- installed C ABI consumer passed;
- CPack ZIP generated;
- SHA256 generated;
- unpacked ZIP C++ consumer passed;
- unpacked ZIP C ABI consumer passed.

Reviewed artifact:

```text
city-life-core-sdk-1.0.0-Windows-AMD64.zip
```

SHA256:

```text
537c2fbd55d2a41cd6a09e24583a4f82a7f62f6e1818e382223c30f862d90230
```

---

## Hard blockers

### 1. License decision

Status: **open**

A final license/contribution model must be chosen by the owner before the public `1.0.0` release.

Required follow-up:

- choose license model;
- add root `LICENSE` file;
- update `README.md` license section;
- update package/release docs that mention licensing;
- confirm contribution policy in `CONTRIBUTING.md`;
- review `docs/PROTECTION_STRATEGY.md` against the chosen model.

Until this is done, the release must remain blocked.

### 2. CI / platform validation decision

Status: **partially complete**

Windows/MSVC local validation is complete. The owner must still decide whether official CI validation is required before merge/release or whether the local Windows validation is accepted for this release candidate.

Supported CI matrix normally includes:

- `ubuntu-latest / gcc`;
- `ubuntu-latest / clang`;
- `windows-latest / msvc`.

Required follow-up:

- run or rerun CI for the release-validation head, or explicitly accept/defer unavailable CI coverage;
- inspect failures manually if GitHub API logs/artifacts are unavailable;
- use `docs/CI_ARTIFACT_REVIEW.md` for review rules;
- use local scripts for troubleshooting if needed:
  - `scripts/manual_release_validation.sh`;
  - `scripts/manual_release_validation.ps1`.

### 3. Artifact review

Status: **Windows local artifact reviewed / remaining scope depends on owner CI decision**

Windows local release artifact review is complete for:

- benchmark output;
- installed SDK smoke path;
- installed C++ consumer;
- installed C ABI consumer;
- SDK ZIP generation;
- SHA256 checksum;
- unpacked ZIP C++ consumer;
- unpacked ZIP C ABI consumer.

Required follow-up:

- decide whether Linux/official CI artifacts are required before release;
- fill final manifest fields that depend on final commit/tag/release date/artifact size;
- complete or supersede `docs/RELEASE_MANIFEST_DRAFT_1.0.0.md`.

---

## Current validation PR

Draft PR #39 is validation-only.

Rules:

- do not merge until the owner explicitly approves release/merge;
- do not remove draft status until hard blockers are closed or explicitly deferred;
- do not publish artifacts as official until checksums and manifest are reviewed;
- keep `main` untouched until release/merge approval.

---

## Non-blocking but recommended before or after 1.0.0

These can be consciously deferred from `1.0.0`, but should remain tracked as follow-up hardening:

- stronger Basic Economy/Market hardening;
- more Data Registry data-pack coverage;
- more Factions/Ownership scenario coverage;
- broader C ABI surface if C/FFI users become a target audience;
- performance baseline history across multiple CI runs.

---

## Exit criteria

The release can be merged/published only when:

- license decision is complete;
- CI/platform validation scope is green or explicitly accepted/deferred by owner;
- benchmark artifacts are reviewed for the accepted scope;
- SDK ZIP artifacts and checksums are reviewed for the accepted scope;
- installed and unpacked ZIP consumers are confirmed for the accepted scope;
- `docs/RELEASE_MANIFEST_DRAFT_1.0.0.md` is filled from reviewed data or superseded by a final manifest;
- owner explicitly approves release/merge.
