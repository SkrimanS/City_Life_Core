# City Life Core

City Life Core is a universal headless C++ simulation core for living settlements, economy, factions, trade routes, contracts, persistence, server-authoritative multiplayer foundations, and future MMO-ready architecture.

The project is intentionally built from a small, testable core first. The current target is a stable local SDK that can later become multiplayer-ready and MMO-ready.

## Current phase

`0.7.x Simulation Core Expansion`

The old `0.1.0 Foundation Core` phase is complete. The repository now contains the foundation, data registry, data pack loading, single-settlement simulation, economy primitives, engine command APIs, snapshots, multi-day scenarios, scenario presets, catalog lookup helpers, and compact diagnostics for scenario/catalog surfaces.

Current implemented areas:

- CMake C++20 project skeleton and core version API
- Result/Error and validation reporting primitives
- game time, event log, world bootstrap, and CLI runner
- data definitions, registry validation, and `.clcd` data pack loading
- settlement state, storage, building ticks, and settlement reports
- economy market reports, trade transactions, ledger, and market orders
- simulation engine with commands, event log, snapshots, day advancement, and multi-day scenarios
- scenario summaries, scenario results, scenario presets, preset catalogs, and catalog runner helpers
- smoke, economy, simulation, resource command, engine lookup, command result, and scenario digest tests

## Next development direction

Stop deepening report/digest-only layers unless a concrete system needs them. The next layers should build real core systems in this order:

1. Save/load and deterministic snapshot persistence
2. Routes between settlements
3. Caravans and route execution
4. Factions and ownership/reputation primitives
5. Contracts, obligations, and settlement/faction trade goals
6. Public SDK-facing API cleanup
7. Server-authoritative action layer
8. Multiplayer-ready and MMO-ready architecture

## Build

```bash
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the bootstrap CLI:

```bash
./build/clc_runner
```

On Windows with multi-config generators:

```powershell
cmake -S . -B build -DCLC_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```
