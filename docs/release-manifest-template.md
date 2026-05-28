# Release Manifest Template

Status: **template for future public releases**

Use this page as the starting point for future City Life Core release manifests.

Public Git tags and GitHub Releases are planned only for:

- `v1.0.0`
- `v2.0.0`
- `v3.0.0`
- `v4.0.0`

Internal minor milestones are development branches and should not use this template as a public release manifest unless the release policy changes.

---

## Release identity

- Release version:
- Git tag:
- Release commit:
- Release date:
- Release owner / reviewer:
- Previous public release:
- Release type:
  - [ ] Public major milestone
  - [ ] Internal validation snapshot
  - [ ] Other

Confirm:

- [ ] Tag exists and points to the intended commit.
- [ ] Release notes match the tagged source.
- [ ] `CHANGELOG.md` includes relevant changes.
- [ ] Release follows the major-only public tag/release policy.

---

## Supported platforms and compilers

Document validated platforms:

| Platform | Compiler / toolchain | Build type | Status | Notes |
| --- | --- | --- | --- | --- |
| Windows |  |  |  |  |
| Linux |  |  |  |  |
| macOS |  |  |  |  |

---

## Build, test and benchmark validation

Commands or CI links:

```text
paste commands, workflow URLs or artifact references here
```

Checklist:

- [ ] Configure passed.
- [ ] Build passed.
- [ ] Tests passed or accepted failures are documented.
- [ ] Benchmarks were reviewed or explicitly skipped.
- [ ] Manual release validation was run or CI equivalent was accepted.

---

## Installed package validation

C++ installed package consumer:

```bash
cmake -S examples/find_package_consumer -B build-consumer -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build-consumer --config Release
```

C ABI installed package consumer:

```bash
cmake -S examples/c_abi_consumer -B build-c-abi-consumer -DCMAKE_PREFIX_PATH=/path/to/city-life-core-sdk
cmake --build build-c-abi-consumer --config Release
```

Checklist:

- [ ] `CityLifeCore::core` is found by external consumers.
- [ ] C++ installed package consumer builds.
- [ ] C ABI installed package consumer builds.
- [ ] Installed package variables are usable where expected.
- [ ] Installed examples are present.

---

## SDK ZIP validation

Archive name(s):

```text
paste archive names here
```

Expected contents:

- [ ] Public headers.
- [ ] Native library artifacts for the selected build configuration.
- [ ] CMake package config files.
- [ ] Documentation.
- [ ] Example `.clcd` data packs.
- [ ] `examples/find_package_consumer/`.
- [ ] `examples/c_abi_consumer/`.
- [ ] `examples/csharp_unity/`, if C# / Unity examples are included for this release.

Notes:

```text
paste archive inspection notes here
```

---

## Checksums and artifact review

- [ ] `SHA256SUMS.txt` was generated or checksums are otherwise documented.
- [ ] Published archive checksums match local verification.
- [ ] CI artifacts were reviewed before publishing.
- [ ] Benchmark artifacts were reviewed before publishing.

Checksum command:

```bash
cmake -E sha256sum city-life-core-sdk-*.zip
```

---

## API and integration surface review

Document changes to:

- [ ] Public C++ API.
- [ ] C ABI.
- [ ] C interface version.
- [ ] C# / Unity wrapper examples.
- [ ] Browser / WebAssembly status.
- [ ] CMake package or install layout.
- [ ] SDK ZIP layout.
- [ ] Save/load or replay behavior.
- [ ] Data registry or validation behavior.

Notes:

```text
paste API and integration notes here
```

---

## C# / Unity review

If C# / Unity examples are included:

- [ ] `examples/csharp_unity/CityLifeCoreNative.cs` matches the C ABI header.
- [ ] `examples/csharp_unity/CityLifeSmokeTest.cs` is present and smoke-test oriented.
- [ ] `examples/csharp_unity/README.md` explains native plug-in requirements.
- [ ] Release notes do not imply a full Unity package unless one is actually included.
- [ ] Platform-specific native plug-in artifacts are documented if provided.

---

## Browser / WebAssembly review

Current status:

- [ ] Not implemented / planned only.
- [ ] Experimental adapter included.
- [ ] Validated adapter included.

If Browser/WASM support is claimed, document:

- WebAssembly build path;
- exported functions;
- JS/TS adapter files;
- browser smoke-test instructions;
- memory/string ownership rules;
- validation status.

---

## Compatibility and migration notes

- [ ] Compatibility policy reviewed.
- [ ] Migration guide updated when needed.
- [ ] Save/load compatibility reviewed.
- [ ] Replay compatibility reviewed.
- [ ] C ABI compatibility reviewed.
- [ ] Known breaking changes documented.

Notes:

```text
paste compatibility notes here
```

---

## Known issues and accepted risks

| Issue / risk | Severity | Accepted by | Notes |
| --- | --- | --- | --- |
|  |  |  |  |

---

## Related documents

- [Release verification](verifying-releases.md)
- [CI artifact review](ci-artifact-review.md)
- [SDK ZIP package](sdk-zip-package.md)
- [Integration validation](integration-validation.md)
- [Compatibility](compatibility.md)
- [Migration](migration.md)
- [Versioning](versioning.md)
