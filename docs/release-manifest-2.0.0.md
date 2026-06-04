# Release Manifest 2.0.0

Status: release-candidate manifest with local Windows/MSVC validation recorded before tagging `v2.0.0`.

## Release Identity

- Version: 2.0.0
- Git tag: `v2.0.0`
- Previous public release: `v1.0.0`
- Release type: public major milestone
- Intended stage: stable multi-game SDK foundation

## Included Scope

- Native C++ SDK source-level API.
- Minimal C ABI version `4`.
- CMake package and CPack SDK ZIP configuration.
- Examples and tests.
- Benchmarks.
- Documentation for API, integration, compatibility, migration, release verification and v2 public surface.

## v1.x Internal Milestones Included

- v1.1.0 integration foundation.
- v1.2.0 Action Bridge.
- v1.3.0 runtime and data hardening.
- v1.4.0 game profiles.
- v1.5.0 platform, integration and deep systems foundation.
- v1.6.0 server-authoritative/MMO foundation.
- v1.7.0 economy, factions and contracts depth.
- v1.8.0 persistence, replay and migration.
- v1.9.0 scale/performance and v2 preparation.

## Validation Checklist

Local Windows/MSVC validation recorded on 2026-06-04:

- [x] Configure passed.
- [x] Build passed.
- [x] Tests passed: 70/70.
- [x] Examples built.
- [x] Benchmarks built and benchmark output reviewed.
- [x] Installed C++ consumer built and ran.
- [x] Installed C ABI consumer built and ran.
- [x] SDK ZIP generated.
- [x] SDK ZIP contents reviewed through CPack/install output.
- [x] Installed SDK consumer checks passed.
- [x] Release notes reviewed.
- [x] Public surface document reviewed.
- [x] Migration guidance reviewed.
- [x] Known limitations reviewed.

## Commands

Recommended local release validation:

```powershell
cmake -S . -B build-v2-release -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON -DCLC_BUILD_BENCHMARKS=ON
cmake --build build-v2-release --config Release
ctest --test-dir build-v2-release --output-on-failure -C Release
cmake --build build-v2-release --target clc_core_benchmarks --config Release
```

Optional package validation:

```powershell
cmake --build build-v2-release --target package --config Release
```

Recorded local commands:

```powershell
cmake -S . -B build-codex-v2 -DCLC_BUILD_TESTS=ON -DCLC_BUILD_EXAMPLES=ON -DCLC_BUILD_BENCHMARKS=ON
cmake --build build-codex-v2 --config Release
ctest --test-dir build-codex-v2 --output-on-failure -C Release
.\build-codex-v2\Release\clc_core_benchmarks.exe
cmake --install build-codex-v2 --config Release --prefix build-codex-v2\install
cmake -S examples\find_package_consumer -B build-codex-v2-consumer -DCMAKE_PREFIX_PATH=C:\Users\Yuta\Desktop\City_Life_Core-1.5.0\build-codex-v2\install
cmake --build build-codex-v2-consumer --config Release
.\build-codex-v2-consumer\Release\city_life_core_consumer.exe
cmake -S examples\c_abi_consumer -B build-codex-v2-c-abi-consumer -DCMAKE_PREFIX_PATH=C:\Users\Yuta\Desktop\City_Life_Core-1.5.0\build-codex-v2\install
cmake --build build-codex-v2-c-abi-consumer --config Release
.\build-codex-v2-c-abi-consumer\Release\city_life_core_c_abi_consumer.exe
cmake --build build-codex-v2 --target package --config Release
git diff --check
```

Recorded benchmark output:

```text
engine_advance_day_100_settlements_365_days,elapsed_ms,18,events,37230
engine_run_scenario_25_settlements_90_days,elapsed_ms,1,reports,90
world_state_serialize_100_settlements_30_days,elapsed_ms,0,bytes,260092
world_state_deserialize_100_settlements_30_days,elapsed_ms,1,settlements,100
runtime_run_days_1000_caravans_30_days,elapsed_ms,3,caravan_ticks,30000
runtime_scale_snapshot_1000_caravans,elapsed_ms,0,serialized_lines,1007
runtime_event_log_checksum_10000_events,elapsed_ms,0,checksum,16490867079834119440
```

Generated package:

```text
build-codex-v2/city-life-core-sdk-2.0.0-Windows.zip
```

## Artifact Review

Review:

- benchmark output;
- CTest logs;
- install tree;
- SDK ZIP contents;
- release notes and manifest consistency;
- C ABI headers and consumer;
- documentation links.

Use [`ci-artifact-review.md`](ci-artifact-review.md) and [`verifying-releases.md`](verifying-releases.md).

## Known Limitations

- No C++ binary ABI promise.
- C ABI is intentionally minimal and does not expose full runtime/economy/persistence surfaces.
- Unity/C# remains example-level, not a packaged Unity SDK.
- Browser/WASM remains planned guidance.
- Networking, auth, replication, databases, sharding, hosting and game-specific rules are not included.

## Sign-Off

- Release reviewer:
- Validation date: 2026-06-04
- Commit:
- Notes: Local validation completed on Windows/MSVC. Branch name remained `v1.5.0` during validation; tag creation was not performed.
