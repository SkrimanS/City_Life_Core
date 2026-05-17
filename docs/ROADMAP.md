# City Life Core Roadmap

## Strategy

Build the project as a universal, headless C++ SDK first. Multiplayer and MMO features are later architecture layers, not the first deliverable.

## Milestone sequence

### 0.1.0 Foundation Core

- Project skeleton
- Core primitives
- World lifecycle
- Time advancement
- Event log
- Smoke tests
- CI

### 0.2.0 Data Registry

- Data loading
- Schema versioning
- Validation reports
- Resource, currency, building, profession, and settlement definitions

### 0.3.0 Single Settlement Simulation

- Settlement state
- Population groups
- Buildings
- Storage
- Production and consumption
- City reports

### 0.4.0 Economy and Market Core

- Demand/supply model
- Dynamic prices
- Buy/sell orders
- Price reason reports
- Economic invariants

### 0.5.0 Trade Routes and Caravans

- Route graph
- Caravan lifecycle
- Cargo transfer
- Import/export decisions

### 0.6.0 Factions, Guilds, and Contracts

- Factions
- Guilds
- Relations
- Contracts
- Permissions foundation

### 0.7.0 Persistence and Replay

- Snapshots
- Save/load
- Event export
- Deterministic random seed
- Replay skeleton

### 0.8.0 Public SDK API and C ABI

- Stable C++ facade
- C ABI boundary
- JSON action bridge
- Integration examples

### 0.9.0 Beta Hardening

- Benchmarks
- Validation suite
- Docs pass
- API cleanup

### 1.0.0 Stable SDK

- Local/single-player-ready SDK
- Packaging
- Example data set
- Compatibility notes

## Later targets

- 1.x: server-authoritative foundation
- 2.0.0: multiplayer-ready core
- 3.0.0: MMO-ready core
