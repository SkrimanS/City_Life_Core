#pragma once

#include "clc/core/Time.hpp"
#include "clc/data/Validation.hpp"
#include "clc/economy/Ledger.hpp"
#include "clc/economy/Trade.hpp"
#include "clc/sim/Contracts.hpp"
#include "clc/sim/Ownership.hpp"
#include "clc/sim/SimulationEngine.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct SimulationSnapshotLoadResult final {
    SimulationSnapshot snapshot{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

struct SimulationWorldState final {
    SimulationEngineState engine{};
    clc::GameTime time{};
    SettlementRouteCatalog routes{};
    CaravanFleet caravans{};
    FactionCatalog factions{};
    OwnershipCatalog ownership{};
    ContractCatalog contracts{};
    economy::Wallet wallet{};
    std::vector<economy::LedgerEntry> ledger_entries{};
};

struct SimulationWorldStateLoadResult final {
    SimulationWorldState state{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

[[nodiscard]] std::string serialize_simulation_snapshot(const SimulationSnapshot& snapshot);
[[nodiscard]] SimulationSnapshotLoadResult deserialize_simulation_snapshot(std::string_view content);

[[nodiscard]] data::ValidationReport save_simulation_snapshot_to_file(
    const SimulationSnapshot& snapshot,
    const std::filesystem::path& path
);

[[nodiscard]] SimulationSnapshotLoadResult load_simulation_snapshot_from_file(const std::filesystem::path& path);

[[nodiscard]] data::ValidationReport validate_simulation_world_state(const SimulationWorldState& state);

[[nodiscard]] SimulationWorldState capture_simulation_world_state(
    const SimulationEngine& engine,
    const SettlementRouteCatalog& routes,
    const CaravanFleet& caravans,
    const FactionCatalog& factions,
    const OwnershipCatalog& ownership,
    const ContractCatalog& contracts,
    economy::Wallet wallet,
    const economy::EconomyLedger& ledger
);

[[nodiscard]] SimulationWorldState capture_simulation_world_state(const SimulationRuntime& runtime);

[[nodiscard]] data::ValidationReport restore_simulation_runtime_from_world_state(
    const SimulationWorldState& state,
    SimulationEngine& engine,
    SettlementRouteCatalog& routes,
    CaravanFleet& caravans,
    FactionCatalog& factions,
    OwnershipCatalog& ownership,
    ContractCatalog& contracts,
    economy::Wallet& wallet,
    economy::EconomyLedger& ledger
);

[[nodiscard]] data::ValidationReport restore_simulation_runtime_from_world_state(
    const SimulationWorldState& state,
    SimulationRuntime& runtime
);

[[nodiscard]] data::ValidationReport save_simulation_runtime_to_file(
    const SimulationEngine& engine,
    const SettlementRouteCatalog& routes,
    const CaravanFleet& caravans,
    const FactionCatalog& factions,
    const OwnershipCatalog& ownership,
    const ContractCatalog& contracts,
    economy::Wallet wallet,
    const economy::EconomyLedger& ledger,
    const std::filesystem::path& path
);

[[nodiscard]] data::ValidationReport save_simulation_runtime_to_file(
    const SimulationRuntime& runtime,
    const std::filesystem::path& path
);

[[nodiscard]] SimulationWorldStateLoadResult load_simulation_runtime_from_file(
    const std::filesystem::path& path,
    SimulationEngine& engine,
    SettlementRouteCatalog& routes,
    CaravanFleet& caravans,
    FactionCatalog& factions,
    OwnershipCatalog& ownership,
    ContractCatalog& contracts,
    economy::Wallet& wallet,
    economy::EconomyLedger& ledger
);

[[nodiscard]] SimulationWorldStateLoadResult load_simulation_runtime_from_file(
    const std::filesystem::path& path,
    SimulationRuntime& runtime
);

[[nodiscard]] std::string serialize_simulation_world_state(const SimulationWorldState& state);
[[nodiscard]] SimulationWorldStateLoadResult deserialize_simulation_world_state(std::string_view content);

[[nodiscard]] data::ValidationReport save_simulation_world_state_to_file(
    const SimulationWorldState& state,
    const std::filesystem::path& path
);

[[nodiscard]] SimulationWorldStateLoadResult load_simulation_world_state_from_file(const std::filesystem::path& path);
[[nodiscard]] data::ValidationReport restore_ledger_from_world_state(
    const SimulationWorldState& state,
    economy::EconomyLedger& ledger
);

} // namespace clc::sim
