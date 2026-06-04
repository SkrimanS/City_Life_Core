# Simulation Processing Doctrine

City Life Core is a headless C++ simulation SDK, not a finished game, renderer, UI framework, account service, matchmaking layer, or engine-specific gameplay framework.

The core owns deterministic domain processing:

- validate world definitions and configuration;
- process ticks through ordered simulation processors;
- apply feature and dependency toggles explicitly;
- emit state deltas, events, diagnostics, validation warnings/errors, and cause breakdowns;
- remain save/load aware, replay aware, diagnostics friendly, and engine agnostic.

Downstream games, servers, editors, and tools own presentation, networking, accounts, product gameplay, and platform-specific integration.

Processor coupling must be explicit. A system such as weather must not secretly mutate a market, route, faction, or population state. The intended path is:

```text
source system -> named effect/dependency -> target processor -> diagnostics/cause breakdown
```

Every dependency should be documented, toggleable, deterministic, visible in diagnostics, and compatible with replay.

