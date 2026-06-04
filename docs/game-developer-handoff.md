# Game Developer Handoff

City Life Core can be handed to game teams as a headless simulation SDK.

Start with:

- `include/clc/CityLifeCore.hpp` for C++ integrations;
- `include/clc/c/CityLifeCoreC.h` for C ABI integrations;
- `examples/game_developer_handoff.cpp` for profile-specific handoff output;
- `examples/full_runtime_flow.cpp` for a runtime workflow;
- `examples/game_profiles.cpp` for profile discovery;
- `examples/action_bridge.cpp` for external command validation;
- `examples/server_authoritative.cpp` for authoritative command envelopes;
- `examples/multiplayer_to_large_world.cpp` for authority/snapshot/replay/large-world readiness.

The handoff API is:

- `make_game_developer_handoff_report`
- `game_developer_handoff_digest`
- `game_developer_handoff_markdown`

The report combines:

- profile support status;
- integration boundary;
- adoption report;
- checklist;
- platform readiness;
- recommended examples;
- required documents;
- handoff warnings.

For C ABI users, read-only handoff diagnostics are exposed through:

- `clc_supported_game_profile_count_c`
- `clc_game_profile_id_c`
- `clc_sdk_handoff_ready_c`
- `clc_sdk_handoff_digest_c`
- `clc_sdk_handoff_digest_for_profile_c`

Game teams should keep these outside the core:

- renderer;
- UI framework;
- input;
- networking transport;
- replication protocol;
- accounts;
- authentication;
- matchmaking;
- engine-specific gameplay framework.

The core owns deterministic simulation processing, validation, diagnostics, events, persistence/replay helpers, profile guidance and readiness reports.

