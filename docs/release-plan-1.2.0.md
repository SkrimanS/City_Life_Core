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

- action model;
- JSON action input format;
- JSON result summary format;
- stable action error-code categories;
- runtime dispatcher;
- rejected action no-mutation behavior;
- produced event reporting;
- C++ example;
- tests for valid, invalid and malformed actions;
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
- docs are updated;
- release notes are updated;
- release manifest is updated.
