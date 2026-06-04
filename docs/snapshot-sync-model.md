# Snapshot and Client Sync Model

Status: **v2.2.0 internal milestone / source-level C++ API**

`SnapshotSync.hpp` defines deterministic runtime snapshot summaries for future client/tool sync layers.

Supported scopes:

- `full`;
- `public_view`;
- `faction`;
- `settlement`.

The output includes version, tick, day, visible settlements, visible caravans, routes, market price count and event count.

The core does not provide a network protocol, WebSocket layer, client prediction, rollback netcode or UI. Hosts can use the summary and digest output as a safe source-level foundation for future transport-specific adapters.
