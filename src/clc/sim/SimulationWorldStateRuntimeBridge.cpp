#include "clc/sim/SimulationPersistence.hpp"

#include <utility>

namespace clc::sim {

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
    return capture_simulation_world_state(
        runtime.engine,
        runtime.routes,
        runtime.caravans,
        runtime.factions,
        runtime.ownership,
        runtime.contracts,
        runtime.wallet,
        runtime.ledger
    );
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

    report = engine.restore_state(state.engine);
    if (!report.ok()) {
        return report;
    }

    routes = state.routes;
    caravans = state.caravans;
    factions = state.factions;
    ownership = state.ownership;
    contracts = state.contracts;
    wallet = state.wallet;
    ledger = std::move(restored_ledger);
    return report;
}

data::ValidationReport restore_simulation_runtime_from_world_state(
    const SimulationWorldState& state,
    SimulationRuntime& runtime
) {
    return restore_simulation_runtime_from_world_state(
        state,
        runtime.engine,
        runtime.routes,
        runtime.caravans,
        runtime.factions,
        runtime.ownership,
        runtime.contracts,
        runtime.wallet,
        runtime.ledger
    );
}

} // namespace clc::sim
