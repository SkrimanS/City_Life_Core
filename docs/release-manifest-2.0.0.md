# Release Manifest 2.0.0

Status: release-candidate manifest. Fill validation results before tagging `v2.0.0`.

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

Record final local or CI results here before tagging.

- [ ] Configure passed.
- [ ] Build passed.
- [ ] Tests passed.
- [ ] Examples built.
- [ ] Benchmarks built and benchmark output reviewed.
- [ ] Installed C++ consumer built.
- [ ] Installed C ABI consumer built.
- [ ] SDK ZIP generated.
- [ ] SDK ZIP contents reviewed.
- [ ] Unpacked SDK consumer checks passed where applicable.
- [ ] Release notes reviewed.
- [ ] Public surface document reviewed.
- [ ] Migration guidance reviewed.
- [ ] Known limitations reviewed.

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
- Validation date:
- Commit:
- Notes:
