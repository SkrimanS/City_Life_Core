# Deep Replay Coverage

`DeepReplayCoverage.hpp` tracks save/load, replay, migration, fixture and diagnostics coverage for deep and regional simulation systems.

It is a readiness matrix, not a storage backend.

Useful APIs:

- `make_standard_deep_replay_coverage_baseline`
- `make_deep_replay_coverage_report`
- `deep_replay_coverage_digest`
- `deep_replay_coverage_markdown`

The completed core should have every major deep/regional system marked with:

- save/load coverage;
- replay coverage;
- migration coverage;
- golden fixture;
- invalid fixture;
- diagnostics.

Concrete fixture metadata lives in [`save-replay-fixtures.md`](save-replay-fixtures.md). The fixture catalog validates golden save payloads through the current save-format review path and validates invalid payloads as rejected.
