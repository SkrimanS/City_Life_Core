# Multiplayer Load and Diagnostics

Status: **v2.5.0 internal milestone / source-level C++ API**

`MultiplayerLoadDiagnostics.hpp` provides lightweight load snapshots for authoritative workflows.

The snapshot counts:

- settlements;
- caravans;
- routes;
- contracts;
- events;
- actions;
- rejected actions;
- generated snapshots.

Configurable watch/high thresholds classify practical risk for local diagnostics. This is not a production SLA, cluster benchmark or network load test.
