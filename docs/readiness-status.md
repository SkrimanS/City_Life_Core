# Readiness Status

Status: **1.0.0 release snapshot + post-1.0 integration planning**

This page summarizes the readiness posture for the 1.0.0 public release line and the post-1.0.0 integration work tracked on internal `v1.x` branches.

---

## 1.0.0 release snapshot

The following areas are ready for SDK use in the 1.0.0 public release line:

- Core simulation primitives.
- Registry and validation flows.
- Tick/day runtime advancement.
- Route and caravan workflows.
- Economy, contracts and ledger helpers.
- Persistence round-trip validation.
- CMake install/export package.
- Minimal C interface smoke-check surface.

The 1.0.0 release does not imply full Unity package support, Browser/WebAssembly support, full managed API support or a complete C ABI runtime surface.

---

## Post-1.0.0 integration readiness

The following readiness levels describe ongoing `v1.x` development work, not the original `v1.0.0` public release.

| Area | Readiness | Notes |
| --- | --- | --- |
| Native C++ SDK | Ready / primary path | Public C++ API and CMake package are the strongest integration path. |
| Installed CMake package | Ready / primary path | `CityLifeCore::core` is the expected target for external C++ consumers. |
| Minimal C ABI | Minimal supported | Suitable for smoke checks and foreign-function foundation; not yet full runtime coverage. |
| C ABI expansion | Planned | Staged in `c-abi-expansion-plan.md`. |
| C# / Unity wrapper | Initial support | P/Invoke wrapper and Unity smoke test exist; not a full Unity package. |
| Browser / WebAssembly | Planned | Architecture and constraints documented; no adapter or WASM build yet. |
| Game integration profiles | Source-level guidance API | C++ profile catalog, validation, scenario recommendations, scenario summaries, adoption report output, adoption summaries, checklist digest output and profile-specific checklist summaries exist; profiles still do not create runtime modes or engine adapters. |
| Server-authoritative runtime | Partial foundation | Tick runtime, persistence and replay foundations exist; command/replay hardening remains planned. |
| MMO-like support | Early foundation | Requires deeper persistence, replay, partitioning assumptions, diagnostics and scale work. |
| Editor/tooling workflows | Partial foundation | Validation and C++ APIs exist; richer diagnostics and bindable APIs remain planned. |
| SDK ZIP integration examples | Partial / documented | C++ and C ABI consumers exist; C# example sources can be included with installed examples. |

---

## Latest local validation

Latest uploaded Windows full-validation archive for internal `v1.4.0` review:

- Date: 2026-06-02.
- Host path: `C:\Users\Yuta\Desktop\City_Life_Core22\City_Life_Core_v14\City_Life_Core`.
- Generator/toolchain: Visual Studio Build Tools / MSBuild, Release config.
- Script: `scripts\full_validation.bat`.
- Summary: `Exit code: 0`, `Status: passed`.
- Tests: `65/65` passed.
- Example smoke checks: passed, including `clc_example_replay_persistence`.
- Installed consumers: passed for `city_life_core_consumer` and `city_life_core_c_abi_consumer`.

This validation confirms the current `v1.4.0` branch state on the local Windows developer machine. It is not a public release manifest and does not change the major-only public release policy.

---

## Readiness meanings

- **Ready / primary path**: suitable as the recommended integration path for current users.
- **Minimal supported**: usable for limited scenarios, but intentionally narrow.
- **Initial support**: examples or wrappers exist, but the surface is not complete.
- **Partial foundation**: core pieces exist, but product-level usage still needs planned hardening.
- **Planned**: documented direction only; not implemented yet.
- **Planning guidance**: documentation exists to guide architecture and future work.
- **Source-level guidance API**: installed C++ helpers exist for integration planning and review, but they are descriptive and do not create product/runtime behavior.

---

## Integration responsibility

Downstream products remain responsible for:

- gameplay-specific balancing;
- networking and replication;
- UI and rendering;
- authentication and anti-cheat;
- database/storage infrastructure;
- production deployment workflows;
- engine-specific package layout;
- platform-specific native plug-in validation.

City Life Core should remain a headless simulation core and should not become a renderer, UI framework, Unity-only SDK, browser-only SDK or networking framework.

---

## Related documents

- [Core concepts](core-concepts.md)
- [Architecture](architecture.md)
- [Public API status](public-api-status.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Game integration profiles](game-profiles.md)
- [Integration targets](integration-targets.md)
- [Integration validation](integration-validation.md)
- [Release notes](release-notes-1.0.0.md)
