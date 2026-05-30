# Action Bridge

City Life Core v1.2.0 adds a local, transport-agnostic action bridge:

```text
external action -> validation -> runtime mutation -> result/events
```

The bridge is intended for external games, editor tools, server backends, tests and future server-authoritative adapters. It is not an HTTP API, WebSocket server, account system, auth layer, matchmaking layer, multiplayer layer, MMO system or UI framework.

## Scope

Implemented in v1.2.0:

- `clc::sim::RuntimeAction`;
- `clc::sim::RuntimeActionParseResult`;
- `clc::sim::RuntimeActionResult`;
- stable action type constants;
- stable validation status constants;
- stable error-code constants;
- JSON action parsing with `parse_runtime_action_json`;
- action validation with `validate_runtime_action`;
- dispatch with `dispatch_runtime_action` and `dispatch_runtime_action_json`;
- result serialization with `runtime_action_result_to_json`;
- explicit result `validation_status`;
- rejected actions do not mutate runtime state;
- produced command/runtime events and validation diagnostics are returned in the result.

## Action model

The public action model contains:

- `action_id` — stable caller-provided action identifier;
- `type` — action type;
- `actor_id` — optional caller, player, tool or service identity;
- `payload` — action-specific JSON object;
- parsed payload fields such as `target_id`, `secondary_target_id`, `resource_id`, `amount` and `days`;
- raw `payload_json` for callers that need to preserve the submitted action payload.

Top-level legacy fields are still parsed for simple callers, but the canonical v1.2.0 JSON format is payload-first.

## Stable constants

Action type constants:

```cpp
clc::sim::runtime_action_type_add_resource;
clc::sim::runtime_action_type_remove_resource;
clc::sim::runtime_action_type_transfer_resource;
clc::sim::runtime_action_type_advance_days;
```

Validation status constants:

```cpp
clc::sim::runtime_action_status_accepted;
clc::sim::runtime_action_status_invalid;
clc::sim::runtime_action_status_rejected;
```

Error-code constants:

```cpp
clc::sim::runtime_action_error_malformed_json;
clc::sim::runtime_action_error_invalid_action;
clc::sim::runtime_action_error_action_rejected;
clc::sim::runtime_action_error_unsupported_action_type;
```

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

## Numeric payload values

`amount` and `days` must be unsigned integer JSON values. Partial numeric values such as `5.5`, negative values such as `-5`, non-numeric values and values that overflow `std::uint64_t` are rejected before runtime mutation.

## Result format

`runtime_action_result_to_json` returns a compact JSON summary plus command/event/diagnostic details:

```json
{
  "action_id": "a1",
  "type": "add_resource",
  "accepted": true,
  "validation_status": "accepted",
  "error_code": "",
  "message": "accepted",
  "events": 1,
  "diagnostics": 0,
  "command_detail": {
    "command": "add_resource_to_settlement",
    "ok": true,
    "subject_id": "riverwatch",
    "target_id": "",
    "resource_id": "grain",
    "amount": 5
  },
  "events_detail": [
    {
      "day": 0,
      "type": "resource.added",
      "message": "added resource"
    }
  ],
  "diagnostics_detail": []
}
```

For `advance_days`, `command_detail` is `null` because the action advances runtime days directly instead of wrapping a `SimulationCommandResult`.

For parse-level failures, the result preserves `action_id` and `type` when those fields were successfully parsed before the failure. For example, an action with a non-object `payload` is rejected as `malformed_json`, but its parsed action identity can still be returned for caller-side correlation.

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

Validation happens before runtime mutation. Invalid action type, missing fields, zero amounts, malformed JSON, malformed payloads, invalid numeric values and overflowing numeric values are rejected before dispatching to runtime commands.

Runtime command failures also return a rejected result and preserve command validation diagnostics.

## Example

See:

```text
examples/action_bridge.cpp
```

## Future work

The action bridge intentionally does not include networking. Future versions may add server-authoritative queues, session identity, player permissions and filtered snapshots, but those remain outside v1.2.0 scope.
