# Verifying Official Releases

Status: **release verification guide**

This guide helps users distinguish official City Life Core releases from forks, modified archives, unexpected packages or unreviewed builds.

---

## Official public release policy

Public Git tags and GitHub Releases are planned only for the major public milestones:

- `v1.0.0`
- `v2.0.0`
- `v3.0.0`
- `v4.0.0`

Internal minor milestones such as `v1.1.0`, `v1.2.0` or `v2.1.0` are development branches or planning labels. They should not be treated as official public release tags unless the release policy changes.

If you see a public package claiming to be an official minor release, verify it against the repository release policy before trusting it.

---

## Verify source and tags

When verifying a release:

- use the official repository release page and tags;
- confirm the expected tag exists;
- confirm the tag points to the intended release commit;
- compare the release notes and release manifest with the tag contents;
- rebuild from source when a downstream package cannot be trusted;
- review `LICENSE` and release notes before redistribution.

For `v1.0.0`, `main` and the `v1.0.0` tag are expected to match the released source state.

---

## Verify archives and checksums

When SDK ZIP archives are published:

- download archives only from the official release or trusted CI artifact source;
- compare archive checksums against published `SHA256SUMS.txt` when provided;
- prefer reproducible local rebuilds when possible;
- inspect archive contents before using them in production.

Example checksum command:

```bash
cmake -E sha256sum city-life-core-sdk-*.zip
```

Compare the output with the published checksum file.

---

## Expected SDK ZIP contents

An SDK ZIP package should generally contain the installed SDK layout:

- public headers;
- native library artifacts for the selected build configuration;
- CMake package config files;
- documentation;
- examples;
- scripts;
- example `.clcd` data packs.

Integration examples and docs may include:

```text
examples/action_bridge.cpp
examples/find_package_consumer/
examples/c_abi_consumer/
examples/csharp_unity/
docs/action-bridge.md
```

Validation scripts may include:

```text
scripts/validate_csharp_wrapper.sh
scripts/validate_csharp_wrapper.ps1
```

The C# / Unity files are wrappers and smoke-test examples. They do not replace the native shared library required by Unity or another host runtime.

---

## Local validation

Run the manual validation script when CI logs or artifacts are unavailable:

```bash
bash scripts/manual_release_validation.sh
```

Windows PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File scripts/manual_release_validation.ps1
```

For integration-related release checks, also review:

```text
docs/integration-validation.md
```

---

## Installed package validation

After installing or unpacking an SDK package, verify CMake package consumption:

```bash
cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build-consumer --config Release
```

This consumer should validate normal C++ package usage, installed Action Bridge header/docs/example availability and C# wrapper validation script availability.

Verify C ABI consumption:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build-c-abi-consumer --config Release
```

The exact build command may differ by generator and platform.

---

## Unity / C# verification note

If a release package includes `examples/csharp_unity/`, treat it as source examples for the current C ABI wrapper.

A Unity verification should also check that the native shared library for the target platform exists and can be loaded by Unity.

Typical files:

```text
examples/csharp_unity/CityLifeCoreNative.cs
examples/csharp_unity/CityLifeWorldSafeAccess.cs
examples/csharp_unity/CityLifeNativeDiagnostics.cs
examples/csharp_unity/CityLifeSmokeTest.cs
examples/csharp_unity/README.md
```

Unity package layout and prebuilt plug-ins are not guaranteed until explicitly added to a future release policy.

---

## Browser / WebAssembly verification note

Browser/WebAssembly support is planned, not implemented.

Do not treat a release as browser-ready unless it explicitly includes a documented WebAssembly build path, adapter and validation notes.

---

## Related documents

- [Release notes](release-notes-1.0.0.md)
- [Release manifest](release-manifest-1.0.0.md)
- [Versioning](versioning.md)
- [CI artifact review](ci-artifact-review.md)
- [SDK ZIP package](sdk-zip-package.md)
- [Integration validation](integration-validation.md)
- [Action Bridge](action-bridge.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
