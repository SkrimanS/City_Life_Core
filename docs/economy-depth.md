# Economy, factions and contracts depth

Status: v1.7.0 foundation.

This document describes the v1.7.0 source-level helpers that connect market pressure, faction access, contract lifecycle state and resource flow planning. The goal is to make settlement and resource simulations easier to evaluate without turning City Life Core into a game-specific economy rules engine.

The API lives in:

```cpp
#include <clc/sim/EconomyDepth.hpp>
```

It is also included by the umbrella header:

```cpp
#include <clc/CityLifeCore.hpp>
```

## Scope

The v1.7.0 layer helps host games and tools answer practical questions:

- is this contract currently deliverable;
- is it blocked by status, deadline, reputation or resources;
- how does the reward compare with the current market value;
- can a warehouse or caravan satisfy the delivery requirement;
- which contracts in a portfolio are deliverable or blocked.

It does not define taxation, auctions, inflation, AI strategy, diplomacy, legal rules, production planning, UI, persistence storage, networking or settlement ownership policy. Those remain game-specific systems layered around the core.

## Contract assessment

`ContractEconomyAssessment` combines the selected contract, current resource availability, `MarketReport` price data, faction access, current tick and due tick into a deliverability result.

```cpp
const auto assessment = clc::sim::make_contract_economy_assessment(
    contracts,
    "grain_contract",
    factions,
    warehouse,
    market_report,
    clc::days_to_ticks(2)
);
```

`validate_contract_economy_assessment` reports blocking conditions as validation errors and reports below-market rewards as warnings. This lets tools distinguish "cannot deliver" from "can deliver, but probably bad economics".

## Resource flow plans

`ContractResourceFlowPlan` describes whether a specific source can fulfill a contract.

```cpp
const auto storage_flow = clc::sim::plan_contract_flow_from_storage(
    contracts,
    "grain_contract",
    warehouse,
    "warehouse_riverwatch"
);

const auto caravan_flow = clc::sim::plan_contract_flow_from_caravan(
    contracts,
    "grain_contract",
    caravan,
    &ownership,
    "riverwatch"
);
```

Caravan plans check arrival state and, when ownership data is provided, expected owner faction. They do not mutate cargo or fulfill the contract. Actual fulfillment still uses the existing contract fulfillment functions.

## Portfolio summary

`ContractEconomyPortfolioSummary` evaluates all contracts in a catalog against the same market, faction and resource context. It counts deliverable contracts, status blocks, reputation blocks, resource shortages and overdue contracts, and totals reward versus market value.

Digest helpers are provided for tests, tools and audit output:

```cpp
clc::sim::contract_economy_assessment_digest(assessment);
clc::sim::contract_resource_flow_plan_digest(storage_flow);
clc::sim::contract_economy_portfolio_summary_digest(summary);
```

## Extension points

Game-specific economy rules should wrap this layer rather than modify it directly. Common extension points include:

- contract selection and AI prioritization;
- reward multipliers by faction, distance, rarity or risk;
- taxes, tariffs and upkeep;
- long-term inflation or seasonal price changes;
- faction treaty rules beyond the built-in access classification;
- product-specific settlement ownership or warehouse permissions;
- UI explanations and player-facing negotiation.

The core helpers intentionally return plain structs so these rules can be composed in game code, server code, editor tools or tests.
