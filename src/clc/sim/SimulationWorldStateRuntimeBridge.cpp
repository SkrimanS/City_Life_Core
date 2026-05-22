#include "clc/sim/SimulationPersistence.hpp"

#include <utility>

namespace clc::sim {

namespace {

clc::GameTime restored_runtime_time(const SimulationWorldState& state) noexcept {
    if (state.time.current_tick() > 0 || state.engine.current_day == 0) {
        return state.time;
    }
    return clc::GameTime{clc::days_to_ticks(state.engine.current_day)};
}

} // namespace

SimulationWorldState capture_simulation_world_state(
    const SimulationEngine& engine,
    const SettlementRouteCatalog& routes,
    const CaravanFleet& caravans,
    const FactionCatalog& factions,
    const OwnershipCatalog& ownership,
    const ContractCatalog& contracts,
    economy::Wallet wallet,
    const economy::EconomyLedger& ledger
) {
    return SimulationWorldState{
        .engine = engine.export_state(),
        .routes = routes,
        .caravans = caravans,
        .factions = factions,
        .ownership = ownership,
        .contracts = contracts,
        .wallet = wallet,
        .ledger_entries = ledger.entries(),
    };
}

SimulationWorldState capture_simulation_world_state(const SimulationRuntime& runtime) {
    auto state = capture_simulation_world_state(
        runtime.engine,
        runtime.routes,
        runtime.caravans,
        runtime.factions,
        runtime.ownership,
        runtime.contracts,
        runtime.wallet,
        runtime.ledger
    );
    state.time = runtime.time;
    return state;
}

data::ValidationReport restore_simulation_runtime_from_world_state(
    const SimulationWorldState& state,
    SimulationEngine& engine,
    SettlementRouteCatalog& routes,
    CaravanFleet& caravans,
    FactionCatalog& factions,
    OwnershipCatalog& ownership,
    ContractCatalog& contracts,
    economy::Wallet& wallet,
    economy::EconomyLedger& ledger
) {
    auto report = validate_simulation_world_state(state);
    if (!report.ok()) {
        return report;
    }

    economy::EconomyLedger restored_ledger;
    report = restore_ledger_from_world_state(state, restored_ledger);
    if (!report.ok()) {
        return report;
    }

    SimulationEngine restored_engine{engine.registry()};
    report = restored_engine.restore_state(state.engine);
    if (!report.ok()) {
        return report;
    }

    auto restored_routes = state.routes;
    auto restored_caravans = state.caravans;
    auto restored_factions = state.factions;
    auto restored_ownership = state.ownership;
    auto restored_contracts = state.contracts;
    auto restored_wallet = state.wallet;

    engine = std::move(restored_engine);
    routes = std::move(restored_routes);
    caravans = std::move(restored_caravans);
    factions = std::move(restored_factions);
    ownership = std::move(restored_ownership);
    contracts = std::move(restored_contracts);
    wallet = restored_wallet;
    ledger = std::move(restored_ledger);
    return report;
}

data::ValidationReport restore_simulation_runtime_from_world_state(
    const SimulationWorldState& state,
    SimulationRuntime& runtime
) {
    SimulationRuntime restored_runtime{runtime.engine.registry()};
    auto report = restore_simulation_runtime_from_world_state(
        state,
        restored_runtime.engine,
        restored_runtime.routes,
        restored_runtime.caravans,
        restored_runtime.factions,
        restored_runtime.ownership,
        restored_runtime.contracts,
        restored_runtime.wallet,
        restored_runtime.ledger
    );
    if (!report.ok()) {
        return report;
    }

    restored_runtime.time = restored_runtime_time(state);
    runtime = std::move(restored_runtime);
    return report;
}

} // namespace clc::sim
