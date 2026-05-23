# Core Concepts

Version: **1.0.0**

City Life Core is a headless C++20 simulation core. It provides deterministic building blocks that can be embedded into a game, server, backend service, editor or simulation tool.

## Main systems

- **Data registry** stores resource, settlement, route, faction and contract definitions.
- **Validation** reports missing ids, invalid references and unsupported definition shapes before runtime use.
- **Runtime** owns simulation state, world time, settlements, caravans, contracts, wallets, ledgers and events.
- **Ticks** are the preferred real-time unit. Day helpers remain available for turn-based flows.
- **Settlements and storage** model population, resource quantities, building inputs and outputs.
- **Routes and caravans** model travel between settlements and cargo movement.
- **Factions and ownership** let contracts, caravans and rewards be associated with game entities.
- **Economy primitives** provide markets, wallets, trades and ledger entries.
- **Persistence** supports save/load round trips and deterministic replay diagnostics.

## Recommended integration flow

1. Build or load a `clc::data::DataRegistry`.
2. Validate definitions before creating gameplay state.
3. Create a `clc::sim::SimulationRuntime`.
4. Advance runtime by ticks or days.
5. Read reports/events and persist state at game-defined boundaries.

## Related documents

- [Public API](public-api.md)
- [SDK structure](sdk-structure.md)
- [Compatibility](compatibility.md)
- [Migration](migration.md)
