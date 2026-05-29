# Scripts

This directory contains helper scripts for local validation, release troubleshooting and integration preflight checks.

Russian documentation starts at [`../docs/ru/README.md`](../docs/ru/README.md).

---

## Manual release validation

Use these scripts when GitHub Actions logs, metadata or artifacts are unavailable, or when you want to reproduce the release-validation flow locally before reviewing a release.

### Unix-like shell

```bash
bash scripts/manual_release_validation.sh
```

With a custom build directory:

```bash
bash scripts/manual_release_validation.sh build-local-validation
```

### Windows PowerShell

```powershell
pwsh -File scripts/manual_release_validation.ps1
```

With a custom build directory:

```powershell
pwsh -File scripts/manual_release_validation.ps1 -BuildDir build-local-validation
```

---

## C# / Unity wrapper validation

Use these scripts to compile-check the C# wrapper outside Unity:

### Unix-like shell

```bash
bash scripts/validate_csharp_wrapper.sh
```

### Windows PowerShell

```powershell
pwsh -File scripts/validate_csharp_wrapper.ps1
```

The scripts run:

```bash
dotnet build examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj -c Release
```

This validation checks the managed wrapper syntax and public managed API surface. It does not load the native library and does not replace Unity Play Mode validation with the platform-specific native plug-in copied into `Assets/Plugins`.

---

## What the manual release validation scripts run

The manual release validation scripts intentionally mirror the release-validation shape used by CI:

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
- world tick/time advancement helpers;
- world event count;
- read-only world event id/tick/type/payload accessors.

---

## Release gate note

A successful local script run is useful for troubleshooting and preflight validation, but it does not replace the final release gate:

- supported CI matrix should pass, or failures should be explicitly accepted and documented;
- benchmark artifacts should be reviewed;
- SDK ZIP artifacts and checksums should be reviewed;
- release manifest should be complete and reviewed;
- LICENSE must be chosen by the owner;
- do not merge release changes until the owner explicitly approves release/merge.

See also:

```text
docs/ci-artifact-review.md
docs/release-manifest-1.0.0.md
docs/verifying-releases.md
```
