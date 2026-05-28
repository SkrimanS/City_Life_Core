# Project Protection Strategy

Status: **1.0.0 protection strategy + post-1.0 integration guidance**

This document summarizes project protection, provenance and distribution guidance for City Life Core.

The goal is to keep the core trustworthy while allowing downstream games, servers, tools and adapters to build on it.

---

## Canonical source

Official source should be verified against the canonical repository:

```text
SkrimanS/City_Life_Core
```

When consuming the project:

- prefer the canonical repository or reviewed mirrors;
- verify Git tags and release commits;
- review release notes and release manifests;
- rebuild from source when binary provenance is unclear;
- avoid depending on unknown third-party archives without checksums or source references.

---

## Public release protection

Public Git tags and GitHub Releases are planned only for:

- `v1.0.0`
- `v2.0.0`
- `v3.0.0`
- `v4.0.0`

Internal minor milestones such as `v1.1.0`, `v1.2.0` and `v2.1.0` are development branches or planning labels.

Protection rules:

- do not present internal minor branches as official public releases;
- do not publish GitHub Releases for internal minor milestones unless the policy changes;
- record public release validation in release notes and release manifests;
- keep `CHANGELOG.md` updated for user-visible changes;
- document accepted validation gaps before publishing.

---

## Archive and package protection

For SDK ZIP packages and release archives:

- keep checksums with release archives where possible;
- publish or preserve `SHA256SUMS.txt` when generated;
- keep license and provenance information visible in distributed archives;
- verify installed package consumers before redistribution;
- inspect SDK ZIP contents before publishing;
- prefer source builds or verified SDK ZIP packages.

Expected SDK package contents should be aligned with:

```text
docs/sdk-zip-package.md
docs/verifying-releases.md
docs/ci-artifact-review.md
```

---

## API boundary protection

City Life Core has separate integration boundaries. Keeping them separate protects consumers from unstable internals.

Rules:

- Native C++ consumers should use the public C++ API and installed CMake target.
- C and foreign-language integrations should use the C ABI.
- Unity/C# wrappers should call the C ABI through P/Invoke.
- Future Browser/WebAssembly adapters should use exported C-compatible functions where practical.
- Downstream bindings should not bind directly to private C++ implementation details.
- The C++ core should not include Unity, browser or engine-specific code.

This protects both the core and downstream adapters from accidental coupling.

---

## Integration claim protection

Avoid overstating support levels.

Current public-facing status:

- Native C++ SDK: primary supported path.
- C ABI: minimal supported foreign-function boundary.
- C# / Unity: initial wrapper and smoke-test guidance.
- Browser / WebAssembly: planned, not implemented.
- MMO-like support: early foundation, not complete MMO infrastructure.

Do not describe the project as having a full Unity package, full managed SDK, production WebAssembly adapter or complete MMO framework until those surfaces exist and are validated.

---

## License and provenance

City Life Core is released under Apache-2.0.

Distribution guidance:

- include `LICENSE` with redistributed source or packages;
- preserve copyright and attribution notices where required;
- document modifications when redistributing altered builds;
- keep dependency and toolchain provenance clear for production packages.

---

## Downstream responsibility

Downstream products remain responsible for:

- gameplay-specific balancing;
- networking and replication;
- UI and rendering;
- authentication and anti-cheat;
- platform packaging;
- production deployment;
- database/storage infrastructure;
- legal review for their own distribution model.

City Life Core should remain a headless simulation core, not a complete engine or platform SDK.

---

## Related documents

- [Release verification](verifying-releases.md)
- [Release manifest](release-manifest-1.0.0.md)
- [Release manifest template](release-manifest-template.md)
- [Versioning](versioning.md)
- [Compatibility](compatibility.md)
- [Build and linking policy](build-and-linking-policy.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Integration targets](integration-targets.md)
