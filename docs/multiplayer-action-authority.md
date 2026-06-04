# Multiplayer Action Authority

Status: **v2.1.0 internal milestone / source-level C++ API**

`ActionAuthority.hpp` adds actor-scoped review before server-authoritative action dispatch.

It checks:

- actor/session identity match;
- stale or future submitted ticks;
- target settlement existence;
- target ownership through `OwnershipCatalog`;
- resource permission;
- resource availability for remove/transfer actions;
- time-advance permission.

The API is local and transport-agnostic. It does not implement networking, accounts, anti-cheat, bans or UI.

Main flow:

```cpp
auto review = clc::sim::review_runtime_action_authority(runtime, envelope, actor);
auto result = clc::sim::dispatch_authorized_server_runtime_action(runtime, envelope, actor);
```

Rejected authority reviews do not dispatch into the runtime and therefore do not mutate state.

See also: [`server-authoritative-mmo.md`](server-authoritative-mmo.md).
