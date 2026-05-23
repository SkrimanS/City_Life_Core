# Draft Release Manifest 1.0.0 / Черновик release manifest 1.0.0

Status: **blocked / local Windows validation passed / не готов к публикации**

This is a draft manifest for the current `v/1.0-rc-prep` validation branch. It must not be used as an official release manifest until LICENSE, CI validation or owner-accepted local validation scope, artifact review and owner approval are complete.

Это черновик manifest для текущей validation branch `v/1.0-rc-prep`. Его нельзя использовать как официальный release manifest, пока не закрыты LICENSE, CI validation или явно принятый owner local-validation scope, artifact review и owner approval.

---

## Manifest draft

```yaml
project: City Life Core
official_repository: https://github.com/SkrimanS/City_Life_Core
official_owner: SkrimanS
version: 1.0.0
tag: v1.0.0
commit: TBD-final-release-commit
release_date: TBD
release_type: stable
license: TBD-owner-decision-required
status: blocked-local-windows-validation-passed

artifacts:
  - name: city-life-core-sdk-1.0.0-Windows-AMD64.zip
    type: source-sdk-archive
    platform: Windows
    architecture: AMD64
    sha256: 537c2fbd55d2a41cd6a09e24583a4f82a7f62f6e1818e382223c30f862d90230
    size_bytes: TBD

verification:
  signed_tag: TBD
  signed_commit: TBD
  ci_run_url: TBD
  ci_matrix_reviewed: false
  linux_gcc_reviewed: false
  linux_clang_reviewed: false
  windows_msvc_reviewed: true
  windows_msvc_validation_type: local-manual-release-validation
  windows_msvc_generator: Visual Studio 18 2026
  windows_msvc_compiler: MSVC 19.50.35729.0
  windows_sdk: 10.0.26100.0
  installed_sdk_smoke_test: passed
  external_find_package_consumer: passed
  external_c_abi_consumer: passed
  unpacked_zip_find_package_consumer: passed
  unpacked_zip_c_abi_consumer: passed
  benchmark_artifacts_reviewed: true
  sdk_zip_artifacts_reviewed: true
  sha256sums_reviewed: true
  c_abi_version: 3
  c_abi_world_handle_checked: true
  c_abi_event_access_checked: true
  tests:
    ctest_total: 58
    ctest_passed: 58
    ctest_failed: 0

public_api:
  readiness_status: docs/READINESS_STATUS.md
  status_document: docs/PUBLIC_API_STATUS.md
  c_abi_document: docs/C_ABI.md
  release_notes: docs/RELEASE_NOTES_1.0.0.md
  stable_headers_reviewed: true
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

## Reviewed local validation result

Windows/MSVC local validation was completed from `v/1.0-rc-prep` using `scripts/manual_release_validation.ps1`.

Confirmed flow:

- configure with tests, examples and benchmarks enabled;
- Release build completed;
- `ctest` passed: 58/58 tests;
- benchmark runner completed;
- `cmake --install` completed;
- installed C++ `find_package` consumer built and ran;
- installed C ABI consumer built and ran;
- CPack ZIP generated;
- SHA256 checksum generated;
- unpacked ZIP C++ consumer built and ran;
- unpacked ZIP C ABI consumer built and ran.

Reviewed output:

```text
SDK ZIP: city-life-core-sdk-1.0.0-Windows-AMD64.zip
SHA256: 537c2fbd55d2a41cd6a09e24583a4f82a7f62f6e1818e382223c30f862d90230
C ABI interface version: 3
C++ consumer output: 1.0.0 300
C ABI consumer output: version=1.0.0
```

---

## Required completion before official use

Before this can become an official manifest:

- owner chooses LICENSE/contribution model;
- root `LICENSE` is added;
- license is reflected in README/package/release docs;
- CI matrix is green or failures are explicitly accepted and documented by owner;
- Linux GCC and Linux Clang validation are reviewed or explicitly deferred/accepted by owner;
- final tag, commit SHA, release date and artifact size are filled;
- issue #40 is closed or unresolved blockers are explicitly accepted by owner;
- owner explicitly approves release/merge.

---

## Notes

Use `docs/RELEASE_MANIFEST_TEMPLATE.md` for the canonical field format. This file is still a branch-local draft for the current validation state and remains blocked until the owner closes or accepts the remaining gates.
