#include "clc/sim/SimulationPersistence.hpp"

namespace clc::sim {

data::ValidationReport save_simulation_runtime_to_file(
    const SimulationEngine& engine,
    const SettlementRouteCatalog& routes,
    const CaravanFleet& caravans,
    const FactionCatalog& factions,
    const OwnershipCatalog& ownership,
    const ContractCatalog& contracts,
    economy::Wallet wallet,
    const economy::EconomyLedger& ledger,
    const std::filesystem::path& path
) {
    const auto state = capture_simulation_world_state(
        engine,
        routes,
        caravans,
        factions,
        ownership,
        contracts,
        wallet,
        ledger
    );

    auto report = validate_simulation_world_state(state);
    if (!report.ok()) {
        return report;
    }

    return save_simulation_world_state_to_file(state, path);
}

SimulationWorldStateLoadResult load_simulation_runtime_from_file(
    const std::filesystem::path& path,
    SimulationEngine& engine,
    SettlementRouteCatalog& routes,
    CaravanFleet& caravans,
    FactionCatalog& factions,
    OwnershipCatalog& ownership,
    ContractCatalog& contracts,
    economy::Wallet& wallet,
    economy::EconomyLedger& ledger
) {
    auto result = load_simulation_world_state_from_file(path);
    if (!result.ok()) {
        return result;
    }

    auto validation = validate_simulation_world_state(result.state);
    if (!validation.ok()) {
        result.validation = validation;
        return result;
    }

    validation = restore_simulation_runtime_from_world_state(
        result.state,
        engine,
        routes,
        caravans,
        factions,
        ownership,
        contracts,
        wallet,
        ledger
    );
    if (!validation.ok()) {
        result.validation = validation;
    }
    return result;
}

} // namespace clc::sim
