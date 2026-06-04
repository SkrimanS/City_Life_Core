# Multiplayer Persistence and Replay

Status: **v2.3.0 internal milestone / source-level C++ API**

`MultiplayerReplay.hpp` adds command-log review and replay helpers for authoritative multiplayer-style hosts.

The API validates command sequence order and replays `ServerRuntimeActionEnvelope` entries through `ActionAuthority` before dispatching them into a runtime.

It is intended for:

- command-log diagnostics;
- replay-sensitive server workflows;
- save plus command-log compatibility planning.

It does not provide a database, cloud storage, replication service or account service.
