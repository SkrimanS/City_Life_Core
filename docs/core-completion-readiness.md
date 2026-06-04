# Core Completion Readiness API

`CoreCompletionReadiness.hpp` provides a compact SDK readiness report for the completed-core goal.

It tracks these areas:

- core architecture;
- v1.x deep simulation systems;
- runtime integration;
- save/load/replay/migration;
- authority and multiplayer safety;
- regional and large-world systems;
- C ABI and foreign-language readiness;
- developer handoff;
- packaging and release readiness.

The core is final-ready only when all areas are ready. Partial areas remain visible in the report and digest.

Useful APIs:

- `make_core_completion_readiness_report`
- `make_core_completion_readiness_input`
- `core_completion_readiness_digest`
- `core_completion_readiness_markdown`

The readiness API supports two paths:

- direct `CoreCompletionReadinessInput` flags for explicit release-gate decisions;
- `CoreCompletionEvidence` for deriving readiness from processor, platform, runtime bridge, SDK handoff manifest, handoff and coverage evidence.

Packaging readiness from evidence requires both package validation availability and a ready `SdkHandoffManifestReport`. This keeps the final gate tied to concrete installed SDK artifacts instead of a loose manual flag.

Example:

- `examples/core_completion_readiness.cpp`
