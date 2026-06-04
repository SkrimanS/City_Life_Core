# Core Completion Master Checklist

This checklist tracks the work required to finish City Life Core as a handoff-ready headless simulation SDK core.

Source documents:

- `docs/core-completion-definition.md`
- `docs/deep-simulation-roadmap-addendum.md`
- `docs/detailed-ai-roadmap.md`
- `docs/roadmap.md`

Status values:

- `done` means the core has public API/docs/examples or tested implementation for the item.
- `partial` means a foundation/report/readiness API exists, but runtime/save/replay/package coverage still needs work.
- `todo` means the item still needs implementation.

## 1. Core Architecture

- `done` Headless SDK boundary and non-goals.
- `done` Simulation processor contract.
- `done` Feature toggle model.
- `done` Dependency toggle model.
- `done` Processor registry presets for all major systems.
- `done` Runtime processor orchestration for all major systems.
- `done` Runtime bridge for deep/regional system reports.
- `done` No-hidden-coupling audit across all deep systems.

## 2. v1.x Deep Simulation Systems

- `done` Resources, storage and basic production foundations.
- `done` Market, ledger, trade and economy foundations.
- `done` Routes, caravans, contracts and factions foundations.
- `done` Population, needs, workforce, happiness and migration.
- `done` Weather, seasons, climate and environmental conditions.
- `done` Taxes, fees, policies and economic controls.
- `done` Ecology, agriculture, regeneration and environmental pressure.
- `done` Events, crises, event chains and shock propagation.
- `done` Autonomous simulation actors and decision profiles.
- `done` Content packs, presets and balance profiles.
- `done` Scenario runner, reports, forecasts and comparative simulation.

## 3. Runtime Integration

- `done` Local runtime workflow foundation.
- `done` Action Bridge foundation.
- `done` Server-authoritative envelope foundation.
- `done` Deep systems integrated into runtime tick loop.
- `done` Regional systems integrated into runtime tick loop.
- `done` Deep/regional systems evaluated from standard runtime workflow.
- `done` Bounded runtime event streams for deep/regional systems.

## 4. Save, Load, Replay and Migration

- `done` Existing runtime save/load and replay foundations.
- `done` Schema version registry for deep systems.
- `done` Save/load coverage markers for deep systems.
- `done` Replay diagnostics coverage markers for deep systems.
- `done` Regional save/load readiness.
- `done` Regional replay/audit stream readiness.
- `done` Golden save/replay fixtures for all new deep/regional systems.

## 5. v2.x Authority and Multiplayer Safety

- `done` Actor/session authority foundation.
- `done` System permission matrix.
- `done` Rejected action no-mutation dispatch.
- `done` Snapshot visibility foundation.
- `done` Multiplayer command log/replay foundation.
- `done` Economy consistency polish across all market APIs.
- `done` Logistics and contract authority polish across all APIs.
- `done` Faction, policy and event authority polish across all APIs.

## 6. v3.x Regional and Large World Systems

- `done` Region model and assignments.
- `done` Regional feature/dependency config.
- `done` Cross-region references.
- `done` Regional snapshots and event streams.
- `done` Regional economy, liquidity, volatility and depth.
- `done` Inter-region logistics and trade networks.
- `done` Regional climate, weather and disasters.
- `done` Faction territories and regional control.
- `done` Regional migration and health pressure.
- `done` World history, event partitioning and audit streams.
- `done` Long-running persistence and maintenance.
- `done` Large-world load and benchmark diagnostics.

## 7. C ABI and Foreign-Language Readiness

- `done` Version/time/world/event C ABI.
- `done` C ABI interface version 8.
- `done` Read-only SDK handoff diagnostics.
- `done` Read-only deep system diagnostics through C ABI.
- `done` Read-only regional diagnostics through C ABI.
- `done` Unity/C# wrapper parity for handoff diagnostics.

## 8. Developer Handoff

- `done` Game profile catalog.
- `done` Adoption reports and checklists.
- `done` Game developer handoff report.
- `done` Handoff docs.
- `done` Handoff example.
- `done` Profile-specific starter examples for all profiles.
- `done` Installed SDK validation for all handoff entry points.

## 9. Packaging and Release Readiness

- `done` CMake package foundation.
- `done` Install docs/examples/scripts/data.
- `done` SDK handoff manifest API for installed artifacts.
- `done` Release manifest aligned with latest C ABI and handoff work.
- `done` ZIP SDK package validation after all core changes.
- `done` Final full validation pass.
- `done` Final package generation.

## 10. v4.0.0 Platform Foundation

- `done` Static module registry.
- `done` Core, optional and genre module metadata.
- `done` Module dependency rules.
- `done` Genre profiles for idle/tycoon, city builder, strategy, settlement RPG and survival economy.
- `done` Profile metadata for enabled systems, default rules, data templates, actions and reports.
- `done` Content pack manifest validation.
- `done` Editor/admin query report.
- `done` Unified diagnostics report.
- `done` v3 to v4 migration guidance.
- `done` Platform core docs, example and test.

## 11. Next Execution Order

1. `done` Add a public core completion readiness API.
2. `done` Link readiness API to processor/deep/regional/platform/handoff reports.
3. `done` Add tests and example for readiness.
4. `done` Expand C ABI read-only diagnostics for core readiness.
5. `done` Add runtime processor presets for all major deep/regional systems.
6. `done` Add save/replay coverage markers and fixture plan.
7. `done` Add SDK handoff manifest API and evidence wiring.
8. `done` Align release docs/package validation.
9. `done` Generate final SDK package.
10. `done` Run full `ctest` validation.
