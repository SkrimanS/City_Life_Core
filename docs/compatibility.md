# Compatibility Policy

Version: **1.0.0**

City Life Core 1.x prioritizes source compatibility for supported public headers and documented workflows.

## Expectations

- Public headers under `include/clc` should remain usable across compatible 1.x updates.
- Consumers should rebuild against the SDK version they ship with.
- Save/load compatibility should be validated by downstream products before upgrading production worlds.
- Tick semantics, deterministic reports and validation behavior should remain explicit in release notes when changed.

## Not guaranteed by default

- Cross-compiler binary compatibility.
- ABI stability for the full C++ API.
- Compatibility for internal implementation details under `src`.

## Related documents

- [Public API status](public-api-status.md)
- [Migration](migration.md)
- [Versioning](versioning.md)
