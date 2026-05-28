# Contributing / Участие в разработке

City Life Core is a released 1.0.0 SDK line with ongoing post-1.0.0 internal development on `v1.x` branches.

External code contributions are currently restricted and require maintainer approval before review or merge. Issues, audit reports, documentation feedback and integration feedback are welcome when they follow the project scope and contribution rules.

City Life Core уже имеет выпущенную линию SDK 1.0.0, а дальнейшая внутренняя разработка ведётся в ветках `v1.x`.

Внешние code contributions сейчас ограничены и требуют одобрения maintainer перед review или merge. Issues, audit reports, feedback по документации и integration feedback принимаются, если они соответствуют scope проекта и правилам contribution.

---

## Current contribution status

Current status: **restricted contributions**.

This means:

- unsolicited large code drops may be closed without review;
- maintainers may accept issues, audit reports, documentation feedback and integration feedback;
- code changes from external contributors require explicit maintainer approval before review;
- no contribution is accepted as official until it is merged by an authorized maintainer;
- no fork, mirror, package or rebuild may claim to be the official City Life Core distribution unless it is published through the official project release channel;
- no third-party package may claim to be an official Unity package, WebAssembly adapter or managed SDK unless the canonical repository explicitly publishes it as such.

This policy protects the project while the public SDK boundary, C ABI expansion, release process and future contribution model continue to mature.

---

## Legal and authorship requirements

Before submitting code, contributors must be able to confirm that:

- they wrote the contribution themselves or have the right to submit it;
- the contribution does not include copied code without permission;
- the contribution does not include malware, backdoors, credential theft, telemetry hidden from maintainers, or intentionally obfuscated behavior;
- the contribution does not intentionally weaken validation, persistence, release verification, package checks, or project protection mechanisms;
- the contribution does not impersonate the project owner or another maintainer.

A final DCO/CLA policy is not selected yet. If external code contributions are accepted later, the maintainer may require a DCO, CLA, signed-off commits or another explicit contributor certification process before merge.

---

## Branch and release policy

Current branch policy:

- `main` — latest public release line; currently expected to match `v1.0.0` until the next approved public milestone merge.
- `v1.1.0`, `v1.2.0`, `v1.3.0`, etc. — broad internal development milestone branches.
- bugfix branches may be used when maintainers decide they are needed, then merged through the approved maintainer flow.

Do not create many small version-prefixed branches for one stage. A broad internal stage branch may contain many commits and tasks.

Public Git tags and GitHub Releases are planned only for:

- `v1.0.0`
- `v2.0.0`
- `v3.0.0`
- `v4.0.0`

Minor versions such as `v1.1.0`, `v1.2.0` or `v2.1.0` are internal development milestones unless the policy changes.

Protected branches should not be force-pushed. Release tags and artifacts should be created only through the official maintainer release process.

---

## Integration boundary policy

When contributing integration-related changes, use the correct boundary:

- Native C++ integrations should use the public C++ API and `CityLifeCore::core`.
- C and foreign-language integrations should use the C ABI.
- C# / Unity wrappers should use P/Invoke over the C ABI.
- Future Browser/WebAssembly adapters should use exported C-compatible functions where practical.
- Engine-specific code should stay outside the native C++ core.
- Do not bind foreign-language integrations directly to private C++ implementation details.

Relevant docs:

```text
docs/architecture.md
docs/public-api.md
docs/public-api-status.md
docs/c-abi.md
docs/c-abi-expansion-plan.md
docs/csharp-unity.md
docs/browser-wasm.md
docs/integration-targets.md
docs/integration-validation.md
```

---

## Pull request expectations

Every PR should include:

- a clear summary;
- the target branch and internal milestone;
- tests or a reason tests are not applicable;
- notes about public API changes;
- notes about C ABI changes if relevant;
- notes about C# / Unity or Browser/WASM adapter impact if relevant;
- notes about simulation invariants affected by the change;
- notes about persistence/save/load compatibility if relevant;
- notes about replay or event payload compatibility if relevant;
- notes about package/install/CI impact if relevant;
- documentation and changelog updates for public-facing changes.

Every non-documentation PR should pass the normal build and test workflow before merge.

Integration-related PRs should also use `docs/integration-validation.md` and the pull request template.

---

## Security-sensitive changes

Security-sensitive changes require extra review. This includes changes to:

- CI workflows;
- release artifact generation;
- checksums/manifests;
- CMake install/export/package config;
- SDK ZIP packaging;
- C ABI boundary or native handle ownership;
- C# / Unity native plug-in loading guidance;
- Browser/WebAssembly adapter boundaries when added;
- persistence and deserialization;
- runtime restore;
- event validation;
- ownership/permission checks;
- ledger/reward flows;
- CODEOWNERS or branch protection related files.

Do not submit vulnerability reports as public issues if they include exploitable details. Follow `SECURITY.md`.

---

## C++ style baseline

- C++20 minimum.
- Prefer explicit types at API boundaries.
- Prefer typed IDs over raw integers in public APIs.
- Prefer workflow/helper APIs over direct runtime state mutation in integration code.
- Avoid floating point for money/resource ledgers.
- Validate overflow-sensitive arithmetic.
- Keep save/load and event payload changes explicit and documented.
- Keep public headers stable once marked stable-candidate.
- Keep C ABI handles opaque and ownership rules explicit.

---

## Documentation style

Developer documentation should explain what the SDK is and how to use it. Avoid roadmap-only language in user-facing docs unless the document is explicitly a release checklist, migration guide, roadmap or maintainer policy document.

Preferred docs focus:

- what the core provides;
- what is released, initial, planned or internal;
- how to include and link it;
- how to create data and runtime state;
- how to use workflows;
- how to validate errors;
- how to save/load and verify packages;
- what is stable, experimental or specialized;
- which integration boundary to use.

When touching documentation, remove obsolete version-specific or pre-1.0 material instead of adding another outdated layer.
