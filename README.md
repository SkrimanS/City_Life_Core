# City Life Core

City Life Core is a planned universal headless C++ simulation core for living settlements, economy, factions, trade routes, contracts, persistence, server-authoritative multiplayer foundations, and future MMO-ready architecture.

The project is intentionally built from a small, testable core first. The first target is not a full MMO; it is a stable local SDK that can later become multiplayer-ready and MMO-ready.

## Current phase

`0.1.0 Foundation Core`

Initial scope:

- CMake C++20 project skeleton
- core version API
- typed IDs
- Result/Error model
- game time primitive
- event log primitive
- world bootstrap and tick advance
- CLI runner
- smoke tests
- GitHub Actions CI

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

## Development direction

1. Foundation Core
2. Data Registry and schema validation
3. Single settlement simulation
4. Economy and market core
5. Trade routes and caravans
6. Factions, guilds, and contracts
7. Persistence, snapshots, and deterministic replay
8. Public SDK API and C ABI
9. Server-authoritative action layer
10. Multiplayer-ready and MMO-ready architecture
