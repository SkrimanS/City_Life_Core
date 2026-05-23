# Draft Release Manifest 0.9.9 / Черновик release manifest 0.9.9

Status: **blocked / не готов к публикации**

This is a draft manifest for the current `v/1.0-rc-prep` validation branch. It must not be used as an official release manifest until LICENSE, CI validation, artifact review and owner approval are complete.

Это черновик manifest для текущей validation branch `v/1.0-rc-prep`. Его нельзя использовать как официальный release manifest, пока не закрыты LICENSE, CI validation, artifact review и owner approval.

---

## Manifest draft

```yaml
project: City Life Core
official_repository: https://github.com/SkrimanS/City_Life_Core
official_owner: SkrimanS
version: 0.9.9
release_candidate_for: 1.0.0-rc1
tag: TBD
commit: TBD-current-validation-head
release_date: TBD
release_type: rc-prep
license: TBD-owner-decision-required
status: blocked

artifacts:
  - name: TBD-city-life-core-sdk-0.9.9-<system>-<processor>.zip
    type: source-sdk-archive
    sha256: TBD
    size_bytes: TBD

verification:
  signed_tag: TBD
  signed_commit: TBD
  ci_run_url: TBD
  ci_matrix_reviewed: false
  linux_gcc_reviewed: false
  linux_clang_reviewed: false
  windows_msvc_reviewed: false
  installed_sdk_smoke_test: pending
  external_find_package_consumer: pending
  external_c_abi_consumer: pending
  unpacked_zip_find_package_consumer: pending
  unpacked_zip_c_abi_consumer: pending
  benchmark_artifacts_reviewed: false
  sdk_zip_artifacts_reviewed: false
  sha256sums_reviewed: false
  c_abi_version: 3
  c_abi_world_handle_checked: false
  c_abi_event_access_checked: false

public_api:
  readiness_status: docs/READINESS_STATUS.md
  status_document: docs/PUBLIC_API_STATUS.md
  c_abi_document: docs/C_ABI.md
  stable_headers_reviewed: false
  experimental_headers_documented: true

release_gates:
  blocker_issue: 40
  blocker_document: docs/RELEASE_BLOCKERS.md
  ci_artifact_review: docs/CI_ARTIFACT_REVIEW.md
  release_checklist: docs/RELEASE_CHECKLIST.md
  blockers_closed_or_accepted_by_owner: false
  owner_release_approval: false

security:
  security_policy: SECURITY.md
  verifying_releases: docs/VERIFYING_RELEASES.md
  codeowners: .github/CODEOWNERS
```

---

## Required completion before official use

Before this can become an official manifest:

- owner chooses LICENSE/contribution model;
- root `LICENSE` is added;
- license is reflected in README/package/release docs;
- CI matrix is green or failures are explicitly accepted and documented by owner;
- benchmark artifacts are reviewed;
- SDK ZIP artifacts are reviewed;
- `SHA256SUMS.txt` is reviewed;
- installed C++ and C ABI consumers are confirmed;
- unpacked ZIP C++ and C ABI consumers are confirmed;
- C ABI world-handle and read-only event access checks are confirmed;
- final tag, commit SHA, release date, artifact name, size and checksum are filled;
- issue #40 is closed or unresolved blockers are explicitly accepted by owner;
- owner explicitly approves release/merge.

---

## Notes

Use `docs/RELEASE_MANIFEST_TEMPLATE.md` for the canonical field format. This file is only a branch-local draft for the current validation state.
