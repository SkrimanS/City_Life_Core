# City Life Core v1.2.0 Release Notes

## Action Bridge Update

v1.2.0 introduces a local action bridge for external game layers, tools and future server-authoritative adapters.

The bridge provides:

- `clc::sim::RuntimeAction`;
- `parse_runtime_action_json`;
- `validate_runtime_action`;
- `dispatch_runtime_action`;
- `dispatch_runtime_action_json`;
- `runtime_action_result_to_json`.

## Supported actions

The initial bridge supports:

- `add_resource`;
- `remove_resource`;
- `transfer_resource`;
- `advance_days`.

## Safety behavior

Invalid and malformed actions are rejected before runtime mutation. Runtime command failures return rejected action results and include command validation state.

## Error-code categories

- `malformed_json`;
- `invalid_action`;
- `action_rejected`;
- `unsupported_action_type`.

## Examples

Added:

```text
examples/action_bridge.cpp
```

## Tests

Added tests for:

- valid action;
- invalid action type;
- malformed JSON;
- missing fields;
- rejected action no mutation;
- deterministic result JSON basics.

## Compatibility

This release keeps City Life Core headless and engine-agnostic. It does not add networking, UI, accounts or multiplayer sessions.
