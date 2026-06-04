# Simulation Processors

`clc::sim::SimulationProcessorRegistry` is the shared C++ contract for deep simulation systems.

It tracks:

- processor descriptors;
- feature toggles;
- dependency toggles;
- deterministic processor order;
- skipped processors caused by disabled features/dependencies;
- diagnostics and cause entries;
- deep-core readiness status.

The API is intentionally headless and engine-agnostic. It does not run UI, networking, scripting, or dynamic plugins.

Core entry points:

- `add_simulation_feature`
- `add_simulation_dependency`
- `add_simulation_processor`
- `make_standard_core_processor_registry`
- `enabled_simulation_processors_in_order`
- `make_simulation_processor_run_report`
- `make_deep_simulation_core_readiness_report`

This module is the baseline for the v1.x processor architecture and for later v2/v3/v4 readiness audits.

`make_standard_core_processor_registry` creates the default SDK registry for major systems from the completion definition: resources, production, population, weather, logistics, market, policies, factions, contracts, ecology, events, autonomous actors, content packs, scenario runner, runtime, save/replay, authority, snapshots, regional systems and handoff.
