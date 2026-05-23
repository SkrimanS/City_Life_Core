# Scripts / Скрипты

This directory contains helper scripts for local validation and release-candidate troubleshooting.

Эта директория содержит helper scripts для локальной проверки и troubleshooting перед release candidate.

---

## Manual release validation

Use these scripts when GitHub Actions logs, metadata or artifacts are unavailable, or when you want to reproduce the release-validation flow locally before reviewing a release candidate.

Используйте эти скрипты, когда GitHub Actions logs, metadata или artifacts недоступны, либо когда нужно локально воспроизвести release-validation flow перед review release candidate.

### Unix-like shell

```bash
bash scripts/manual_release_validation.sh
```

With a custom build directory:

```bash
bash scripts/manual_release_validation.sh build-local-rc-validation
```

### Windows PowerShell

```powershell
pwsh -File scripts/manual_release_validation.ps1
```

With a custom build directory:

```powershell
pwsh -File scripts/manual_release_validation.ps1 -BuildDir build-local-rc-validation
```

---

## What the validation scripts run

The scripts intentionally mirror the release-validation shape used by CI:

- configure/build with tests, examples and benchmarks enabled;
- `ctest`;
- benchmark runner;
- local `cmake --install` prefix;
- installed C++ `find_package` consumer;
- installed C ABI consumer;
- CPack ZIP package;
- checksum generation;
- unpacked ZIP C++ consumer;
- unpacked ZIP C ABI consumer.

The C ABI consumer validates:

- C ABI version;
- version/time utilities;
- opaque `clc_world` create/destroy and state access;
- world tick advancement;
- world event count;
- read-only world event id/tick/type/payload accessors.

---

## Release gate note

A successful local script run is useful for troubleshooting and preflight validation, but it does not replace the final release gate:

- supported CI matrix should pass, or failures should be explicitly accepted and documented;
- benchmark artifacts should be reviewed;
- SDK ZIP artifacts and checksums should be reviewed;
- LICENSE must be chosen by the owner;
- do not merge to `main` until the owner explicitly approves release/merge.

See also:

```text
docs/CI_ARTIFACT_REVIEW.md
docs/RELEASE_CHECKLIST.md
```