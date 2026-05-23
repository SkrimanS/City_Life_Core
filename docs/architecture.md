# City Life Core Architecture

City Life Core is a headless simulation SDK. Its architecture keeps domain simulation, data definitions, validation, persistence and integration packaging separate.

## Layers

- **Public headers** expose the supported C++ and minimal C integration surface.
- **Data layer** stores definitions and validates references before runtime use.
- **Simulation layer** owns settlements, storage, routes, caravans, factions, contracts, events and runtime time.
- **Economy layer** provides market, wallet, trade and ledger primitives.
- **Persistence layer** saves, loads and validates runtime state.
- **Packaging layer** exports the SDK as a CMake package and optional SDK ZIP archive.

## Principles

- Keep the SDK headless and renderer/UI/network agnostic.
- Prefer deterministic runtime operations and explicit validation.
- Let downstream games and servers own product-specific rules.
- Keep installed documentation English-primary, with Russian documentation under `docs/ru`.

## Related documents

- [Core concepts](core-concepts.md)
- [SDK structure](sdk-structure.md)
- [Public API](public-api.md)
