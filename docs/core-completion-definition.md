# Core Completion Definition

This document defines what "ready core" means for City Life Core.

## Target

City Life Core should become a ready headless simulation SDK core that game developers can use to build different games on top of it.

The core is not a finished game. It does not own UI, rendering, input, networking transport, replication protocol, accounts, authentication, matchmaking, anti-cheat client software or an engine-specific gameplay framework.

The core owns deterministic simulation processing, validation, diagnostics, events, state changes, reports, configuration models, save/load helpers, replay helpers, authority checks, visibility/snapshot helpers and developer handoff guidance.

## Required Simulation Coverage

The completed core should cover the mechanics from the roadmap and deep simulation addendum as core simulation systems, processors, reports, diagnostics, configs and readiness checks:

- resources, goods, storage, quality, decay and spoilage;
- production chains, workplaces, upkeep and maintenance;
- population, needs, workforce, happiness and migration;
- weather, seasons, climate and environmental conditions;
- logistics, routes, transport, caravans and risk;
- market supply, demand, price pressure, liquidity, volatility, depth and shocks;
- taxes, fees, policies and economic controls;
- factions, influence, control, law and security;
- contracts, commitments, orders, obligations, deadlines, rewards and penalties;
- ecology, agriculture, regeneration and environmental pressure;
- events, crises, event chains and shock propagation;
- autonomous simulation actors and decision profiles;
- content packs, presets and balance profiles;
- scenario runner, reports, forecasts and comparative simulation;
- local runtime orchestration;
- save/load, replay and migration;
- authority, ownership, permission matrix and rejected action audit;
- snapshots, visibility and client sync models;
- regional and large-world simulation systems;
- platform readiness and game developer handoff.

## Completion Criteria

The core is ready when:

- all major systems are represented in public C++ SDK APIs or documented internal processors;
- systems are headless, deterministic, optional/configurable and dependency-toggle compatible;
- cross-system effects are explicit, named, diagnostic-friendly and replay-compatible;
- public docs explain how to use the SDK and what the core intentionally does not do;
- examples show how different game teams should start;
- C ABI exposes stable version/time/world basics and read-only readiness/handoff diagnostics;
- install and package output include headers, docs, examples, scripts and data needed by SDK consumers;
- validation covers build, examples, tests, install consumers, C ABI consumer and package generation.

## Handoff Goal

The final deliverable should be something that can be handed to developers of different games so they can build their own UI, gameplay, server, tools or engine integration on top of City Life Core.

