# Contributing

City Life Core is currently in private early development.

## Branches

- `main` — stable integration branch.
- `develop` — future development integration branch.
- `feature/*` — feature work.
- `bootstrap/*` — early repository/bootstrap work.

## Pull request expectations

Every non-documentation PR should include:

- a clear summary;
- tests or a reason tests are not applicable;
- notes about public API changes;
- notes about simulation invariants affected by the change.

## C++ style baseline

- C++20 minimum.
- Prefer explicit types at API boundaries.
- Prefer typed IDs over raw integers in public APIs.
- Avoid direct state mutation from integration layers; prefer validated commands/actions as the project evolves.
- Avoid floating point for money/resource ledgers in future economy modules.
