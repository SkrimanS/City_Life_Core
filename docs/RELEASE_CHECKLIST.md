# Release Checklist / Чеклист релиза

Status: **1.0.0-rc readiness checklist / чеклист готовности к 1.0.0-rc**

This checklist must be completed before publishing a public SDK release or release candidate.

Этот чеклист должен быть закрыт перед публикацией public SDK release или release candidate.

---

## 1. Version sync / Синхронизация версии

- [ ] `CMakeLists.txt` has the target release version.
- [ ] `include/clc/core/Version.hpp` has the same version.
- [ ] `src/clc/c/CityLifeCoreC.cpp` returns the same C version string.
- [ ] `tests/smoke_tests.cpp` expects the same version.
- [ ] `tests/c_abi_smoke_tests.c` expects the same version.
- [ ] `README.md` shows the same version.
- [ ] `CHANGELOG.md` has a release heading with version and date/rc label.
- [ ] `docs/PUBLIC_API.md` shows the same version/status.
- [ ] `docs/PUBLIC_API_STATUS.md` shows the same version/status.
- [ ] `docs/CORE_CONCEPTS.md` shows the same version/status.
- [ ] `docs/C_ABI.md` shows the same version/status.
- [ ] `docs/CMAKE_PACKAGE.md` shows the same version/status.
- [ ] `docs/BUILD_AND_LINKING_POLICY.md` shows the same version/status.
- [ ] `docs/SDK_STRUCTURE.md` shows the same version/status.
- [ ] `docs/PACKAGING.md` shows the same version/status.
- [ ] `docs/SDK_ZIP_PACKAGE.md` shows the same version/status.
- [ ] Version-specific release notes exist.

## 2. Legal / Юридический минимум

- [ ] `LICENSE` exists at repository root.
- [ ] README names the license.
- [ ] Package docs name the license.
- [ ] Third-party dependency/license notes are documented if dependencies are added.
- [ ] `docs/PROTECTION_STRATEGY.md` is reviewed by the owner.
- [ ] `SECURITY.md` is present and reviewed by the owner.
- [ ] Official release channel is documented.
- [ ] Policy for unofficial forks/builds is documented.

## 3. Project protection / Защита проекта

- [ ] License model is chosen: proprietary/source-available/open-source/dual-license.
- [ ] External contribution policy is chosen.
- [ ] `CONTRIBUTING.md` exists and documents restricted contributions until final license/contribution policy is chosen.
- [ ] DCO/CLA policy is chosen if external contributions are accepted.
- [ ] `main` branch is protected.
- [ ] Force push is disabled for protected branches.
- [ ] CI is required before merge.
- [ ] Release tags are signed or release signing policy is documented.
- [ ] Release checksums are generated.
- [ ] Release manifest is generated from `docs/RELEASE_MANIFEST_TEMPLATE.md`.
- [ ] `docs/VERIFYING_RELEASES.md` is included in release docs.
- [ ] `.github/CODEOWNERS` or equivalent ownership review policy is added for critical files.
- [ ] Official artifacts are linked only from official release notes/channels.
- [ ] Unofficial forks/builds are explicitly forbidden from claiming official status.

## 4. Public API freeze / Freeze публичного API

- [ ] `include/clc/CityLifeCore.hpp` is the recommended C++ umbrella header.
- [ ] `include/clc/c/CityLifeCoreC.h` is documented as the minimal C interface header.
- [ ] `docs/PUBLIC_API_STATUS.md` lists every installed public header.
- [ ] Every installed public header is classified as recommended/stable-candidate/experimental/diagnostics/legacy/specialized/c-abi-minimal.
- [ ] Stable headers are reviewed for naming and semantics.
- [ ] Experimental headers are clearly documented as experimental.
- [ ] Diagnostics APIs have documented compatibility boundaries.
- [ ] C interface scope is documented in `docs/C_ABI.md`.
- [ ] C interface scope is reflected in `README.md`, `docs/SDK_STRUCTURE.md`, `docs/PACKAGING.md`, and `docs/SDK_ZIP_PACKAGE.md`.
- [ ] `SimulationRuntime` mutable data-bag policy is accepted or replaced with a facade.
- [ ] Pointer/reference invalidation rules are documented in `PUBLIC_API.md`.
- [ ] Missing documented headers are fixed or removed from docs.

## 5. Compatibility / Совместимость

- [ ] Tick/time model is frozen.
- [ ] Day/tick dual-field policy is frozen.
- [ ] Runtime event names are frozen.
- [ ] Runtime event payload schemas are documented.
- [ ] Persistence format/version policy is documented.
- [ ] Save/load legacy compatibility behavior is documented.
- [ ] Data-pack schema compatibility policy is documented.
- [ ] Migration guide from 0.9.x to target release exists.
- [ ] C interface version policy is documented.
- [ ] C++ source/binary compatibility position is documented in `docs/BUILD_AND_LINKING_POLICY.md`.

## 6. Code hardening / Укрепление кода

- [ ] `ResourceStorage::transfer` rollback/exception safety is hardened.
- [ ] Runtime cargo load/unload/delivery rollback is hardened.
- [ ] Contract fulfillment + ledger recording atomicity policy is implemented or documented.
- [ ] Runtime/world restore uses temporary state/runtime or otherwise avoids partial mutation on failure.
- [ ] Save writes are atomic or documented as non-atomic with mitigation.
- [ ] Time conversion and advancement overflow is checked or documented.
- [ ] Settlement tick scaling overflow is checked or documented.
- [ ] Ledger sequence overflow/uniqueness/monotonicity validation is implemented.
- [ ] Market demand-only resources are represented in market reports.
- [ ] Market report lookup helpers exist.
- [ ] Recommended trade+ledger wrapper exists or docs warn users explicitly.
- [ ] C interface version/time functions have C smoke coverage.
- [ ] C interface opaque `clc_world` handle has C smoke coverage for create/destroy, null-safety, state access and tick advancement.
- [ ] C interface read-only world event accessors have C smoke coverage for null-safety, out-of-range indexes, event ids, ticks, types and payloads.

## 7. Tests / Тесты

- [ ] Unit tests pass on supported platforms.
- [ ] Runtime tick tests pass.
- [ ] Persistence tests pass.
- [ ] Corrupted save tests pass.
- [ ] Event-log diagnostics tests pass.
- [ ] New public headers have compile/include coverage.
- [ ] C++ umbrella header test passes.
- [ ] C interface smoke test passes.
- [ ] C interface smoke test covers C ABI version, time utilities, opaque `clc_world` behavior and read-only world event accessors.
- [ ] Installed C++ SDK consumer test passes.
- [ ] Installed C ABI consumer test passes.
- [ ] Installed C ABI consumer exercises C ABI version, time utilities, opaque `clc_world` behavior and read-only world event accessors.
- [ ] External `find_package(CityLifeCore CONFIG REQUIRED)` examples build.
- [ ] CI covers tests, examples, benchmarks and package smoke checks.

## 8. Benchmarks / Бенчмарки

- [ ] `CLC_BUILD_BENCHMARKS` benchmark flow exists.
- [ ] `clc_core_benchmarks` builds in CI.
- [ ] `clc_core_benchmarks` runs in CI.
- [ ] CI uploads `benchmark-output.txt` artifact.
- [ ] 10/100 settlement × 365-day baseline is recorded or consciously deferred.
- [ ] 100/1000 caravan baseline is recorded or consciously deferred.
- [ ] Runtime tick small-step baseline is recorded or consciously deferred.
- [ ] Event log 10k/100k/1M baseline is recorded or consciously deferred.
- [ ] Save/load large-state baseline is recorded or consciously deferred.
- [ ] Replay validation large-state baseline is recorded or consciously deferred.
- [ ] Ledger 1k/100k/1M baseline is recorded or consciously deferred.
- [ ] Bulk cargo delivery baseline is recorded or consciously deferred.

## 9. Packaging / Упаковка

- [ ] `cmake --install` works.
- [ ] CMake package config works from install prefix.
- [ ] CMake package variables exist: `CityLifeCore_INCLUDE_DIR`, `CityLifeCore_DOCS_DIR`, `CityLifeCore_DATA_DIR`, `CityLifeCore_EXAMPLES_DIR`.
- [ ] Installed C++ consumer builds and runs.
- [ ] Installed C ABI consumer builds and runs.
- [ ] Installed C ABI consumer validates C ABI version, time utilities, opaque `clc_world` behavior and read-only world event accessors.
- [ ] CPack ZIP package builds.
- [ ] SDK ZIP checksum file `SHA256SUMS.txt` is produced.
- [ ] Unpacked SDK ZIP C++ consumer builds and runs.
- [ ] Unpacked SDK ZIP C ABI consumer builds and runs.
- [ ] Unpacked SDK ZIP C ABI consumer validates C ABI version, time utilities, opaque `clc_world` behavior and read-only world event accessors.
- [ ] CI uploads SDK ZIP artifact.
- [ ] CI uploads checksum artifact.
- [ ] Examples source is included.
- [ ] Demo data is included.
- [ ] Docs are included.
- [ ] Static/shared policy is documented in `docs/BUILD_AND_LINKING_POLICY.md`.
- [ ] Source-only vs binary release policy is documented in `docs/BUILD_AND_LINKING_POLICY.md`.

## 10. Release execution / Выполнение релиза

- [ ] Release branch is created from current `main`.
- [ ] No force push is used unless explicitly approved.
- [ ] CI is green or failures are documented.
- [ ] Benchmark artifacts are reviewed.
- [ ] SDK ZIP artifacts are reviewed.
- [ ] `SHA256SUMS.txt` is reviewed.
- [ ] Release notes are final.
- [ ] Tag naming follows `docs/VERSIONING.md`.
- [ ] Release artifacts are attached or documented.
- [ ] Release manifest/checksums are attached or linked.
- [ ] Post-release compare confirms target branch/tag state.
