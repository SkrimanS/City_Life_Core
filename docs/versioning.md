# Versioning and Release Policy

Status: **active policy after v1.0.0**

City Life Core uses version numbers for planning, documentation and public releases, but not every planned version number becomes a public Git tag or GitHub Release.

This document is the source of truth for the current tag, release and milestone policy.

---

## Public release rule

Public Git tags and GitHub Releases are created only for major public milestones:

- `v1.0.0`
- `v2.0.0`
- `v3.0.0`
- `v4.0.0`

These are the only planned public release tags. The repository should not publish Git tags or GitHub Releases for internal minor milestones such as `v1.1.0`, `v1.2.0`, `v2.1.0` or similar labels.

---

## Internal milestone rule

Minor version labels are internal development milestones.

Examples:

- `v1.1.0` - core polish
- `v1.2.0` - runtime/data hardening
- `v1.3.0` - game profiles
- `v1.4.0` - platform/integration layer
- `v1.5.0` - MMO foundation I
- `v1.6.0` - economy/factions/contracts depth
- `v1.7.0` - persistence/replay/migration
- `v1.8.0` - performance/scale
- `v1.9.0` - v2 preparation

These labels may appear in:

- branch names;
- roadmap documents;
- issue titles or labels;
- pull request titles;
- changelog planning notes;
- local or internal planning discussions.

They must not be published as public Git tags or GitHub Releases unless the release policy is intentionally changed first.

---

## Branch policy

The project should avoid many small long-lived planning branches. Prefer one broad branch per internal stage.

Stage branches are named after the internal milestone itself:

- `v1.1.0`
- `v1.2.0`
- `v1.3.0`
- `v1.4.0`
- `v1.5.0`
- `v1.6.0`
- `v1.7.0`
- `v1.8.0`
- `v1.9.0`

Each stage branch may contain many focused commits and tasks. When the stage is ready, merge it into `main`.

Do not create a public tag or GitHub Release for the internal stage after merging.

---

## Development direction by release line

Before `v2.0.0`, the project focuses on polishing the base, stabilizing the SDK, improving API/runtime/docs/packaging and preparing the first foundations for different game types and MMO/server-authoritative scenarios.

From `v2.0.0` to `v4.0.0`, the project focuses on global mechanics deepening: settlement, resource, economy, faction, contract, runtime, tools, validation, replay, persistence and diagnostics should become deeper, more connected and more useful for real projects.

Starting with `v4.0.0`, the project moves toward maximum mechanics depth and large-project readiness: complex worlds, MMO-like simulations, server-authoritative architectures, production-scale diagnostics and deeper simulation layers.

---

## Bugfix policy

Bug fixes are handled separately from internal milestone planning.

When a bug is found:

1. Fix it in a focused branch or direct maintenance branch as appropriate.
2. Merge the fix into `main` after review/validation.
3. Record the fix in `CHANGELOG.md` under `Unreleased`.
4. Update roadmap, docs or release notes if the fix changes project status, behavior or public guidance.

Bug fixes do not automatically create patch release tags. Public patch releases are not part of the current planned release model.

---

## Changelog policy

`CHANGELOG.md` tracks notable changes that have landed in `main`.

Use `Unreleased` for work that has been merged after the latest public major release and before the next public major release.

For public major releases, create or update release-specific notes and manifests, for example:

- `docs/release-notes-1.0.0.md`
- `docs/release-manifest-1.0.0.md`

Future public release notes and manifests should follow the lower-kebab-case naming convention.

---

## Compatibility policy

City Life Core remains source-first for the C++ API. C++ binary ABI stability is not promised.

The C ABI is intentionally smaller and more conservative. It may be expanded only when the exposed surface is stable enough to support external consumers.

Major public releases may introduce migration-required changes. Such changes must be documented in release notes and migration guidance.

Internal minor milestones should avoid unnecessary breakage, but they are planning and integration stages, not public release contracts.

---

## Related documents

- [Roadmap](roadmap.md)
- [Compatibility](compatibility.md)
- [Migration](migration.md)
- [Release notes 1.0.0](release-notes-1.0.0.md)
- [Release manifest 1.0.0](release-manifest-1.0.0.md)
- [Release manifest template](release-manifest-template.md)
