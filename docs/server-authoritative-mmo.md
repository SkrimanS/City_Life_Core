# Server-Authoritative and MMO Foundation

Status: **planning / v1.6.0 foundation**

City Life Core v1.6.0 adds a small source-level C++ foundation for server-authoritative and MMO-like runtime use.

This foundation is deliberately not a network stack, session service, account system, matchmaking layer, replication system, database layer or MMO framework. It gives host servers a clearer way to wrap local runtime actions with ordered command metadata, host-owned session identity, shard descriptors and audit records while keeping platform and product responsibilities outside the native core.

---

## Scope

The v1.6.0 foundation covers:

- host-owned player/session metadata that stays separate from simulation state;
- server action envelopes with sequence numbers, submitted ticks, shard ids and `RuntimeAction` payloads;
- local dispatch into `SimulationRuntime` through the existing Action Bridge command model;
- audit records for accepted, rejected and invalid commands;
- sequence summaries and validation for replay-sensitive authoritative command logs;
- shard descriptors for documenting server ownership and partition assumptions;
- digest output for logs, smoke tests and tooling.

Header:

```cpp
#include "clc/sim/ServerAuthoritative.hpp"
```

The recommended umbrella header also includes this surface:

```cpp
#include "clc/CityLifeCore.hpp"
```

---

## Main types

```cpp
clc::sim::ServerSessionIdentity;
clc::sim::ServerShardDescriptor;
clc::sim::ServerRuntimeActionEnvelope;
clc::sim::ServerRuntimeActionAuditRecord;
clc::sim::ServerRuntimeActionDispatchResult;
clc::sim::ServerActionSequenceSummary;
```

`ServerSessionIdentity` is host-owned metadata. It should represent the product server's current view of a player/session/connection. The core records it for diagnostics and audit output, but it does not authenticate it and does not turn sessions into simulation entities.

`RuntimeAction::actor_id` remains the simulation-facing actor id. A host may map a session/player to an actor before dispatch. If `actor_id` differs from `player_id`, envelope validation emits a warning rather than rejecting the command, because some products may intentionally use service actors, tool actors or delegated control.

---

## Basic authoritative flow

```cpp
auto bootstrap = clc::sim::make_basic_runtime_scenario();
auto& runtime = bootstrap.runtime;

clc::sim::ServerRuntimeActionEnvelope envelope{
    .sequence = 1,
    .submitted_tick = runtime.time.current_tick(),
    .shard_id = "riverlands-1",
    .session = clc::sim::ServerSessionIdentity{
        .player_id = "player-a",
        .session_id = "session-a",
        .connection_id = "connection-a",
    },
    .action = clc::sim::RuntimeAction{
        .action_id = "server-add-grain",
        .type = std::string{clc::sim::runtime_action_type_add_resource},
        .actor_id = "player-a",
        .target_id = "riverwatch",
        .resource_id = "grain",
        .amount = 10,
    },
};

const auto result = clc::sim::dispatch_server_runtime_action(runtime, envelope);
const auto audit_digest = clc::sim::server_runtime_action_audit_digest(result.audit);
```

Invalid envelopes are rejected before runtime mutation. Runtime-rejected actions still produce audit records with validation status, error code, event count and diagnostic count.

---

## Sequence validation

Authoritative servers often need command logs that are easy to replay and audit. v1.6.0 provides helpers for contiguous, strictly increasing sequence review:

```cpp
const auto summary = clc::sim::summarize_server_action_sequence(audit_log);
const auto validation = clc::sim::validate_server_action_sequence(audit_log);
```

These helpers do not store logs or implement replay by themselves. They make host-owned command logs easier to check before save/load, replay, soak tests or incident review.

---

## Shard and partition assumptions

`ServerShardDescriptor` documents the host's ownership boundary:

```cpp
clc::sim::ServerShardDescriptor shard{
    .shard_id = "riverlands-1",
    .owner_service_id = "simulation-service-a",
    .region_id = "local",
    .partition_id = "riverlands",
};
```

This descriptor is intentionally descriptive. It does not route commands, open sockets, load databases or move entities between shards. Downstream servers remain responsible for persistence storage, routing, replication and deployment.

---

## Example

See:

```text
examples/server_authoritative.cpp
tests/server_authoritative_tests.cpp
```

The example creates a shard descriptor, dispatches two ordered server action envelopes, records audit entries, validates the sequence and prints digest output.

---

## Non-goals

This foundation does not provide:

- HTTP or WebSocket transport;
- sessions, login, accounts or authentication;
- authorization or permissions;
- anti-cheat;
- matchmaking;
- replication, snapshots or client prediction;
- database or save-service infrastructure;
- complete MMO gameplay systems;
- C ABI exposure for the server-authoritative surface.

Those layers belong to the host product or future deliberately scoped adapters.

---

## Related documents

- [Action Bridge](action-bridge.md)
- [Game integration profiles](game-profiles.md)
- [Platform integration layer](platform-integration-layer.md)
- [Integration targets](integration-targets.md)
- [Integration validation](integration-validation.md)
- [Roadmap](roadmap.md)
