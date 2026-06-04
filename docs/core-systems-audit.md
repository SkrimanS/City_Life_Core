# Core Systems Audit

`CoreSystemsAudit.hpp` provides the completed-core audit for hidden coupling, deep report coverage, authority sweeps and regional report coverage.

It checks:

- engine-agnostic processors;
- explicit feature/dependency gates;
- deep domain report availability;
- authority sweeps for economy, logistics, contracts, factions, policies and events;
- regional reports for market, logistics, climate, territory, migration, audit, maintenance and large-world load.

Useful APIs:

- `make_standard_core_systems_audit_report`
- `make_core_systems_audit_report`
- `core_systems_audit_digest`
- `core_systems_audit_markdown`

Example:

- `examples/core_systems_audit.cpp`
