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

## Hard blockers

### 1. License decision

A final license/contribution model must be chosen by the owner before the public `1.0.0` release.

Required follow-up:

- choose license model;
- add root `LICENSE` file;
- update `README.md` license section;
- update package/release docs that mention licensing;
- confirm contribution policy in `CONTRIBUTING.md`;
- review `docs/PROTECTION_STRATEGY.md` against the chosen model.

Until this is done, the release must remain blocked.

### 2. CI validation

The release branch must have reviewed CI validation for the supported matrix:

- `ubuntu-latest / gcc`;
- `ubuntu-latest / clang`;
- `windows-latest / msvc`.

Required follow-up:

- run or rerun CI for the release-validation head;
- inspect failures manually if GitHub API logs/artifacts are unavailable;
- use `docs/CI_ARTIFACT_REVIEW.md` for review rules;
- use local scripts for troubleshooting if needed:
  - `scripts/manual_release_validation.sh`;
  - `scripts/manual_release_validation.ps1`.

### 3. Artifact review

Release artifacts must be reviewed before publishing.

Required follow-up:

- review benchmark artifacts;
- review SDK ZIP artifacts;
- verify `SHA256SUMS.txt`;
- verify installed C++ consumer;
- verify installed C ABI consumer;
- verify unpacked ZIP C++ consumer;
- verify unpacked ZIP C ABI consumer;
- fill `docs/RELEASE_MANIFEST_DRAFT_1.0.0.md` from reviewed results or consciously supersede it with a final manifest.

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
- CI matrix is green or failures are explicitly accepted and documented;
- benchmark artifacts are reviewed;
- SDK ZIP artifacts and checksums are reviewed;
- installed and unpacked ZIP consumers are confirmed;
- `docs/RELEASE_MANIFEST_DRAFT_1.0.0.md` is filled from reviewed data or superseded by a final manifest;
- owner explicitly approves release/merge.
