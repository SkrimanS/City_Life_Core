# CI and Artifact Review

Status: **release and integration artifact review guide**

Use this guide when reviewing CI results, benchmark output, SDK ZIP archives and release artifacts.

This document complements `verifying-releases.md` and `integration-validation.md`.

---

## Public release policy reminder

Public Git tags and GitHub Releases are planned only for:

- `v1.0.0`
- `v2.0.0`
- `v3.0.0`
- `v4.0.0`

Internal minor milestones such as `v1.1.0` and `v1.2.0` are development branches. CI artifacts from those branches may be useful for review, but they are not official public releases by themselves.

---

## CI review checklist

Confirm:

- configure/build jobs complete for supported toolchains;
- tests pass or documented failures are explicitly accepted;
- Action Bridge tests and examples are built when integration changes touch local action dispatch;
- benchmark artifacts do not show obvious regressions;
- installed package consumers can build;
- installed package consumers validate Action Bridge header/docs/example availability;
- C ABI consumers can build;
- SDK ZIP artifacts are present when expected;
- documentation-only changes still keep links and package notes consistent;
- integration-related changes include docs/changelog updates.

---

## SDK ZIP artifact review

Download SDK ZIP artifacts and inspect the install layout before publishing or redistributing them.

Expected contents generally include:

- public headers;
- native library artifacts for the selected build configuration;
- CMake package config files;
- documentation;
- examples;
- scripts;
- example `.clcd` data packs.

Expected integration examples and docs may include:

```text
examples/action_bridge.cpp
examples/find_package_consumer/
examples/c_abi_consumer/
examples/csharp_unity/
docs/action-bridge.md
```

Expected validation scripts may include:

```text
scripts/validate_csharp_wrapper.sh
scripts/validate_csharp_wrapper.ps1
```

The C# / Unity files are source examples and wrappers. They are not proof that a platform-specific Unity native plug-in has been built correctly.

---

## Checksum review

Verify checksums before publishing or redistributing archives.

Example:

```bash
cmake -E sha256sum city-life-core-sdk-*.zip
```

Compare the result against the published `SHA256SUMS.txt` when one is provided.

---

## Installed package consumer review

After unpacking or installing an SDK artifact, verify the installed C++ package consumer:

```bash
cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build-consumer --config Release
```

The installed package consumer should validate normal C++ usage plus installed Action Bridge header/docs/example and validation script availability.

Verify the C ABI consumer:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build-c-abi-consumer --config Release
```

The exact command may vary by generator and platform.

---

## C# / Unity artifact review

If SDK artifacts include `examples/csharp_unity/`, check that these files are present:

```text
examples/csharp_unity/CityLifeCoreNative.cs
examples/csharp_unity/CityLifeWorldSafeAccess.cs
examples/csharp_unity/CityLifeNativeDiagnostics.cs
examples/csharp_unity/CityLifeSmokeTest.cs
examples/csharp_unity/CityLifeCoreNative.CompileCheck.csproj
examples/csharp_unity/README.md
```

Review notes:

- these files should match the current C ABI header;
- they require a native shared library built for the Unity target platform;
- they are not yet an official Unity package;
- they are not a substitute for Unity Editor/player plug-in validation.

---

## Browser / WebAssembly artifact review

Browser/WebAssembly support is planned, not implemented.

Do not mark an artifact as browser-ready unless it includes:

- a documented WebAssembly build path;
- exported C-compatible functions suitable for the adapter;
- JavaScript or TypeScript adapter files;
- browser smoke-test instructions;
- validation notes.

Until then, Browser/WASM review is limited to confirming that native-core changes do not block the planned adapter path.

---

## Benchmark review

Review benchmark artifacts for obvious regressions:

- unexpected order-of-magnitude slowdowns;
- missing benchmark output;
- large variance without explanation;
- changes to benchmark names or output format that break artifact review.

Benchmarks are not a complete performance certification. Treat them as trend and regression signals.

---

## Documentation review

For integration-related changes, check that relevant documents stay aligned:

```text
docs/architecture.md
docs/public-api.md
docs/public-api-status.md
docs/action-bridge.md
docs/c-abi.md
docs/c-abi-expansion-plan.md
docs/csharp-unity.md
docs/browser-wasm.md
docs/integration-targets.md
docs/integration-validation.md
docs/game-profiles.md
docs/packaging.md
docs/sdk-zip-package.md
docs/verifying-releases.md
CHANGELOG.md
```

---

## Related documents

- [Release verification](verifying-releases.md)
- [Release manifest](release-manifest-1.0.0.md)
- [Action Bridge](action-bridge.md)
- [SDK ZIP package](sdk-zip-package.md)
- [Integration validation](integration-validation.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
