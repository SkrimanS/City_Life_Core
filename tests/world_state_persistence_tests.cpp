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

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "grain should register");
    require(registry.add(clc::data::ResourceDefinition{.id = "wood", .display_name = "Wood", .category = "construction", .base_value = 6}).ok(), "wood should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "riverwatch", .display_name = "Riverwatch", .starting_population = 120}).ok(), "riverwatch definition should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "hillford", .display_name = "Hillford", .starting_population = 80}).ok(), "hillford definition should register");
    return registry;
}

clc::sim::SimulationWorldState make_world_state() {
    clc::sim::SimulationWorldState state;
    state.engine.current_day = 12;
    state.engine.market_demands.push_back(clc::sim::SimulationMarketDemand{.resource_id = "grain", .demand = 80});
    state.engine.events.push_back(clc::sim::SimulationEvent{.day = 12, .type = "contract", .message = "grain delivered"});

    clc::sim::SettlementState riverwatch{.id = "riverwatch", .display_name = "Riverwatch", .population = 120};
    require(riverwatch.storage.add("grain", 90).ok(), "riverwatch should accept grain");
    require(riverwatch.storage.add("wood", 30).ok(), "riverwatch should accept wood");
    riverwatch.buildings.push_back(clc::sim::BuildingInstance{.definition_id = "farm", .assigned_workers = 8});
    state.engine.settlements.push_back(riverwatch);

    clc::sim::SettlementState hillford{.id = "hillford", .display_name = "Hillford", .population = 80};
    require(hillford.storage.add("grain", 20).ok(), "hillford should accept grain");
    state.engine.settlements.push_back(hillford);

    state.routes.routes.push_back(clc::sim::SettlementRoute{
        .id = "riverwatch_to_hillford",
        .display_name = "Riverwatch to Hillford",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "hillford",
        .travel_days = 3,
    });

    auto caravan = clc::sim::create_caravan_for_route(
        state.routes.routes[0],
        "caravan_001",
        "First Caravan"
    );
    require(caravan.cargo.add("grain", 50).ok(), "caravan should accept grain cargo");
    require(clc::sim::advance_caravan_day(caravan).days_remaining_after == 2, "caravan should advance once");
    state.caravans.caravans.push_back(caravan);

    state.factions.factions.push_back(clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"});
    state.factions.factions.push_back(clc::sim::FactionState{.id = "traders_guild", .display_name = "Traders Guild"});
    state.factions.reputations.push_back(clc::sim::FactionReputation{.from_faction_id = "riverwatch", .to_faction_id = "traders_guild", .value = 15});

    state.ownership.settlements.push_back(clc::sim::SettlementOwnership{.settlement_id = "riverwatch", .faction_id = "riverwatch"});
    state.ownership.caravans.push_back(clc::sim::CaravanOwnership{.caravan_id = "caravan_001", .faction_id = "riverwatch"});

    state.contracts.contracts.push_back(clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery_001",
        .display_name = "Grain Delivery 001",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 50,
        .reward_coins = 120,
        .due_day = 14,
        .status = clc::sim::ContractStatus::open,
    });
    state.contracts.contracts.push_back(clc::sim::ResourceDeliveryContract{
        .id = "grain_delivery_done",
        .display_name = "Completed Grain Delivery",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 20,
        .reward_coins = 40,
        .due_day = 10,
        .status = clc::sim::ContractStatus::fulfilled,
    });

    state.wallet.coins = 250;
    state.ledger_entries.push_back(clc::economy::LedgerEntry{
        .sequence = 1,
        .type = clc::economy::LedgerEntryType::contract_reward,
        .resource_id = "grain",
        .quantity = 20,
        .unit_price = 0,
        .total_price = 40,
        .reference_id = "grain_delivery_done",
        .note = "contract reward payout",
    });

    return state;
}

} // namespace

int main() {
    const auto state = make_world_state();
    require(clc::sim::validate_simulation_world_state(state).ok(), "valid world state should pass integrated validation");

    auto broken_route_state = state;
    broken_route_state.routes.routes[0].destination_settlement_id = "missing_settlement";
    require(!clc::sim::validate_simulation_world_state(broken_route_state).ok(), "world state validation should reject routes with unknown settlements");

    auto broken_caravan_state = state;
    broken_caravan_state.caravans.caravans[0].route_id = "missing_route";
    require(!clc::sim::validate_simulation_world_state(broken_caravan_state).ok(), "world state validation should reject caravans with unknown routes");

    auto broken_reputation_state = state;
    broken_reputation_state.factions.reputations[0].to_faction_id = "missing_faction";
    require(!clc::sim::validate_simulation_world_state(broken_reputation_state).ok(), "world state validation should reject reputations with unknown endpoints");

    auto broken_ownership_state = state;
    broken_ownership_state.ownership.caravans[0].faction_id = "missing_faction";
    require(!clc::sim::validate_simulation_world_state(broken_ownership_state).ok(), "world state validation should reject ownership with unknown faction");

    auto broken_contract_state = state;
    broken_contract_state.contracts.contracts[0].receiver_faction_id = "missing_faction";
    require(!clc::sim::validate_simulation_world_state(broken_contract_state).ok(), "world state validation should reject contracts with unknown factions");

    auto duplicate_route_state = state;
    duplicate_route_state.routes.routes.push_back(duplicate_route_state.routes.routes[0]);
    require(!clc::sim::validate_simulation_world_state(duplicate_route_state).ok(), "world state validation should reject duplicate route ids");

    const auto serialized = clc::sim::serialize_simulation_world_state(state);
    require(serialized.find("CLC_SIM_WORLD_STATE\t1") != std::string::npos, "serialized state should include header");
    require(serialized.find("contract\tgrain_delivery_001") != std::string::npos, "serialized state should include open contract");
    require(serialized.find("ledger\t1\tcontract_reward") != std::string::npos, "serialized state should include ledger entry");

    const auto loaded = clc::sim::deserialize_simulation_world_state(serialized);
    require(loaded.ok(), "serialized world state should deserialize");
    require(loaded.state.engine.current_day == 12, "loaded world state should preserve current day");
    require(loaded.state.engine.settlements.size() == 2, "loaded world state should preserve settlements");
    require(loaded.state.engine.settlements[0].id == "riverwatch", "loaded world state should preserve settlement id");
    require(loaded.state.engine.settlements[0].storage.amount("grain") == 90, "loaded world state should preserve settlement storage");
    require(loaded.state.engine.settlements[0].buildings.size() == 1, "loaded world state should preserve buildings");
    require(loaded.state.engine.market_demands.size() == 1, "loaded world state should preserve market demands");
    require(loaded.state.engine.events.size() == 1, "loaded world state should preserve events");

    require(loaded.state.routes.routes.size() == 1, "loaded world state should preserve routes");
    require(loaded.state.routes.routes[0].travel_days == 3, "loaded world state should preserve route travel days");
    require(loaded.state.caravans.caravans.size() == 1, "loaded world state should preserve caravans");
    require(loaded.state.caravans.caravans[0].days_remaining == 2, "loaded world state should preserve caravan progress");
    require(loaded.state.caravans.caravans[0].cargo.amount("grain") == 50, "loaded world state should preserve caravan cargo");

    require(loaded.state.factions.factions.size() == 2, "loaded world state should preserve factions");
    require(loaded.state.factions.reputations.size() == 1, "loaded world state should preserve reputations");
    require(loaded.state.factions.reputations[0].value == 15, "loaded world state should preserve reputation value");
    require(loaded.state.ownership.settlements.size() == 1, "loaded world state should preserve settlement ownership");
    require(loaded.state.ownership.caravans.size() == 1, "loaded world state should preserve caravan ownership");

    require(loaded.state.contracts.contracts.size() == 2, "loaded world state should preserve contracts");
    require(loaded.state.contracts.contracts[0].status == clc::sim::ContractStatus::open, "loaded world state should preserve open status");
    require(loaded.state.contracts.contracts[1].status == clc::sim::ContractStatus::fulfilled, "loaded world state should preserve fulfilled status");
    require(loaded.state.wallet.coins == 250, "loaded world state should preserve wallet");
    require(loaded.state.ledger_entries.size() == 1, "loaded world state should preserve ledger entries");
    require(loaded.state.ledger_entries[0].reference_id == "grain_delivery_done", "loaded world state should preserve ledger reference");
    require(loaded.state.ledger_entries[0].total_price == 40, "loaded world state should preserve ledger reward amount");

    clc::economy::EconomyLedger restored_ledger;
    require(clc::sim::restore_ledger_from_world_state(loaded.state, restored_ledger).ok(), "loaded ledger entries should restore into live ledger");
    require(restored_ledger.entries().size() == 1, "restored live ledger should contain loaded entry");
    require(restored_ledger.next_sequence() == 2, "restored live ledger should continue after max loaded sequence");
    require(restored_ledger.record_contract_reward("grain_delivery_next", "grain", 10, 25, "next payout"), "restored live ledger should accept next reward entry");
    require(restored_ledger.entries().size() == 2, "restored live ledger should append next entry");
    require(restored_ledger.entries()[1].sequence == 2, "restored live ledger should assign next sequence");
    require(restored_ledger.total_contract_rewards("grain") == 65, "restored live ledger should aggregate old and new rewards");

    clc::sim::SimulationEngine source_engine{make_registry()};
    require(source_engine.restore_state(state.engine).ok(), "source engine should restore base state");
    clc::economy::EconomyLedger source_ledger;
    require(clc::sim::restore_ledger_from_world_state(state, source_ledger).ok(), "source ledger should restore base entries");
    const auto captured = clc::sim::capture_simulation_world_state(
        source_engine,
        state.routes,
        state.caravans,
        state.factions,
        state.ownership,
        state.contracts,
        state.wallet,
        source_ledger
    );
    const auto captured_roundtrip = clc::sim::deserialize_simulation_world_state(clc::sim::serialize_simulation_world_state(captured));
    require(captured_roundtrip.ok(), "captured runtime world state should roundtrip");

    clc::sim::SimulationEngine target_engine{make_registry()};
    clc::sim::SettlementRouteCatalog target_routes;
    clc::sim::CaravanFleet target_caravans;
    clc::sim::FactionCatalog target_factions;
    clc::sim::OwnershipCatalog target_ownership;
    clc::sim::ContractCatalog target_contracts;
    clc::economy::Wallet target_wallet;
    clc::economy::EconomyLedger target_ledger;
    require(clc::sim::restore_simulation_runtime_from_world_state(
        captured_roundtrip.state,
        target_engine,
        target_routes,
        target_caravans,
        target_factions,
        target_ownership,
        target_contracts,
        target_wallet,
        target_ledger
    ).ok(), "captured state should restore full runtime");
    require(target_engine.current_day() == 12, "runtime restore should restore engine day");
    require(target_engine.settlement_resource_amount("riverwatch", "grain") == 90, "runtime restore should restore settlement storage");
    require(target_routes.routes.size() == 1, "runtime restore should restore routes");
    require(target_caravans.caravans.size() == 1, "runtime restore should restore caravans");
    require(target_caravans.caravans[0].days_remaining == 2, "runtime restore should restore caravan progress");
    require(target_factions.factions.size() == 2, "runtime restore should restore factions");
    require(target_ownership.caravans.size() == 1, "runtime restore should restore ownership");
    require(target_contracts.contracts.size() == 2, "runtime restore should restore contracts");
    require(target_wallet.coins == 250, "runtime restore should restore wallet");
    require(target_ledger.next_sequence() == 2, "runtime restore should restore live ledger next sequence");
    require(target_ledger.record_contract_reward("grain_delivery_after_restore", "grain", 5, 15, "after restore"), "runtime-restored ledger should continue recording");
    require(target_ledger.entries()[1].sequence == 2, "runtime-restored ledger should continue sequence");
    require(target_engine.advance_day().day == 13, "runtime-restored engine should continue simulation days");

    auto broken_runtime_state = captured_roundtrip.state;
    broken_runtime_state.ledger_entries[0].quantity = 0;
    clc::sim::SimulationEngine untouched_engine{make_registry()};
    clc::sim::SettlementRouteCatalog untouched_routes;
    clc::sim::CaravanFleet untouched_caravans;
    clc::sim::FactionCatalog untouched_factions;
    clc::sim::OwnershipCatalog untouched_ownership;
    clc::sim::ContractCatalog untouched_contracts;
    clc::economy::Wallet untouched_wallet{.coins = 999};
    clc::economy::EconomyLedger untouched_ledger;
    require(!clc::sim::restore_simulation_runtime_from_world_state(
        broken_runtime_state,
        untouched_engine,
        untouched_routes,
        untouched_caravans,
        untouched_factions,
        untouched_ownership,
        untouched_contracts,
        untouched_wallet,
        untouched_ledger
    ).ok(), "broken runtime state should fail before mutation");
    require(untouched_engine.current_day() == 0, "failed runtime restore should not mutate engine");
    require(untouched_routes.routes.empty(), "failed runtime restore should not mutate routes");
    require(untouched_wallet.coins == 999, "failed runtime restore should not mutate wallet");
    require(untouched_ledger.entries().empty(), "failed runtime restore should not mutate ledger");

    auto broken_reference_runtime_state = captured_roundtrip.state;
    broken_reference_runtime_state.caravans.caravans[0].route_id = "missing_route";
    require(!clc::sim::restore_simulation_runtime_from_world_state(
        broken_reference_runtime_state,
        untouched_engine,
        untouched_routes,
        untouched_caravans,
        untouched_factions,
        untouched_ownership,
        untouched_contracts,
        untouched_wallet,
        untouched_ledger
    ).ok(), "broken referenced runtime state should fail validation before mutation");
    require(untouched_engine.current_day() == 0, "failed reference restore should not mutate engine");
    require(untouched_routes.routes.empty(), "failed reference restore should not mutate routes");

    const auto directory = std::filesystem::temp_directory_path() / "clc_world_state_persistence_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);
    const auto file_path = directory / "world_state.clcs";

    const auto save_report = clc::sim::save_simulation_world_state_to_file(state, file_path);
    require(save_report.ok(), "world state should save to file");
    const auto loaded_from_file = clc::sim::load_simulation_world_state_from_file(file_path);
    require(loaded_from_file.ok(), "world state should load from file");
    require(loaded_from_file.state.contracts.contracts.size() == 2, "file roundtrip should preserve contracts");
    require(loaded_from_file.state.caravans.caravans[0].cargo.amount("grain") == 50, "file roundtrip should preserve caravan cargo");
    require(loaded_from_file.state.ledger_entries[0].reference_id == "grain_delivery_done", "file roundtrip should preserve ledger reference");

    std::filesystem::remove_all(directory);

    const auto invalid = clc::sim::deserialize_simulation_world_state("day\t1\n");
    require(!invalid.ok(), "world state without header should fail");

    clc::economy::EconomyLedger invalid_ledger;
    clc::sim::SimulationWorldState invalid_ledger_state;
    invalid_ledger_state.ledger_entries.push_back(clc::economy::LedgerEntry{
        .sequence = 1,
        .type = clc::economy::LedgerEntryType::contract_reward,
        .resource_id = "grain",
        .quantity = 0,
        .unit_price = 0,
        .total_price = 40,
        .reference_id = "broken",
    });
    require(!clc::sim::restore_ledger_from_world_state(invalid_ledger_state, invalid_ledger).ok(), "invalid loaded ledger entries should fail restore");
    require(invalid_ledger.entries().empty(), "failed ledger restore should not mutate live ledger");

    return 0;
}
