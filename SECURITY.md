# Security Policy / Политика безопасности

This document defines the official security and distribution policy for City Life Core.

Этот документ фиксирует официальную политику безопасности и распространения City Life Core.

---

## Official project identity / Официальная идентичность проекта

Official repository:

```text
https://github.com/SkrimanS/City_Life_Core
```

Official project owner identity for this repository:

```text
SkrimanS
```

Official C++ namespace/package identity:

```text
clc
CityLifeCore::core
```

Only releases, tags, archives, packages, and checksums published through the official repository or explicitly listed official channels should be treated as official City Life Core releases.

Только релизы, теги, архивы, пакеты и checksums, опубликованные через официальный репозиторий или явно указанные официальные каналы, следует считать официальными релизами City Life Core.

---

## Impersonation warning / Предупреждение об имперсонации

A fork, copy, mirror, archive, package, binary, or website is **not official** unless it is linked from the official repository or official release notes.

Форк, копия, зеркало, архив, пакет, бинарник или сайт **не являются официальными**, если они не указаны в официальном репозитории или официальных release notes.

Do not trust claims such as:

- “official City Life Core build” from another account;
- “SkrimanS release” outside official channels;
- modified archives without official checksums;
- binaries distributed without release notes and provenance;
- packages that are not referenced by the official repository.

---

## Reporting security issues / Сообщение о проблемах безопасности

Report these issues privately to the project owner before public disclosure:

- malicious code injection;
- suspicious commits or releases;
- impersonation attempts;
- fake official packages/builds;
- compromised credentials or tokens;
- vulnerabilities that can corrupt saves, execute code, or compromise build systems;
- dependency or release artifact tampering.

Until a dedicated private security contact is published, report through the official repository owner channel and avoid sharing exploit details publicly.

Пока отдельный private security contact не опубликован, сообщайте через официальный канал владельца репозитория и не публикуйте детали эксплуатации открыто.

---

## Supported versions / Поддерживаемые версии

During the pre-1.0 phase, only the current `main` branch and active release-preparation branches are considered supported for security review.

Во время pre-1.0 фазы только текущая ветка `main` и активные release-preparation branches считаются поддерживаемыми для security review.

---

## Official release verification / Проверка официального релиза

Before using a City Life Core release:

1. Confirm it comes from the official repository.
2. Confirm the release tag matches the release notes.
3. Confirm the commit SHA is listed in release notes or manifest.
4. Verify checksums when release artifacts are provided.
5. Prefer signed tags or signed release commits when available.
6. Build from source if binary provenance cannot be verified.

Перед использованием релиза City Life Core:

1. Проверьте, что он получен из официального репозитория.
2. Проверьте, что release tag совпадает с release notes.
3. Проверьте, что commit SHA указан в release notes или manifest.
4. Проверяйте checksums, если релиз содержит artifacts.
5. Предпочитайте signed tags или signed release commits, когда они доступны.
6. Собирайте из исходников, если происхождение бинарника нельзя подтвердить.

See also:

```text
docs/VERIFYING_RELEASES.md
```

---

## Maintainer security requirements / Требования к maintainer security

Before public 1.0 release, the project should enforce:

- protected `main` branch;
- no force-push to protected branches;
- required CI before merge;
- required review for critical files;
- release tags signed or explicitly documented as unsigned;
- release checksums for SDK archives;
- release manifest with commit SHA and artifact hashes;
- CODEOWNERS for public API, build, release and security files.

---

## Critical files / Критичные файлы

Changes to these areas should require owner review:

- `include/clc/**`
- `src/clc/**`
- `CMakeLists.txt`
- `cmake/**`
- `.github/**`
- `SECURITY.md`
- `LICENSE`
- `CHANGELOG.md`
- `docs/VERSIONING.md`
- `docs/COMPATIBILITY.md`
- `docs/PUBLIC_API_STATUS.md`
- `docs/RELEASE_CHECKLIST.md`
- `docs/PROTECTION_STRATEGY.md`
- `docs/VERIFYING_RELEASES.md`

---

## Non-official builds / Неофициальные сборки

Third-party builds may exist, but they must not claim to be official City Life Core releases unless explicitly approved and linked by the official repository.

Сторонние сборки могут существовать, но они не должны называться официальными релизами City Life Core без явного подтверждения и ссылки из официального репозитория.
