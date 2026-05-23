# Release Manifest Template / Шаблон release manifest

Status: **1.0.0 release manifest template / шаблон release manifest 1.0.0**

A release manifest helps users verify that an SDK archive, binary package, or checksum belongs to an official City Life Core release.

Release manifest помогает пользователям проверить, что SDK-архив, binary package или checksum принадлежат официальному релизу City Life Core.

---

## Required fields / Обязательные поля

```yaml
project: City Life Core
official_repository: https://github.com/SkrimanS/City_Life_Core
official_owner: SkrimanS
version: 1.0.0
tag: v1.0.0
commit: <full-git-commit-sha>
release_date: YYYY-MM-DD
release_type: stable
license: <chosen-license>

artifacts:
  - name: city-life-core-sdk-1.0.0-<system>-<processor>.zip
    type: source-sdk-archive
    sha256: <sha256>
    size_bytes: <size>

verification:
  signed_tag: true-or-false
  signed_commit: true-or-false
  ci_run_url: <url-or-empty>
  installed_sdk_smoke_test: pass-or-fail
  external_find_package_consumer: pass-or-fail
  external_c_abi_consumer: pass-or-fail
  unpacked_zip_find_package_consumer: pass-or-fail
  unpacked_zip_c_abi_consumer: pass-or-fail
  c_abi_version: 3
  c_abi_world_handle_checked: true-or-false
  c_abi_event_access_checked: true-or-false

public_api:
  readiness_status: docs/READINESS_STATUS.md
  status_document: docs/PUBLIC_API_STATUS.md
  c_abi_document: docs/C_ABI.md
  stable_headers_reviewed: true-or-false
  experimental_headers_documented: true-or-false

release_gates:
  blocker_document: docs/RELEASE_BLOCKERS.md
  ci_artifact_review: docs/CI_ARTIFACT_REVIEW.md
  release_checklist: docs/RELEASE_CHECKLIST.md
  blockers_closed_or_accepted_by_owner: true-or-false
  owner_release_approval: true-or-false

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
commit: 0000000000000000000000000000000000000000
release_date: 2026-06-01
release_type: stable
license: TBD

artifacts:
  - name: city-life-core-sdk-1.0.0-Linux-x86_64.zip
    type: source-sdk-archive
    sha256: 0000000000000000000000000000000000000000000000000000000000000000
    size_bytes: 0

verification:
  signed_tag: false
  signed_commit: false
  ci_run_url: ""
  installed_sdk_smoke_test: pending
  external_find_package_consumer: pending
  external_c_abi_consumer: pending
  unpacked_zip_find_package_consumer: pending
  unpacked_zip_c_abi_consumer: pending
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

## Release process use / Как использовать в релизе

For the 1.0.0 release:

1. Copy this template into a versioned manifest file or fill `docs/RELEASE_MANIFEST_DRAFT_1.0.0.md`.
2. Fill exact tag, commit SHA, date, and artifact names.
3. Generate SHA-256 checksums for all artifacts.
4. Record installed and unpacked ZIP C++/C ABI consumer results, including C ABI version, world-handle checks, and read-only event access checks.
5. Record blocker/owner approval status.
6. Attach manifest and checksums to the release.
7. Link the manifest from release notes.
8. Do not call artifacts official unless they match this manifest.

Для релиза 1.0.0:

1. Скопировать шаблон в versioned manifest file или заполнить `docs/RELEASE_MANIFEST_DRAFT_1.0.0.md`.
2. Заполнить точный tag, commit SHA, дату и названия artifacts.
3. Сгенерировать SHA-256 checksums для всех artifacts.
4. Записать результаты installed и unpacked ZIP C++/C ABI consumer checks, включая C ABI version, world-handle checks и read-only event access checks.
5. Записать blocker/owner approval status.
6. Приложить manifest и checksums к релизу.
7. Сослаться на manifest из release notes.
8. Не называть artifacts официальными, если они не совпадают с manifest.

---

## Anti-impersonation note / Защита от имперсонации

If an archive or binary claims to be an official City Life Core release but does not match the official repository, tag, commit, and artifact hash from the manifest, treat it as unofficial and potentially malicious.

Если архив или бинарник утверждает, что является официальным City Life Core release, но не совпадает с официальным repository, tag, commit и artifact hash из manifest, считайте его неофициальным и потенциально вредоносным.
