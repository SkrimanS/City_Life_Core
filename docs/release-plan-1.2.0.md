# City Life Core v1.2.0 Release Plan

## Version goal

v1.2.0 is the Action Bridge Update.

The goal is to add a local, engine-agnostic path for external tools and game layers:

```text
external action -> validation -> runtime mutation -> result/events
```

This is preparation for future server-authoritative workflows, but it is not networking or multiplayer.

## Scope

Implemented scope:

- action model with `action_id`, `type`, optional `actor_id`, parsed target fields and raw `payload_json`;
- canonical payload-first JSON action input format;
- compact JSON action result summary format;
- explicit `validation_status` values: `accepted`, `invalid`, `rejected`;
- stable action error-code categories;
- runtime command dispatcher;
- validate action before mutation;
- reject invalid or malformed actions without runtime mutation;
- produced event reporting;
- diagnostic count reporting;
- C++ example;
- tests for valid, invalid, malformed, missing-field and malformed-payload actions;
- deterministic result JSON basics;
- documentation.

## Non-goals

v1.2.0 does not add:

- HTTP API;
- WebSocket;
- accounts;
- auth;
- matchmaking;
- multiplayer sessions;
- MMO systems;
- UI.

## Validation checklist

Before merging v1.2.0 to `main`:

- CMake configure passes;
- CMake build passes;
- ctest passes;
- action bridge example builds;
- manual release validation passes;
- invalid actions do not mutate runtime state;
- action result format is documented;
- docs are updated;
- release notes are updated;
- release manifest is updated.
