# Release Manifest 4.0.0

Status: platform foundation manifest.

Included:

- `include/clc/sim/PlatformCore.hpp`
- `src/clc/sim/PlatformCore.cpp`
- `examples/platform_core.cpp`
- `tests/platform_core_tests.cpp`
- `docs/platform-core.md`
- `docs/module-architecture.md`
- `docs/genre-profiles.md`
- `docs/content-pipeline.md`
- `docs/editor-admin-api.md`
- `docs/diagnostics-api.md`
- `docs/migration-v3-to-v4.md`

Validation:

- [x] `city_life_core` target built after platform core additions.
- [x] `clc_test_platform_core_tests` built.
- [x] `clc_example_platform_core` built.
- [x] Full local `ctest` after platform core and C ABI v8 additions: 85/85 passed.
- [x] C# / Unity P/Invoke wrapper compile check passed with 0 warnings and 0 errors.
- [x] Installed SDK generated at `build-codex-v4/install`.
- [x] Installed C++ `find_package` consumer configured, built and ran against the installed SDK: `4.0.0 300`.
- [x] Installed C ABI consumer configured, built and ran against the installed SDK.
- [x] C ABI consumer reported `version=4.0.0`, `c_interface_version=8`, `sdk_handoff=...ready=yes`, `core_completion=...ready=yes`, `deep_replay_coverage=...ready=yes`, `sdk_handoff_manifest=...ready=yes` and `platform_diagnostics=...ready=yes`.
- [x] SDK ZIP regenerated after platform core and C ABI v8 additions: `build-codex-v2/city-life-core-sdk-4.0.0-Windows.zip`.
- [x] SDK ZIP size: 4,474,307 bytes.
- [x] `git diff --check` completed without whitespace errors; Git only reported LF-to-CRLF working-copy warnings.
