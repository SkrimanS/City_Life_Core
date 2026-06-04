# Dependency Registry

Dependency toggles declare named links between simulation systems.

Examples:

- `weather.market`
- `weather.route_risk`
- `contract.market_pressure`
- `faction.route_control`
- `ecology.price_pressure`

Processors list `required_dependency_ids`. If a dependency is disabled, the processor does not apply that cross-system effect. This prevents hidden coupling and makes replay/audit output explainable.

