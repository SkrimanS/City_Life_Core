#include "clc/sim/SimulationPersistence.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::ResourceDeliveryContract make_contract(
    std::string_view id,
    clc::sim::ContractStatus status
) {
    return clc::sim::ResourceDeliveryContract{
        .id = std::string{id},
        .display_name = std::string{id},
        .issuer_faction_id = "settlement_faction",
        .receiver_faction_id = "guild_faction",
        .resource_id = "grain",
        .quantity = 10,
        .reward_coins = 25,
        .due_day = 3,
        .status = status,
    };
}

clc::sim::FactionCatalog make_factions() {
    clc::sim::FactionCatalog factions;
    factions.factions.push_back(clc::sim::FactionState{.id = "settlement_faction", .display_name = "Settlement Faction"});
    factions.factions.push_back(clc::sim::FactionState{.id = "guild_faction", .display_name = "Guild Faction"});
    return factions;
}

clc::sim::ContractCatalog make_persisted_terminal_contracts() {
    clc::sim::ContractCatalog contracts;
    contracts.contracts.push_back(make_contract("persisted_fulfilled", clc::sim::ContractStatus::fulfilled));
    contracts.contracts.push_back(make_contract("persisted_failed", clc::sim::ContractStatus::failed));
    contracts.contracts.push_back(make_contract("persisted_cancelled", clc::sim::ContractStatus::cancelled));
    return contracts;
}

void require_contract_status(
    const clc::sim::ContractCatalog& contracts,
    std::string_view contract_id,
    clc::sim::ContractStatus expected_status
) {
    const auto* contract = clc::sim::contract_by_id(contracts, contract_id);
    require(contract != nullptr, "persisted contract should exist after restore/load");
    require(contract->status == expected_status, "persisted contract status should be preserved");
}

void require_terminal_contracts_preserved(const clc::sim::ContractCatalog& contracts) {
    require(contracts.contracts.size() == 3, "all terminal contracts should be preserved");
    require_contract_status(contracts, "persisted_fulfilled", clc::sim::ContractStatus::fulfilled);
    require_contract_status(contracts, "persisted_failed", clc::sim::ContractStatus::failed);
    require_contract_status(contracts, "persisted_cancelled", clc::sim::ContractStatus::cancelled);
}

} // namespace

int main() {
    clc::sim::SimulationWorldState state;
    state.factions = make_factions();
    state.contracts = make_persisted_terminal_contracts();

    require(clc::sim::validate_simulation_world_state(state).ok(), "persisted terminal contracts should validate in world state");

    const auto serialized = clc::sim::serialize_simulation_world_state(state);
    require(serialized.find("contract\tpersisted_fulfilled") != std::string::npos, "serialized state should include fulfilled contract");
    require(serialized.find("\tfulfilled") != std::string::npos, "serialized state should include fulfilled status");
    require(serialized.find("\tfailed") != std::string::npos, "serialized state should include failed status");
    require(serialized.find("\tcancelled") != std::string::npos, "serialized state should include cancelled status");

    const auto deserialized = clc::sim::deserialize_simulation_world_state(serialized);
    require(deserialized.ok(), "persisted terminal contracts should deserialize");
    require_terminal_contracts_preserved(deserialized.state.contracts);

    clc::sim::SimulationEngine restored_engine{clc::data::DataRegistry{}};
    clc::sim::SettlementRouteCatalog restored_routes;
    clc::sim::CaravanFleet restored_caravans;
    clc::sim::FactionCatalog restored_factions;
    clc::sim::OwnershipCatalog restored_ownership;
    clc::sim::ContractCatalog restored_contracts;
    clc::economy::Wallet restored_wallet;
    clc::economy::EconomyLedger restored_ledger;

    require(clc::sim::restore_simulation_runtime_from_world_state(
        deserialized.state,
        restored_engine,
        restored_routes,
        restored_caravans,
        restored_factions,
        restored_ownership,
        restored_contracts,
        restored_wallet,
        restored_ledger
    ).ok(), "persisted terminal contracts should restore into runtime components");
    require_terminal_contracts_preserved(restored_contracts);

    const auto directory = std::filesystem::temp_directory_path() / "clc_persisted_terminal_contract_restore_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto save_path = directory / "runtime.clcs";

    clc::sim::SimulationEngine source_engine{clc::data::DataRegistry{}};
    clc::sim::SettlementRouteCatalog source_routes;
    clc::sim::CaravanFleet source_caravans;
    clc::sim::OwnershipCatalog source_ownership;
    clc::economy::Wallet source_wallet;
    clc::economy::EconomyLedger source_ledger;

    require(clc::sim::save_simulation_runtime_to_file(
        source_engine,
        source_routes,
        source_caravans,
        state.factions,
        source_ownership,
        state.contracts,
        source_wallet,
        source_ledger,
        save_path
    ).ok(), "runtime save should accept persisted terminal contracts");

    clc::sim::SimulationRuntime loaded_runtime{clc::data::DataRegistry{}};
    const auto runtime_load = clc::sim::load_simulation_runtime_from_file(save_path, loaded_runtime);
    require(runtime_load.ok(), "runtime load should accept persisted terminal contracts");
    require_terminal_contracts_preserved(loaded_runtime.contracts);

    std::filesystem::remove_all(directory);
    return 0;
}
