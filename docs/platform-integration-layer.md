# Platform Integration Layer

Status: **planning / v1.5.0 integration foundation**

This document defines the platform and adapter boundary review for the `v1.5.0` internal milestone.

City Life Core should stay a headless, engine-agnostic simulation SDK. Platform integrations should be thin adapters around the public C++ API, the local Action Bridge, or the C ABI. They should not move engine, browser, network, account, UI or deployment concerns into the native core.

---

## v1.5.0 scope

The `v1.5.0` milestone focuses on making the SDK easier to embed across platforms, engines and backend environments while starting the first deeper connected gameplay-system foundations.

Planned work covered by this document:

- review engine-agnostic adapter boundaries;
- improve examples for consuming the installed SDK from external projects;
- evaluate safe C ABI expansion candidates after the minimal 1.0.0 interface;
- expand C# / Unity guidance where the C ABI is stable enough;
- add clearer integration notes for servers, editors, tools, Unity projects and game clients;
- improve CMake package, ZIP SDK and artifact-review expectations where needed;
- document threading, ownership and lifecycle assumptions;
- define integration diagnostics that are useful outside local tests;
- keep the new market, ledger, faction, contract, production and logistics diagnostic surfaces source-level and engine-agnostic.

---

## Integration boundary model

Recommended layering:

```text
Product / host platform
  -> host-owned adapter layer
      -> public C++ API, Action Bridge or C ABI
          -> City Life Core native simulation core
```

The host-owned adapter layer may be a Unity script wrapper, a C# assembly, a WebAssembly JavaScript or TypeScript facade, a game-server service layer, an editor plug-in, a command-line tool, or a custom engine bridge.

The adapter layer owns platform details. The core owns deterministic simulation behavior, validation, persistence, replay, diagnostics and stable integration boundaries.

---

## Boundary rules

### Keep platform details outside the core

The native core should not include Unity, Unreal, Godot, browser, operating-system UI, web server, database, authentication, matchmaking, replication or rendering APIs.

Adapters may translate platform concepts into core concepts, but the core should not depend on those platforms.

### Prefer stable public boundaries

Use the richest boundary that is safe for the host:

| Host type | Preferred boundary | Notes |
| --- | --- | --- |
| Native C++ game/tool/server | Public C++ API | Richest API surface today. |
| C consumer | C ABI | Minimal but stable foreign-function baseline. |
| Unity / C# | C ABI + P/Invoke wrapper | Initial support; high-level managed SDK remains future work. |
| Browser / WebAssembly | Future WASM adapter over C ABI exports | Planned; no browser API should enter the core. |
| Server-authoritative product | C++ API + local Action Bridge | Transport, sessions, auth and permissions remain product-owned. |
| Editor or balancing tool | C++ API, Action Bridge, C ABI where needed | Needs richer bindable diagnostics over time. |

### Keep Action Bridge transport-agnostic

The Action Bridge is a local validation and dispatch layer. It should remain independent of HTTP, WebSocket, matchmaking, session ownership and client prediction.

Products can place networking or permission checks before calling the Action Bridge, but the bridge should not become a networking framework.

### Keep deep-system snapshots source-level for now

The v1.5.0 deep-system foundation adds source-level C++ helpers for market pressure, ledger summaries, faction access reports, contract lifecycle summaries, production pressure, logistics pressure and combined deep-system diagnostics.

These helpers are designed for native games, server-side tools, editor tooling and future adapters. They should not be exposed through the C ABI until the structures, ownership model and text-output patterns are reviewed in a later C ABI expansion phase.

### Grow the C ABI deliberately

The C ABI is the safest long-term boundary for Unity/C#, WebAssembly, scripting languages and non-C++ tools. It should grow in small phases with explicit ownership, null behavior, error/status conventions and tests.

Do not expose C++ classes, STL containers, templates, exceptions or unstable object layouts through the C ABI.

---

## Adapter responsibilities

Host adapters are responsible for:

- locating and loading the native library;
- translating host strings, buffers, ids and asset paths into core inputs;
- mapping core diagnostics to host logs, UI or editor panels;
- scheduling simulation ticks according to product rules;
- applying authorization, account, session and permission checks before core mutation;
- handling network transport, replication, client prediction and persistence storage infrastructure;
- packaging native plug-ins, managed assemblies, WASM modules or command-line tools for the target platform.

City Life Core is responsible for:

- deterministic simulation primitives;
- public C++ source APIs;
- the intentionally stable C ABI surface;
- local Action Bridge validation and dispatch;
- registry, runtime, validation, persistence, replay and diagnostics behavior;
- source-level deep-system reports and digests for native integrations;
- installed SDK and package metadata needed by consumers.

---

## Lifecycle and ownership assumptions

General lifecycle expectations for adapter authors:

1. Initialize host platform services outside the core.
2. Load or link the City Life Core native library.
3. Check version and C interface compatibility when using the C ABI.
4. Create core-owned handles or C++ runtime objects through documented APIs.
5. Pass only valid inputs and preserve ownership rules for borrowed strings, buffers and handles.
6. Step the simulation from one host-owned thread or synchronization point unless a future API explicitly documents stronger threading guarantees.
7. Read diagnostics, events and state snapshots through documented APIs.
8. Destroy or release core-owned handles through matching destroy/free functions.

The current SDK should be treated as externally synchronized. Adapter authors should not assume that mutable runtime objects can be advanced or mutated concurrently from multiple threads.

---

## Integration diagnostics direction

Useful integration diagnostics should answer questions that host adapters need during setup and smoke tests:

- Which public boundary is being used?
- Which version of the native library is loaded?
- Which C interface version is available and required?
- Which profile or target is being validated?
- Which required systems are unavailable through the chosen boundary?
- Which adapter responsibilities remain product-owned?
- Which warnings should be shown in editor or CI logs?
- Which market, production, logistics or contract pressure warnings should be shown to designers or server operators?

For `v1.5.0`, integration diagnostics can begin as documentation, checklist output, examples and C++ helper output. Future milestones may expose more of this through the C ABI for Unity, browser and tooling use.

---

## Deep-system diagnostics surface

The v1.5.0 source-level deep-system diagnostics surface is intentionally read-only and report-oriented:

- `clc/economy/Market.hpp` exposes market pressure snapshots and per-resource shortage/surplus/depleted signals.
- `clc/economy/Ledger.hpp` exposes ledger summaries and per-resource buy/sell/contract-reward totals.
- `clc/sim/Factions.hpp` exposes faction access reports and permission flags.
- `clc/sim/Contracts.hpp` exposes contract lifecycle summaries and reputation/overdue review helpers.
- `clc/sim/Production.hpp` exposes settlement production pressure snapshots.
- `clc/sim/Logistics.hpp` exposes route, caravan and cargo pressure snapshots.
- `clc/sim/DeepSystemsDiagnostics.hpp` combines the above into warning counts, critical counts, digest output and markdown output.

Adapters can use these reports to populate editor panels, server smoke-test logs, balancing dashboards or local debug overlays without giving the core any dependency on UI, networking or engine-specific APIs.

The source-tree example `examples/deep_systems_foundation.cpp` demonstrates the intended v1.5.0 flow: build individual market, ledger, contract, production and logistics snapshots, combine them into a `DeepSystemsDiagnostics` value, then emit digest and markdown output for a host-owned tool or log surface.

---

## Safe C ABI expansion candidates

Candidates that fit the current direction:

- validation result handles with error/warning counts;
- indexed diagnostic messages and diagnostic codes;
- registry create/destroy and data-pack loading from caller-provided strings or buffers;
- read-only registry queries for resources, buildings, professions and settlements;
- runtime create/destroy through validated registry input;
- runtime time, tick and event queries;
- explicit caller-owned buffer or owned-string patterns for result text;
- future text/digest-only access to reviewed deep-system diagnostics, once ownership and lifetime rules are settled.

Candidates to defer until the native systems are more stable:

- full economy mutation APIs;
- faction diplomacy or reputation mutation APIs;
- contract lifecycle mutation APIs;
- persistence and replay buffer APIs;
- callbacks, async APIs or host-owned thread callbacks;
- direct C ABI exposure of C++ STL-backed deep-system snapshot structures;
- networking, sessions, authentication, matchmaking or replication concepts.

---

## Non-goals

The platform integration layer is not:

- a Unity package implementation;
- a browser runtime implementation;
- a web server or multiplayer transport;
- an account, auth, anti-cheat or matchmaking system;
- a renderer or UI framework;
- a database abstraction;
- a promise of C++ binary ABI stability across compilers;
- a reason to expose internal C++ implementation details to foreign-language bindings.

---

## Related documents

- [Architecture](architecture.md)
- [Integration targets](integration-targets.md)
- [Integration validation](integration-validation.md)
- [C ABI expansion plan](c-abi-expansion-plan.md)
- [C# and Unity integration](csharp-unity.md)
- [Browser and WebAssembly integration](browser-wasm.md)
- [Game integration profiles](game-profiles.md)
- [Build and linking policy](build-and-linking-policy.md)
- [CMake package](cmake-package.md)
- [SDK ZIP package](sdk-zip-package.md)
