# City Life Core v1.2.0 Release Manifest

## Release line

v1.2.0 — Action Bridge Update.

## Branch

```text
v1.2.0
```

## Scope summary

This release adds a local action bridge:

```text
external action -> validation -> runtime mutation -> result/events
```

## Public API additions

Header:

```text
include/clc/sim/ActionBridge.hpp
```

Types:

- `clc::sim::RuntimeAction`;
- `clc::sim::RuntimeActionParseResult`;
- `clc::sim::RuntimeActionResult`.

Functions:

- `clc::sim::parse_runtime_action_json`;
- `clc::sim::validate_runtime_action`;
- `clc::sim::dispatch_runtime_action`;
- `clc::sim::dispatch_runtime_action_json`;
- `clc::sim::runtime_action_result_to_json`.

## Examples

```text
examples/action_bridge.cpp
```

## Tests

```text
tests/action_bridge_tests.cpp
```

## Documentation

```text
docs/action-bridge.md
docs/release-plan-1.2.0.md
docs/release-notes-1.2.0.md
docs/release-manifest-1.2.0.md
```

## Validation requirements

- configure/build;
- ctest;
- example build;
- manual release validation;
- SDK install;
- SDK ZIP;
- installed and unpacked consumers.

## Compatibility notes

No networking dependency is introduced. No Unity/browser-specific runtime is introduced. The action bridge is local C++ SDK functionality.

## Future work

Future versions may build server-runtime queues, actor permissions, session identity and multiplayer-safe action authority on top of this bridge.
