# Deep Simulation Systems

`DeepSimulationSystems.hpp` adds compact headless models for the v1.x deep simulation line from the addendum.

The API covers:

- population, needs, happiness and migration diagnostics;
- weather, seasons and climate impact diagnostics;
- taxes, fees and policy impact diagnostics;
- ecology, agriculture pressure and regeneration diagnostics;
- world events, crises and shock propagation;
- deterministic autonomous decision profiles;
- versioned schema registry;
- deep scenario preset validation.

These systems are intentionally processor-friendly and engine-agnostic. They produce reports, reasons, pressure scores and digest strings that can feed runtime processors, tools, backend services or downstream gameplay layers.

The module does not provide UI, rendering, networking, accounts, matchmaking, scripting, dynamic plugins or product-specific game rules.

Related roadmap scope:

- v1.10.0 population/needs/workforce/happiness/migration;
- v1.11.0 weather/seasons/climate;
- v1.15.0 taxes/fees/policies;
- v1.18.0 ecology/agriculture/regeneration;
- v1.19.0 events/crises/shock propagation;
- v1.20.0 autonomous simulation actors;
- v1.21.0 content/profile readiness through schema and preset validation;
- v1.22.0 scenario/report preparation.

