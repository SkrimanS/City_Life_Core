# Release Checklist / Чеклист релиза

Status: **draft for 1.0.0-rc1 / черновик для 1.0.0-rc1**

This checklist must be completed before publishing a public SDK release or release candidate.

Этот чеклист должен быть закрыт перед публикацией public SDK release или release candidate.

---

## 1. Version sync / Синхронизация версии

- [ ] `CMakeLists.txt` has the target release version.
- [ ] `include/clc/core/Version.hpp` has the same version.
- [ ] `tests/smoke_tests.cpp` expects the same version.
- [ ] `README.md` shows the same version.
- [ ] `CHANGELOG.md` has a release heading with version and date/rc label.
- [ ] `docs/PUBLIC_API.md` shows the same version/status.
- [ ] `docs/SDK_STRUCTURE.md` shows the same version/status.
- [ ] `docs/PACKAGING.md` shows the same version/status.
- [ ] Version-specific release notes exist.

## 2. Legal / Юридический минимум

- [ ] `LICENSE` exists at repository root.
- [ ] README names the license.
- [ ] Package docs name the license.
- [ ] Third-party dependency/license notes are documented if dependencies are added.

## 3. Public API freeze / Freeze публичного API

- [ ] `docs/PUBLIC_API_STATUS.md` lists every installed public header.
- [ ] Every header is classified as stable/experimental/diagnostics/legacy/internal-risk.
- [ ] Stable headers are reviewed for naming and semantics.
- [ ] Experimental headers are clearly documented as experimental.
- [ ] Diagnostics APIs have documented compatibility boundaries.
- [ ] `SimulationRuntime` mutable data-bag policy is accepted or replaced with a facade.
- [ ] Pointer/reference invalidation rules are documented in `PUBLIC_API.md`.
- [ ] Missing documented headers are fixed or removed from docs.

## 4. Compatibility / Совместимость

- [ ] Tick/time model is frozen.
- [ ] Day/tick dual-field policy is frozen.
- [ ] Runtime event names are frozen.
- [ ] Runtime event payload schemas are documented.
- [ ] Persistence format/version policy is documented.
- [ ] Save/load legacy compatibility behavior is documented.
- [ ] Data-pack schema compatibility policy is documented.
- [ ] Migration guide from 0.9.x to target release exists.

## 5. Code hardening / Укрепление кода

- [ ] `ResourceStorage::transfer` rollback/exception safety is hardened.
- [ ] Runtime cargo load/unload/delivery rollback is hardened.
- [ ] Contract fulfillment + ledger recording atomicity policy is implemented or documented.
- [ ] Runtime/world restore uses temporary state/runtime or otherwise avoids partial mutation on failure.
- [ ] Save writes are atomic or documented as non-atomic with mitigation.
- [ ] Time conversion and advancement overflow is checked or documented.
- [ ] Settlement tick scaling overflow is checked or documented.
- [ ] Ledger sequence overflow/uniqueness/monotonicity validation is implemented.
- [ ] Market demand-only resources are represented in market reports.
- [ ] Recommended trade+ledger wrapper exists or docs warn users explicitly.

## 6. Tests / Тесты

- [ ] Unit tests pass on supported platforms.
- [ ] Runtime tick tests pass.
- [ ] Persistence tests pass.
- [ ] Corrupted save tests pass.
- [ ] Event-log diagnostics tests pass.
- [ ] New public headers have compile/include coverage.
- [ ] Installed SDK consumer test passes.
- [ ] External `find_package(CityLifeCore CONFIG REQUIRED)` example builds.
- [ ] CI covers tests and examples.

## 7. Benchmarks / Бенчмарки

- [ ] `CLC_BUILD_BENCHMARKS` or equivalent benchmark flow exists.
- [ ] 10/100 settlement × 365-day baseline is recorded.
- [ ] 100/1000 caravan baseline is recorded.
- [ ] Runtime tick small-step baseline is recorded.
- [ ] Event log 10k/100k/1M baseline is recorded.
- [ ] Save/load large-state baseline is recorded.
- [ ] Replay validation large-state baseline is recorded.
- [ ] Ledger 1k/100k/1M baseline is recorded.
- [ ] Bulk cargo delivery baseline is recorded.

## 8. Packaging / Упаковка

- [ ] `cmake --install` works.
- [ ] CMake package config works from install prefix.
- [ ] SDK zip or packaging script exists for release candidates.
- [ ] Package checksum/manifest is produced.
- [ ] Examples source is included.
- [ ] Demo data is included.
- [ ] Docs are included.
- [ ] Static/shared policy is documented.
- [ ] Source-only vs binary release policy is documented.

## 9. Release execution / Выполнение релиза

- [ ] Release branch is created from current `main`.
- [ ] No force push is used unless explicitly approved.
- [ ] CI is green or failures are documented.
- [ ] Release notes are final.
- [ ] Tag naming follows `docs/VERSIONING.md`.
- [ ] Release artifacts are attached or documented.
- [ ] Post-release compare confirms target branch/tag state.
