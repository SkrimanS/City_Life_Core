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

Internal milestone history is tracked in `CHANGELOG.md` and current documentation, not separate per-minor release-note or release-manifest files.

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
v1.x  -> base polish, SDK preparation and integration foundations
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
7. Provide a local Action Bridge for transport-agnostic external action validation and runtime mutation.

---

## Internal v1.x milestones

### v1.1.0 - Integration Foundation Update

Branch: `v1.1.0`

Focus: improve SDK integration foundations after the `v1.0.0` baseline, including initial Unity/C# support through the C ABI.

Planned work:

- Audit public headers, includes and namespace layout.
- Improve examples so they represent recommended integration patterns.
- Add initial C# / Unity integration guidance through the existing C ABI and P/Invoke.
- Add C ABI world advancement helpers required by the initial managed wrapper.
- Add shared-library build support needed by native plug-in style integrations.
- Improve installed SDK and ZIP package validation for examples, scripts and integration files.
- Keep documentation aligned with the English-primary structure and `docs/ru` entry point.

Expected outcome: a cleaner and safer integration baseline for Unity/C#, C ABI, installed SDK and ZIP package consumers.

### v1.2.0 - Action Bridge Update

Branch: `v1.2.0`

Focus: add a local, transport-agnostic action layer:

```text
external action -> validation -> runtime mutation -> result/events
```

Planned work:

- Add a public C++ Action Bridge API.
- Define stable action type, status and error-code constants.
- Add JSON action input parsing for supported local actions.
- Add JSON action result output with command, event and diagnostic details.
- Add runtime dispatch that validates before mutation and rejects invalid actions without mutation.
- Add tests for valid actions, invalid action types, malformed JSON, missing fields, runtime rejection, deterministic results and invalid numeric payloads.
- Add a C++ Action Bridge example.
- Document the Action Bridge as local and transport-agnostic, explicitly excluding HTTP, WebSocket, accounts, auth, matchmaking, multiplayer, MMO and UI.

Expected outcome: a reusable local action bridge that external game layers, tools and future server-authoritative adapters can build around without coupling directly to runtime internals.

### v1.3.0 - Runtime and data hardening

Branch: `v1.3.0`

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

### v1.4.0 - Game profiles

Branch: `v1.4.0`

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

### v1.5.0 - Platform and integration layer

Branch: `v1.5.0`

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

### v1.6.0 - MMO foundation I

Branch: `v1.6.0`

Focus: start the first explicit foundation for server-authoritative and MMO-like runtime use.

Planned work:

- Deepen command-oriented runtime flows suitable for authoritative servers.
- Separate simulation state concepts from player/session concepts.
- Improve deterministic replay and command validation requirements.
- Clarify world partitioning, shard-friendly assumptions and server ownership boundaries.
- Add tests for long-running tick simulation and replay-sensitive behavior.
- Document how clients, servers and tools should interact with the core without making the core a networking framework.

Expected outcome: a practical foundation for larger server-controlled simulations without coupling the SDK to a specific network stack.

### v1.7.0 - Economy, factions and contracts depth

Branch: `v1.7.0`

Focus: deepen the systems that make settlement and resource simulations feel connected.

Planned work:

- Expand market, wallet, trade and ledger scenarios.
- Improve faction reputation, ownership and relationship flows.
- Add richer contract lifecycle rules and validation.
- Improve resource flow between storage, settlements, caravans and contracts.
- Add more cross-system tests for economy, factions and logistics.
- Document extension points for game-specific economy rules.

Expected outcome: deeper connected gameplay primitives while preserving the SDK's headless, embeddable design.

### v1.8.0 - Persistence, replay and migration

Branch: `v1.8.0`

Focus: make saved data and replay workflows more reliable for real projects.

Planned work:

- Define stronger save-format versioning and migration expectations.
- Add migration tests for supported older data shapes.
- Improve deterministic replay diagnostics and mismatch reporting.
- Add checkpoint and event-log guidance for long simulations.
- Improve docs for persistence in tools, servers and shipped games.
- Review serialization boundaries for future large-world scenarios.

Expected outcome: safer long-term data handling and better replay-based debugging.

### v1.9.0 - Performance, scale and v2 preparation

Branch: `v1.9.0`

Focus: prepare the core for larger simulations and for the public `v2.0.0` milestone.

Planned work:

- Expand benchmark coverage and make benchmark artifacts easier to compare.
- Add large-world and high-volume scenario tests.
- Profile memory use and hot runtime paths.
- Reduce unnecessary allocations and redundant lookups where practical.
- Improve diagnostics for scale limits and slow validation paths.
- Document practical scale expectations and integration tradeoffs.
- Freeze the intended `v2.0.0` public surface.
- Review compatibility, migration and release documentation.
- Finalize release notes and release manifest drafts.
- Review CI, artifact packaging, examples and installed SDK consumers.
- Confirm known limitations and post-v2 priorities.
- Run final release validation before tagging `v2.0.0`.

Expected outcome: a more scalable and release-ready `main` branch for the next public major milestone.

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

Main post-`v2.0.0` themes:

- deeper settlement simulation;
- richer resource and production chains;
- more realistic economy, market, pricing and trade behavior;
- stronger faction, ownership, diplomacy and reputation systems;
- deeper contract, task and event lifecycle;
- larger-world runtime support;
- better server-authoritative and MMO-like workflows;
- stronger validation, diagnostics, persistence, replay and migration;
- better tools and integration APIs;
- performance and scale improvements.

---

## v3.0.0 - Deep mechanics and production hardening

`v3.0.0` is a planned public Git tag and GitHub Release.

By `v3.0.0`, the core should have substantially deeper connected systems than `v2.0.0` while staying stable enough to use in serious projects.

Expected direction:

- deeper settlements, professions, buildings and production flows;
- richer resource chains, scarcity, spoilage or quality rules where useful;
- more connected market and pricing behavior;
- deeper faction reputation, ownership, diplomacy and conflict hooks;
- richer contract/task chains and failure behavior;
- larger simulation scenarios;
- improved persistence, replay, diagnostics and migration;
- stronger integration APIs for tools, servers and non-C++ users.

---

## v4.0.0 - Maximum mechanics depth baseline

`v4.0.0` is a planned public Git tag and GitHub Release.

By `v4.0.0`, City Life Core should be useful as a deep simulation foundation for large projects, complex worlds and MMO-like runtime environments.

Expected direction:

- deeply developed settlement, economy, faction, logistics and contract systems;
- production-ready persistence and migration workflows;
- large-world and long-running simulation guidance;
- stronger server-authoritative and MMO-like assumptions;
- improved diagnostics, telemetry-friendly outputs and tooling hooks;
- stable public documentation, release notes and migration guidance.
