# Release Manifest Template / Шаблон release manifest

Status: **template for future releases / шаблон для будущих релизов**

A release manifest helps users verify that an SDK archive, binary package, or checksum belongs to an official City Life Core release.

Release manifest помогает пользователям проверить, что SDK-архив, binary package или checksum принадлежат официальному релизу City Life Core.

---

## Required fields / Обязательные поля

```yaml
project: City Life Core
official_repository: https://github.com/SkrimanS/City_Life_Core
official_owner: SkrimanS
version: <version>
tag: v<version>
commit: <full-git-commit-sha>
release_date: YYYY-MM-DD
release_type: stable
license: Apache-2.0
license_file: LICENSE

artifacts:
  - name: city-life-core-sdk-<version>-<system>-<processor>.zip
    type: source-sdk-archive
    platform: <system>
    architecture: <processor>
    sha256: <sha256>
    size_bytes: <size-or-not-recorded>

verification:
  signed_tag: true-or-false
  signed_commit: true-or-false
  ci_run_url: <url-or-deferred>
  ci_matrix_reviewed: true-or-false-or-deferred
  linux_gcc_reviewed: true-or-false-or-deferred
  linux_clang_reviewed: true-or-false-or-deferred
  windows_msvc_reviewed: true-or-false-or-deferred
  installed_sdk_smoke_test: passed-or-failed-or-deferred
  external_find_package_consumer: passed-or-failed-or-deferred
  external_c_abi_consumer: passed-or-failed-or-deferred
  unpacked_zip_find_package_consumer: passed-or-failed-or-deferred
  unpacked_zip_c_abi_consumer: passed-or-failed-or-deferred
  c_abi_version: <number>
  c_abi_world_handle_checked: true-or-false
  c_abi_event_access_checked: true-or-false

public_api:
  readiness_status: docs/READINESS_STATUS.md
  status_document: docs/PUBLIC_API_STATUS.md
  c_abi_document: docs/C_ABI.md
  release_notes: docs/RELEASE_NOTES_<version>.md
  stable_headers_reviewed: true-or-false
  experimental_headers_documented: true-or-false

follow_up:
  post_release_validation_issue: <issue-number-or-none>
  github_actions: completed-or-deferred
  linux_gcc: completed-or-deferred
  linux_clang: completed-or-deferred

security:
  security_policy: SECURITY.md
  verifying_releases: docs/VERIFYING_RELEASES.md
  codeowners: .github/CODEOWNERS
```

---

## Example / Пример

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
  installed_sdk_smoke_test: passed
  external_find_package_consumer: passed
  external_c_abi_consumer: passed
  unpacked_zip_find_package_consumer: passed
  unpacked_zip_c_abi_consumer: passed
  c_abi_version: 3
  c_abi_world_handle_checked: true
  c_abi_event_access_checked: true

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

## Release process use / Как использовать в релизе

For a future release:

1. Copy this template into a versioned manifest file such as `docs/RELEASE_MANIFEST_<version>.md`.
2. Fill exact tag, commit SHA, date, artifact names and artifact hashes.
3. Record installed and unpacked ZIP C++/C ABI consumer results.
4. Record CI status or explicit owner-approved deferrals.
5. Attach or link manifest and checksums from the GitHub Release.
6. Link the manifest from release notes.
7. Do not call artifacts official unless they match the official repository, tag, commit and artifact hash from the manifest.

Для будущего релиза:

1. Скопировать шаблон в versioned manifest file, например `docs/RELEASE_MANIFEST_<version>.md`.
2. Заполнить точный tag, commit SHA, дату, названия artifacts и hashes artifacts.
3. Записать результаты installed и unpacked ZIP C++/C ABI consumer checks.
4. Записать CI status или явно принятые owner deferrals.
5. Приложить или сослаться на manifest и checksums из GitHub Release.
6. Сослаться на manifest из release notes.
7. Не называть artifacts официальными, если они не совпадают с official repository, tag, commit и artifact hash из manifest.

---

## Anti-impersonation note / Защита от имперсонации

If an archive or binary claims to be an official City Life Core release but does not match the official repository, tag, commit and artifact hash from the manifest, treat it as unofficial and potentially malicious.

Если архив или бинарник утверждает, что является официальным City Life Core release, но не совпадает с official repository, tag, commit и artifact hash из manifest, считайте его неофициальным и потенциально вредоносным.
