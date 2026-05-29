# Action Bridge

City Life Core v1.2.0 adds a local, transport-agnostic action bridge:

```text
external action -> validation -> runtime mutation -> result/events
```

The bridge is intended for external games, editor tools, server backends, tests and future network adapters. It is not an HTTP API, WebSocket server, account system or multiplayer layer.

## Scope

Implemented in v1.2.0:

- `clc::sim::RuntimeAction`;
- JSON action parsing with `parse_runtime_action_json`;
- action validation with `validate_runtime_action`;
- dispatch with `dispatch_runtime_action` and `dispatch_runtime_action_json`;
- result serialization with `runtime_action_result_to_json`;
- explicit result `validation_status`;
- rejected actions do not mutate runtime state;
- produced command/runtime events and diagnostic counts are returned in the result.

## Action model

The public action model contains:

- `action_id` — stable caller-provided action identifier;
- `type` — action type;
- `actor_id` — optional caller, player, tool or service identity;
- `payload` — action-specific JSON object;
- parsed payload fields such as `target_id`, `secondary_target_id`, `resource_id`, `amount` and `days`.

Top-level legacy fields are still parsed for simple callers, but the canonical v1.2.0 JSON format is payload-first.

## Supported action types

### `add_resource`

Required fields:

```json
{
  "action_id": "a1",
  "type": "add_resource",
  "actor_id": "tool-or-player",
  "payload": {
    "target_id": "riverwatch",
    "resource_id": "grain",
    "amount": 5
  }
}
```

### `remove_resource`

Required fields:

```json
{
  "action_id": "a2",
  "type": "remove_resource",
  "payload": {
    "target_id": "riverwatch",
    "resource_id": "grain",
    "amount": 1
  }
}
```

### `transfer_resource`

Required fields:

```json
{
  "action_id": "a3",
  "type": "transfer_resource",
  "payload": {
    "target_id": "riverwatch",
    "secondary_target_id": "hillford",
    "resource_id": "grain",
    "amount": 2
  }
}
```

`target_id` is the source settlement and `secondary_target_id` is the destination settlement.

### `advance_days`

Required fields:

```json
{
  "action_id": "a4",
  "type": "advance_days",
  "payload": {
    "days": 2
  }
}
```

## Result format

`runtime_action_result_to_json` returns a compact JSON summary:

```json
{
  "action_id": "a1",
  "type": "add_resource",
  "accepted": true,
  "validation_status": "accepted",
  "error_code": "",
  "message": "accepted",
  "events": 1,
  "diagnostics": 0
}
```

`validation_status` is one of:

- `accepted` — validation passed and the action was applied;
- `invalid` — parse or validation failed before mutation;
- `rejected` — validation passed but runtime command execution rejected the action.

Rejected actions use stable error-code categories:

- `malformed_json` — the input is not a JSON object, lacks required parse-level fields or has a non-object `payload`;
- `invalid_action` — the action object is parseable but fails validation;
- `action_rejected` — the action reached runtime command execution but the command rejected it;
- `unsupported_action_type` — reserved fallback for unsupported dispatch paths.

## No-mutation rule

Validation happens before runtime mutation. Invalid action type, missing fields, zero amounts, malformed JSON and malformed payloads are rejected before dispatching to runtime commands.

Runtime command failures also return a rejected result and preserve the command validation diagnostics.

## Example

See:

```text
examples/action_bridge.cpp
```

## Future work

The action bridge intentionally does not include networking. Future versions may add server-authoritative queues, session identity, player permissions and filtered snapshots, but those remain outside v1.2.0 scope.
