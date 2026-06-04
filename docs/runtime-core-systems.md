# Runtime Core Systems

`RuntimeCoreSystems.hpp` bridges the existing `SimulationRuntime` with deep and regional simulation reports.

It evaluates:

- population needs and migration pressure;
- weather and policy impact;
- ecology and crisis propagation;
- autonomous decision response;
- regional market pressure;
- regional climate pressure;
- regional migration pressure;
- long-running maintenance pressure.

Useful APIs:

- `make_default_runtime_core_systems_config`
- `evaluate_runtime_core_systems`
- `runtime_core_systems_digest`
- `runtime_core_systems_markdown`

Example:

- `examples/runtime_core_systems.cpp`

