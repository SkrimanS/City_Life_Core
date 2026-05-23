# Verifying Official Releases / Проверка официальных релизов

Status: **draft for 1.0.0-rc1 / черновик для 1.0.0-rc1**

This guide helps users distinguish official City Life Core releases from forks, modified archives, fake packages, or malicious builds.

Этот документ помогает отличать официальные релизы City Life Core от форков, изменённых архивов, фейковых пакетов или вредоносных сборок.

---

## Official source / Официальный источник

Official repository:

```text
https://github.com/SkrimanS/City_Life_Core
```

Official owner/account:

```text
SkrimanS
```

Anything not linked from the official repository, official release notes, or official package manifest should be treated as unofficial.

Всё, что не указано в официальном репозитории, release notes или package manifest, следует считать неофициальным.

---

## What makes a release official?

A release should be considered official only when all of these are true:

- it is published from the official repository;
- it has a tag or commit SHA listed in official release notes;
- release artifacts have checksums or a manifest when artifacts are provided;
- CI, installed SDK consumer checks, and unpacked SDK consumer checks are linked or recorded in the manifest;
- the package name and namespace match official documentation;
- the release is not marked as revoked or compromised.

---

## Recommended verification flow

1. Open the official repository.
2. Check the release/tag name.
3. Compare the commit SHA with release notes.
4. Verify artifact checksums when provided.
5. Check the release manifest for CI and consumer validation results.
6. Prefer signed tags or signed release commits when available.
7. Build from source if a binary package cannot be verified.
8. Do not trust third-party mirrors unless they are listed in official release notes.

---

## Checksums

Release artifacts should include checksums, for example:

```text
city-life-core-sdk-1.0.0-rc1.zip
SHA256SUMS.txt
```

Users should compare the downloaded artifact checksum with the official checksum before use.

---

## Release manifest

A release manifest should include enough information to verify both artifact identity and package usability:

```yaml
project: City Life Core
official_repository: https://github.com/SkrimanS/City_Life_Core
version: 1.0.0-rc1
commit: <commit-sha>
tag: v1.0.0-rc1
artifacts:
  - name: city-life-core-sdk-1.0.0-rc1.zip
    sha256: <sha256>
verification:
  ci_run_url: <url-or-empty>
  installed_sdk_smoke_test: pass-or-fail
  external_find_package_consumer: pass-or-fail
  external_c_abi_consumer: pass-or-fail
  unpacked_zip_find_package_consumer: pass-or-fail
  unpacked_zip_c_abi_consumer: pass-or-fail
  c_abi_version: <number>
  c_abi_world_handle_checked: true-or-false
  c_abi_event_access_checked: true-or-false
```

For the current 0.9.9/1.0-rc-prep SDK shape, the C ABI consumer should validate the C ABI version, time utilities, the minimal opaque `clc_world` handle, and read-only world event accessors. The C++ consumer should validate normal `find_package(CityLifeCore CONFIG REQUIRED)` usage through the exported `CityLifeCore::core` target.

---

## Warning signs

Treat a build as suspicious if:

- it claims to be official but comes from another account;
- it has no release notes;
- it has no commit SHA;
- it has no checksum or checksum mismatch;
- it has no CI or consumer validation results for distributed SDK artifacts;
- it asks users to disable security checks;
- it changes namespace/package name while claiming official status;
- it is distributed only through private messages or unknown mirrors.

---

## Reporting fake or malicious builds

If you find a fake official release, impersonation attempt, or malicious package:

- do not run the package;
- preserve the URL and artifact hash if safe;
- report it privately to the project owner;
- do not publicly share exploit details before the owner has a chance to respond.

---

## Future 3.x+ distribution model

For 1.x/2.x, City Life Core may remain a source-first SDK while the public API stabilizes.

For 3.x and later, the owner may choose a more protected distribution model, such as:

- closed core with public headers and extension points;
- source-available core with restricted redistribution;
- dual-license model;
- official binary SDK plus mod/plugin API;
- signed official packages only.

The goal is not to block legitimate modding. The goal is to keep the official core identifiable, verifiable, and protected from impersonation or malicious redistribution.

Цель — не запретить нормальный моддинг. Цель — чтобы официальное ядро можно было отличить, проверить и защитить от имперсонации или вредоносного распространения.
