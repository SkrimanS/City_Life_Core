# Runtime Processor Orchestration

`RuntimeProcessorOrchestration.hpp` connects the deterministic processor registry to a concrete `SimulationRuntime` tick step.

It provides:

- standard processor orchestration using `make_standard_core_processor_registry`;
- explicit orchestration with a caller-provided registry;
- runtime tick advancement through the existing tick API;
- runtime core systems evaluation after the tick;
- bounded diagnostic event streams for deep and regional systems;
- digest and markdown output for tools, servers and release validation.

Useful APIs:

- `orchestrate_standard_runtime_core_processors`
- `orchestrate_runtime_core_processors`
- `runtime_processor_orchestration_digest`
- `runtime_processor_orchestration_markdown`

The bounded stream is intentionally small and string-based. It is not a network protocol; game teams can mirror it into their own telemetry, editor windows, server logs or replay tooling.

Example:

- `examples/runtime_processor_orchestration.cpp`
