#pragma once

#include "clc/data/DataRegistry.hpp"
#include "clc/economy/Ledger.hpp"
#include "clc/economy/Trade.hpp"
#include "clc/sim/Contracts.hpp"
#include "clc/sim/Ownership.hpp"
#include "clc/sim/SimulationEngine.hpp"

#include <utility>

namespace clc::sim {

struct SimulationRuntime final {
    explicit SimulationRuntime(data::DataRegistry registry)
        : engine(std::move(registry)) {}

    SimulationEngine engine;
    SettlementRouteCatalog routes{};
    CaravanFleet caravans{};
    FactionCatalog factions{};
    OwnershipCatalog ownership{};
    ContractCatalog contracts{};
    economy::Wallet wallet{};
    economy::EconomyLedger ledger{};
};

} // namespace clc::sim
