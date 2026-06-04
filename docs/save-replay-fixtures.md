# Save Replay Fixtures

`SaveReplayFixtures.hpp` provides the completed-core fixture catalog for deep and regional simulation systems.

It uses the existing `SimulationPersistenceReplay` save-format review APIs. A fixture is ready when:

- the system id is named;
- the schema id and version are present;
- the golden save payload is loadable in the current format;
- the invalid payload is rejected;
- a replay digest is present.

Useful APIs:

- `make_standard_save_replay_fixtures`
- `validate_save_replay_fixture`
- `make_save_replay_fixture_catalog_report`
- `save_replay_fixture_catalog_digest`
- `save_replay_fixture_catalog_markdown`

The standard catalog covers deep systems and regional systems, including regional market, logistics, climate, territory, migration, audit and long-running maintenance.

Example:

- `examples/save_replay_fixtures.cpp`
