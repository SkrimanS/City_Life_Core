# Roadmap

Status: **active planning after v1.0.0**

This roadmap describes the intended direction for City Life Core after the `v1.0.0` baseline. It is a planning document, not a promise that every internal milestone will become a public Git tag or GitHub Release.

City Life Core is developed as an engine-agnostic, headless simulation SDK for games, tools, servers, editors and MMO-like backends. The long-term direction is to make the core useful across different genres, platforms and runtime models while keeping it easy to embed.

---

## Release model used by this roadmap

Public Git tags and GitHub Releases are reserved for major public milestones only:

- `v1.0.0`
- `v2.0.0`
- `v3.0.0`
- `v4.0.0`

Minor version names such as `v1.1.0`, `v1.2.0`, `v2.1.0` and similar labels are **internal development milestones**. They may be used as branch names, planning notes, issues and pull requests, but they are not public release tags.

Bug fixes are handled separately from the milestone branches. When a bug fix is ready, it should be merged into `main` and recorded in `CHANGELOG.md` under `Unreleased`.

See [`versioning.md`](versioning.md) for the full release and versioning policy.

---

## Branch strategy

The project avoids a large number of small long-lived planning branches. Each internal stage should normally use one broad branch named after the internal milestone itself:

- `v1.1.0`
- `v1.2.0`
- `v1.3.0`
- `v1.4.0`
- `v1.5.0`
- `v1.6.0`
- `v1.7.0`
- `v1.8.0`
- `v1.9.0`

Each stage branch may contain many focused commits and tasks. After a stage is ready, its branch is merged into `main`. No public tag is created for the internal stage.

---

## Roadmap shape

```text
v1.x  -> base polish and SDK preparation
v2.x  -> start of global mechanics deepening
v3.x  -> expansion, stabilization and production hardening of deeper systems
v4.0  -> transition to a deeply developed simulation core
v4.x+ -> large-project, MMO and complex-world evolution
```

---

## Direction before v2.0.0

The main goal before `v2.0.0` is to polish the existing 1.0.0 foundation and make the SDK easier to embed into different games, platforms and server-authoritative runtimes.

Primary themes:

1. Optimize and polish the existing codebase.
2. Stabilize the SDK, public API, runtime, documentation and packaging baseline.
3. Improve support for different game types and integration styles.
4. Start the foundation for MMO and server-authoritative games.
5. Harden runtime, validation, persistence, replay, packaging, documentation, examples and the C ABI.
6. Add first-class integration guidance for C# and Unity through the C ABI without making the core Unity-specific.

---

## Internal v1.x milestones

### v1.1.0 - Core polish

Branch: `v1.1.0`

Focus: improve the quality, consistency and maintainability of the current 1.0.0 codebase without changing the overall product direction.

Planned work:

- Audit public headers, includes and namespace layout.
- Remove avoidable duplication and simplify common helper paths.
- Tighten validation messages and error reporting consistency.
- Expand regression coverage for known edge cases.
- Improve examples so they represent recommended integration patterns.
- Add initial C# / Unity integration guidance through the existing C ABI and P/Invoke.
- Review runtime APIs for clarity before larger data/runtime work starts.
- Keep source compatibility in mind for current 1.x users.
- Keep documentation aligned with the English-primary structure and `docs/ru` entry point.

Expected outcome: a cleaner and safer core baseline for later runtime, data, Unity/C# and MMO-oriented work.

### v1.2.0 - Runtime and data hardening

Branch: `v1.2.0`

Focus: make runtime data, validation and deterministic behavior more robust.

Planned work:

- Harden registry validation and package-data checks.
- Improve runtime error handling for invalid definitions and invalid state transitions.
- Add stronger tests for deterministic tick progression.
- Review save/load failure behavior and malformed input handling.
- Strengthen diagnostics for world state, events, routes, caravans and contracts.
- Clarify data ownership and mutation boundaries.
- Improve documentation for data authors and backend integrators.

Expected outcome: safer data ingestion and more predictable runtime behavior.

### v1.3.0 - Game profiles

Branch: `v1.3.0`

Focus: make the core easier to adopt for different game styles.

Planned work:

- Define supported integration profiles, such as:
  - turn-based city/settlement simulation;
  - real-time economy simulation;
  - survival or colony-style logistics;
  - Unity/C# client or tool integration through native plug-ins;
  - backend service simulation;
  - MMO/server-authoritative simulation.
- Document which systems are required, optional or recommended for each profile.
- Add example scenarios or presets that show how the same core can support different genres.
- Clarify platform-neutral assumptions and non-goals.
- Identify missing APIs needed by non-desktop, Unity/C# or non-C++ integration layers.

Expected outcome: clearer adoption paths for external developers building different types of games and tools.

### v1.4.0 - Platform and integration layer

Branch: `v1.4.0`

Focus: improve SDK embedding across platforms, engines and backend environments.

Planned work:

- Review engine-agnostic adapter boundaries.
- Improve examples for consuming the installed SDK from external projects.
- Evaluate safe C ABI expansion candidates after the 1.0.0 minimal interface.
- Expand C# / Unity support once the required C ABI surface is stable enough.
- Add clearer integration notes for servers, editors, tools, Unity projects and game clients.
- Improve CMake package, ZIP SDK and artifact-review workflows where needed.
- Document threading, ownership and lifecycle assumptions.
- Add integration diagnostics that are useful outside local tests.

Expected outcome: a cleaner integration surface for games, tools, Unity projects and backend services.

### v1.5.0 - MMO foundation I

Branch: `v1.5.0`

Focus: start the first explicit foundation for server-authoritative and MMO-like runtime use.

Planned work:

- Define command-oriented runtime flows suitable for authoritative servers.
- Separate simulation state concepts from player/session concepts.
- Improve deterministic replay and command validation requirements.
- Clarify world partitioning, shard-friendly assumptions and server ownership boundaries.
- Add tests for long-running tick simulation and replay-sensitive behavior.
- Document how clients, servers and tools should interact with the core without making the core a networking framework.

Expected outcome: a practical foundation for larger server-controlled simulations without coupling the SDK to a specific network stack.

### v1.6.0 - Economy, factions and contracts depth

Branch: `v1.6.0`

Focus: deepen the systems that make settlement and resource simulations feel connected.

Planned work:

- Expand market, wallet, trade and ledger scenarios.
- Improve faction reputation, ownership and relationship flows.
- Add richer contract lifecycle rules and validation.
- Improve resource flow between storage, settlements, caravans and contracts.
- Add more cross-system tests for economy, factions and logistics.
- Document extension points for game-specific economy rules.

Expected outcome: deeper connected gameplay primitives while preserving the SDK's headless, embeddable design.

### v1.7.0 - Persistence, replay and migration

Branch: `v1.7.0`

Focus: make saved data and replay workflows more reliable for real projects.

Planned work:

- Define stronger save-format versioning and migration expectations.
- Add migration tests for supported older data shapes.
- Improve deterministic replay diagnostics and mismatch reporting.
- Add checkpoint and event-log guidance for long simulations.
- Improve docs for persistence in tools, servers and shipped games.
- Review serialization boundaries for future large-world scenarios.

Expected outcome: safer long-term data handling and better replay-based debugging.

### v1.8.0 - Performance and scale

Branch: `v1.8.0`

Focus: prepare the core for larger simulations and more demanding server/tool scenarios.

Planned work:

- Expand benchmark coverage and make benchmark artifacts easier to compare.
- Add large-world and high-volume scenario tests.
- Profile memory use and hot runtime paths.
- Reduce unnecessary allocations and redundant lookups where practical.
- Improve diagnostics for scale limits and slow validation paths.
- Document practical scale expectations and integration tradeoffs.

Expected outcome: a more scalable foundation for bigger worlds and longer-running simulations.

### v1.9.0 - v2 preparation

Branch: `v1.9.0`

Focus: prepare `main` for the public `v2.0.0` milestone.

Planned work:

- Freeze the intended `v2.0.0` public surface.
- Review compatibility, migration and release documentation.
- Finalize release notes and release manifest drafts.
- Review CI, artifact packaging, examples and installed SDK consumers.
- Confirm known limitations and post-v2 priorities.
- Run final release validation before tagging `v2.0.0`.

Expected outcome: a release-ready `main` branch for the next public major milestone.

---

## v2.0.0 - Stable multi-game SDK foundation

`v2.0.0` is the next public Git tag and GitHub Release after `v1.0.0`.

`v2.0.0` marks the point where City Life Core should become a stable SDK foundation for different types of games, tools and server-authoritative runtimes.

The release should represent the completed pre-v2 work from the internal v1.x milestones. It should include updated release notes, a release manifest, verification guidance, migration notes and reviewed SDK artifacts.

After `v2.0.0`, development is no longer limited to base polishing. Each following internal minor milestone should gradually deepen gameplay mechanics and make the core more globally developed.

No `v1.1.0`, `v1.2.0` or other minor v1.x public release tags are planned.

---

## Main development line after v2.0.0

Starting with `v2.0.0`, City Life Core moves from the **stable SDK foundation** stage into the stage of **global gameplay-mechanics deepening**.

If `v1.x` is about polishing the base, API, runtime, documentation, packaging and the first steps toward multiple game types and MMO/server-authoritative foundations, then `v2.x` and `v3.x` should gradually turn the core into a deeper, more flexible and more useful system for real projects.

From `v2.0.0` to `v4.0.0`, the main task is to:

- deepen existing mechanics;
- expand settlement, resource, economy, faction, contract and runtime systems;
- add more links between systems;
- prepare the core for large worlds, server scenarios and MMO-like use;
- improve tools, validation, replay, persistence and diagnostics;
- make the core convenient for different genres and platforms;
- avoid merely adding surface-level features and instead make systems deeper, more reliable and more useful.

`v2.x` and `v3.x` are the foundation for a mature simulation core.

---

## v2.x internal milestones

The v2.x line starts the global deepening of mechanics introduced and stabilized before `v2.0.0`.

Likely work areas:

- richer settlement growth and lifecycle rules;
- more detailed resource production, consumption and transformation chains;
- stronger economy balancing primitives;
- faction relationships, influence, reputation and ownership depth;
- contract generation, contract negotiation and failure consequences;
- runtime events that connect economy, factions, settlements and logistics;
- better tools-facing validation and data-authoring feedback.

Minor v2.x labels remain internal planning milestones unless the release policy changes.

---

## v3.0.0 - Deepening production systems

`v3.0.0` should develop what was prepared in `v2.x`: runtime, economy, factions, settlements, persistence, replay, MMO foundation, tools and diagnostics.

The focus of `v3.x` is not just new features. It is system connectedness, scale, integration quality and preparation for `v4.0.0`.

---

## v3.x internal milestones

The v3.x line should expand, stabilize and harden deeper production systems for large worlds and server-centric scenarios.

Likely work areas:

- large data-set validation and faster content iteration;
- persistent world operations and long-running simulation diagnostics;
- stronger replay, rollback and audit workflows;
- large-scale logistics and cross-region simulation;
- better observability for servers and tools;
- stronger migration guarantees for shipped games;
- improved C ABI, C# or other foreign-language integration where the API is stable enough.

Minor v3.x labels remain internal planning milestones unless the release policy changes.

---

## v4.0.0 and beyond - Deep mechanics and large project readiness

`v4.0.0` opens the stage of maximum mechanics deepening. After this version, City Life Core should evolve as a serious core for large projects, complex game worlds, server-authoritative architectures and MMO-like simulations.

Before `v4.0.0`, the core gradually strengthens its base, expands systems and prepares the foundation. Starting with `v4.0.0`, it should become not only an SDK foundation but a deeply developed simulation core.

The main goal after `v4.0.0` is to:

- deepen gameplay mechanics as much as possible;
- develop complex economy, production, logistics and consumption chains;
- expand settlement, faction, contract, ownership and event behavior;
- improve scalability for large worlds;
- strengthen MMO/server-authoritative support;
- add more complex simulation layers;
- improve analysis, balancing, debugging and replay tools;
- make the core useful not only for small games, but also for large production projects.

`v4.0.0` is not the finish line. It is the transition into deep mechanics development.

The core should remain headless, portable and embeddable. Rendering, UI and networking should remain outside the core unless a future policy explicitly changes that boundary.
