# City Life Core Architecture

## Core principles

1. The core simulates a living world. A concrete game decides how players interact with it.
2. The simulation is headless and engine-agnostic.
3. External code must eventually mutate world state through validated actions, not direct state writes.
4. IDs are typed from the beginning to avoid cross-domain mistakes.
5. Time, events, persistence, and deterministic replay are first-class concerns.
6. Economy and resource movement must be auditable before multiplayer work begins.

## Initial module layout

```text
include/clc/core/      Public C++ headers
src/clc/core/          Core implementation
apps/clc_runner/       Bootstrap CLI runner
tests/                 Smoke and unit tests
data/                  Demo data packs later
docs/                  Architecture and roadmap notes
.github/workflows/     CI
```

## 0.1.0 Foundation Core modules

- `Version` — public version surface.
- `Result` — explicit success/error return type.
- `Ids` — typed IDs for core domain objects.
- `GameTime` — simulation tick counter.
- `EventLog` — append-only in-memory event stream.
- `World` — minimal world lifecycle and time advancement.

## Near-term architecture target

The next architectural step is to introduce a data registry and validation layer before complex simulation logic. That keeps resources, buildings, professions, settlements, and future economy rules data-driven from the start.
