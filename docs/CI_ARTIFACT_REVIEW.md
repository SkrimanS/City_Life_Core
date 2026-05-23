# CI and Artifact Review / Проверка CI и artifacts

Status: **manual release gate / ручной release gate**

This document describes how to review CI, benchmark artifacts and SDK ZIP artifacts when preparing a release candidate.

Этот документ описывает, как вручную проверять CI, benchmark artifacts и SDK ZIP artifacts перед release candidate.

---

## Purpose

A release candidate must not be merged or published only because the source branch is mergeable. CI results and generated artifacts must be reviewed first.

Release candidate нельзя merge/publish только потому, что branch mergeable. Сначала нужно проверить CI results и generated artifacts.

---

## Required CI matrix

The `CI` workflow should be reviewed for all matrix jobs:

- `ubuntu-latest / gcc`
- `ubuntu-latest / clang`
- `windows-latest / msvc`

Each job should pass:

- configure;
- build;
- tests;
- benchmark runner;
- install SDK;
- installed C++ consumer;
- installed C ABI consumer;
- CPack ZIP package;
- SHA256SUMS generation;
- unpacked ZIP C++ consumer;
- unpacked ZIP C ABI consumer;
- artifact upload.

---

## Local manual validation fallback

If GitHub Actions logs, metadata or artifacts are unavailable, run the local validation helper from the repository root:

```bash
bash scripts/manual_release_validation.sh
```

Optional custom build directory:

```bash
bash scripts/manual_release_validation.sh build-local-rc-validation
```

The script runs the same release-validation shape as CI:

- configure/build with tests, examples and benchmarks enabled;
- `ctest`;
- benchmark runner;
- `cmake --install` into a local prefix;
- installed C++ consumer;
- installed C ABI consumer;
- CPack ZIP package;
- SHA256SUMS generation;
- unpacked ZIP C++ consumer;
- unpacked ZIP C ABI consumer.

The script prints paths for:

- benchmark output;
- checksum file;
- SDK ZIP;
- unpacked SDK prefix.

A successful local script run does not replace the final release CI requirement, but it is an acceptable troubleshooting fallback when GitHub UI/API data is unavailable.

---

## Required artifacts

For each successful matrix job, review these artifacts:

```text
city-life-core-benchmarks-<os>-<compiler>
city-life-core-sdk-<os>-<compiler>
```

The SDK artifact should contain:

```text
city-life-core-sdk-*.zip
SHA256SUMS.txt
```

---

## Benchmark artifact review

Open `benchmark-output.txt` for each matrix job and check:

- the benchmark executable ran successfully;
- output is not empty;
- output is plausible for the runner platform;
- there are no crashes, assertion failures, sanitizer messages, or truncated output;
- major regressions are either understood or documented.

Benchmarks are not a strict performance contract yet. They are a release sanity check and baseline capture.

---

## SDK ZIP artifact review

For each SDK ZIP artifact:

1. Download the ZIP and `SHA256SUMS.txt`.
2. Recompute SHA-256 for the ZIP.
3. Confirm the hash matches `SHA256SUMS.txt`.
4. Inspect the archive layout.
5. Confirm the archive includes public headers, docs, examples and demo data.
6. Confirm CMake package files exist under the install library directory.
7. Confirm `include/clc/CityLifeCore.hpp` is present.
8. Confirm `include/clc/c/CityLifeCoreC.h` is present.

Expected layout summary:

```text
include/clc/
include/clc/c/CityLifeCoreC.h
lib*/cmake/CityLifeCore/
share/doc/CityLifeCore/
share/CityLifeCore/data/
```

`lib*` may be `lib`, `lib64` or another GNUInstallDirs path depending on platform/toolchain.

---

## Installed SDK consumer review

The CI workflow should build and run:

```text
examples/find_package_consumer/
examples/c_abi_consumer/
```

The C++ consumer should validate normal external usage:

```cmake
find_package(CityLifeCore CONFIG REQUIRED)
target_link_libraries(... CityLifeCore::core)
```

The C ABI consumer should validate:

- C ABI version;
- version/time utilities;
- opaque `clc_world` create/destroy;
- world name/seed/current tick access;
- world tick advancement;
- world event count;
- read-only world event id/tick/type/payload accessors.

---

## Unpacked ZIP consumer review

The CI workflow should unpack the generated ZIP and then build both standalone consumers against the unpacked archive prefix:

```text
examples/find_package_consumer/
examples/c_abi_consumer/
```

This verifies that the distributed ZIP works independently from the intermediate `cmake --install` directory.

---

## Failure diagnostics

If CI fails, review:

- failing job name;
- failing step;
- configure output;
- build output;
- CTest output;
- `LastTest.log` when tests fail;
- CMake output/error logs when configure fails;
- whether failure is platform-specific or common to all matrix jobs.

If workflow logs or artifacts are unavailable through automation, inspect the GitHub Actions run manually in the GitHub UI or use the local manual validation fallback above.

---

## Release decision rule

Before leaving draft status or publishing a release candidate:

- all required CI jobs should be green, or failures should be explicitly accepted and documented;
- benchmark artifacts should be reviewed;
- SDK ZIP artifacts should be reviewed;
- checksums should be verified;
- installed and unpacked ZIP consumers should be confirmed;
- release manifest fields should be fillable from the reviewed run;
- LICENSE must be chosen by the owner.

Do not merge to `main` until the owner explicitly approves release/merge.