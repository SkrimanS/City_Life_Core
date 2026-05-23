# Release Manifest 1.0.0 / Release manifest 1.0.0

Status: **final / released**

```yaml
project: City Life Core
official_repository: https://github.com/SkrimanS/City_Life_Core
official_owner: SkrimanS
version: 1.0.0
tag: v1.0.0
commit: d58af03dfc776cf24cbfd34cc07d47b07210bd0a
release_date: 2026-05-23
release_type: stable
license: Apache-2.0
license_file: LICENSE
status: released

artifacts:
  - name: city-life-core-sdk-1.0.0-Windows-AMD64.zip
    type: source-sdk-archive
    platform: Windows
    architecture: AMD64
    sha256: 537c2fbd55d2a41cd6a09e24583a4f82a7f62f6e1818e382223c30f862d90230
    size_bytes: not-recorded

verification:
  signed_tag: false
  signed_commit: false
  ci_run_url: deferred-post-release
  ci_matrix_reviewed: deferred-post-release
  linux_gcc_reviewed: deferred-post-release
  linux_clang_reviewed: deferred-post-release
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

follow_up:
  post_release_validation_issue: 41
  github_actions: deferred-post-release
  linux_gcc: deferred-post-release
  linux_clang: deferred-post-release

security:
  security_policy: SECURITY.md
  verifying_releases: docs/VERIFYING_RELEASES.md
  codeowners: .github/CODEOWNERS
```

---

## Validation summary

Windows/MSVC local release validation was accepted as the release gate for `1.0.0`.

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

## Deferred verification

GitHub Actions and Linux GCC/Clang validation were consciously deferred to post-release verification and are tracked in issue #41.
