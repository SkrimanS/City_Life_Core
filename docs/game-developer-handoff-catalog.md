# Game Developer Handoff Catalog

`GameDeveloperHandoffCatalog.hpp` provides a catalog-level handoff gate for all supported game integration profiles.

It checks:

- every non-planned profile has a ready handoff report;
- every ready profile has starter examples;
- every ready profile has required documents;
- the SDK handoff manifest is ready.

Useful APIs:

- `make_standard_game_developer_handoff_catalog_report`
- `make_game_developer_handoff_catalog_report`
- `game_developer_handoff_catalog_digest`
- `game_developer_handoff_catalog_markdown`

Example:

- `examples/game_developer_handoff_catalog.cpp`
