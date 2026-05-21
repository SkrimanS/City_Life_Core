#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void expect_runtime_drift_detected(
    const clc::sim::SimulationRuntime& expected,
    const clc::sim::SimulationRuntime& drifted,
    std::string_view message
) {
    const auto drift_match = clc::sim::validate_simulation_runtimes_match(expected, drifted);
    require(!drift_match.ok(), message);
}

void restore_engine_state(
    clc::sim::SimulationRuntime& runtime,
    clc::sim::SimulationEngineState state,
    std::string_view message
) {
    const auto restore_report = runtime.engine.restore_state(state);
    require(restore_report.ok(), message);
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "bootstrap failed");

    auto& runtime = bootstrap.runtime;

    auto created = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "persistence_validation_caravan",
        "Persistence Validation Caravan"
    );
    require(created.ok(), "caravan creation failed");

    clc::sim::SimulationRuntime loaded{clc::sim::make_basic_runtime_scenario_registry()};

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_persistence_validation_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    const auto path = directory / "runtime_validation.clcs";

    const auto validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(
        runtime,
        loaded,
        path
    );
    require(validation.ok(), "roundtrip validation failed");

    const auto runtime_match = clc::sim::validate_simulation_runtimes_match(runtime, loaded);
    require(runtime_match.ok(), "runtime match validation failed");

    auto current_day_drifted = loaded;
    auto current_day_state = current_day_drifted.engine.export_state();
    ++current_day_state.current_day;
    restore_engine_state(current_day_drifted, current_day_state, "current day drift setup failed");
    expect_runtime_drift_detected(runtime, current_day_drifted, "runtime match unexpectedly accepted current day drift");

    auto engine_storage_drifted = loaded;
    require(
        engine_storage_drifted.engine.add_resource_to_settlement("riverwatch", "grain", 1).ok(),
        "engine settlement storage drift setup failed"
    );
    expect_runtime_drift_detected(runtime, engine_storage_drifted, "runtime match unexpectedly accepted engine settlement storage drift");

    auto engine_event_drifted = loaded;
    engine_event_drifted.engine.add_resource_to_settlement_command("riverwatch", "grain", 1);
    expect_runtime_drift_detected(runtime, engine_event_drifted, "runtime match unexpectedly accepted engine event drift");

    auto market_demand_drifted = loaded;
    require(
        market_demand_drifted.engine.market().set_demand("grain", 100).ok(),
        "engine market demand drift setup failed"
    );
    expect_runtime_drift_detected(runtime, market_demand_drifted, "runtime match unexpectedly accepted engine market demand drift");

    auto population_drifted = loaded;
    auto population_state = population_drifted.engine.export_state();
    ++population_state.settlements[0].population;
    restore_engine_state(population_drifted, population_state, "engine settlement population drift setup failed");
    expect_runtime_drift_detected(runtime, population_drifted, "runtime match unexpectedly accepted engine settlement population drift");

    auto settlement_id_drifted = loaded;
    auto settlement_id_state = settlement_id_drifted.engine.export_state();
    settlement_id_state.settlements[0].id = "drifted_settlement";
    restore_engine_state(settlement_id_drifted, settlement_id_state, "engine settlement id drift setup failed");
    expect_runtime_drift_detected(runtime, settlement_id_drifted, "runtime match unexpectedly accepted engine settlement id drift");

    auto settlement_name_drifted = loaded;
    auto settlement_name_state = settlement_name_drifted.engine.export_state();
    settlement_name_state.settlements[0].display_name = "Drifted Settlement";
    restore_engine_state(settlement_name_drifted, settlement_name_state, "engine settlement display name drift setup failed");
    expect_runtime_drift_detected(runtime, settlement_name_drifted, "runtime match unexpectedly accepted engine settlement display name drift");

    auto building_drifted = loaded;
    auto building_state = building_drifted.engine.export_state();
    building_state.settlements[0].buildings.push_back(clc::sim::BuildingInstance{
        .definition_id = "drifted_building",
        .assigned_workers = 1,
    });
    restore_engine_state(building_drifted, building_state, "engine settlement building drift setup failed");
    expect_runtime_drift_detected(runtime, building_drifted, "runtime match unexpectedly accepted engine settlement building drift");

    auto faction_id_drifted = loaded;
    faction_id_drifted.factions.factions[0].id = "drifted_faction";
    expect_runtime_drift_detected(runtime, faction_id_drifted, "runtime match unexpectedly accepted faction id drift");

    auto faction_name_drifted = loaded;
    faction_name_drifted.factions.factions[0].display_name = "Drifted Faction";
    expect_runtime_drift_detected(runtime, faction_name_drifted, "runtime match unexpectedly accepted faction display name drift");

    auto faction_reputation_drifted = loaded;
    ++faction_reputation_drifted.factions.reputations[0].value;
    expect_runtime_drift_detected(runtime, faction_reputation_drifted, "runtime match unexpectedly accepted faction reputation drift");

    auto wallet_drifted = loaded;
    ++wallet_drifted.wallet.coins;
    expect_runtime_drift_detected(runtime, wallet_drifted, "runtime match unexpectedly accepted wallet drift");

    auto route_id_drifted = loaded;
    route_id_drifted.routes.routes[0].id = "drifted_route";
    expect_runtime_drift_detected(runtime, route_id_drifted, "runtime match unexpectedly accepted route id drift");

    auto route_name_drifted = loaded;
    route_name_drifted.routes.routes[0].display_name = "Drifted Route";
    expect_runtime_drift_detected(runtime, route_name_drifted, "runtime match unexpectedly accepted route display name drift");

    auto route_origin_drifted = loaded;
    route_origin_drifted.routes.routes[0].origin_settlement_id = "drifted_origin";
    expect_runtime_drift_detected(runtime, route_origin_drifted, "runtime match unexpectedly accepted route origin drift");

    auto route_destination_drifted = loaded;
    route_destination_drifted.routes.routes[0].destination_settlement_id = "drifted_destination";
    expect_runtime_drift_detected(runtime, route_destination_drifted, "runtime match unexpectedly accepted route destination drift");

    auto route_duration_drifted = loaded;
    ++route_duration_drifted.routes.routes[0].travel_days;
    expect_runtime_drift_detected(runtime, route_duration_drifted, "runtime match unexpectedly accepted route travel duration drift");

    auto caravan_id_drifted = loaded;
    caravan_id_drifted.caravans.caravans[0].id = "drifted_caravan";
    expect_runtime_drift_detected(runtime, caravan_id_drifted, "runtime match unexpectedly accepted caravan id drift");

    auto caravan_name_drifted = loaded;
    caravan_name_drifted.caravans.caravans[0].display_name = "Drifted Caravan";
    expect_runtime_drift_detected(runtime, caravan_name_drifted, "runtime match unexpectedly accepted caravan display name drift");

    auto caravan_route_drifted = loaded;
    caravan_route_drifted.caravans.caravans[0].route_id = "drifted_route";
    expect_runtime_drift_detected(runtime, caravan_route_drifted, "runtime match unexpectedly accepted caravan route drift");

    auto caravan_origin_drifted = loaded;
    caravan_origin_drifted.caravans.caravans[0].origin_settlement_id = "drifted_origin";
    expect_runtime_drift_detected(runtime, caravan_origin_drifted, "runtime match unexpectedly accepted caravan origin drift");

    auto caravan_destination_drifted = loaded;
    caravan_destination_drifted.caravans.caravans[0].destination_settlement_id = "drifted_destination";
    expect_runtime_drift_detected(runtime, caravan_destination_drifted, "runtime match unexpectedly accepted caravan destination drift");

    auto caravan_total_days_drifted = loaded;
    ++caravan_total_days_drifted.caravans.caravans[0].total_travel_days;
    expect_runtime_drift_detected(runtime, caravan_total_days_drifted, "runtime match unexpectedly accepted caravan total travel days drift");

    auto caravan_progress_drifted = loaded;
    --caravan_progress_drifted.caravans.caravans[0].days_remaining;
    expect_runtime_drift_detected(runtime, caravan_progress_drifted, "runtime match unexpectedly accepted caravan progress drift");

    auto caravan_cargo_drifted = loaded;
    require(
        caravan_cargo_drifted.caravans.caravans[0].cargo.add("grain", 1).ok(),
        "caravan cargo drift setup failed"
    );
    expect_runtime_drift_detected(runtime, caravan_cargo_drifted, "runtime match unexpectedly accepted caravan cargo drift");

    auto ownership_settlement_drifted = loaded;
    ownership_settlement_drifted.ownership.settlements[0].faction_id = "drifted_faction";
    expect_runtime_drift_detected(runtime, ownership_settlement_drifted, "runtime match unexpectedly accepted settlement ownership drift");

    auto contract_id_drifted = loaded;
    contract_id_drifted.contracts.contracts[0].id = "drifted_contract";
    expect_runtime_drift_detected(runtime, contract_id_drifted, "runtime match unexpectedly accepted contract id drift");

    auto contract_name_drifted = loaded;
    contract_name_drifted.contracts.contracts[0].display_name = "Drifted Contract";
    expect_runtime_drift_detected(runtime, contract_name_drifted, "runtime match unexpectedly accepted contract display name drift");

    auto contract_issuer_drifted = loaded;
    contract_issuer_drifted.contracts.contracts[0].issuer_faction_id = "drifted_issuer";
    expect_runtime_drift_detected(runtime, contract_issuer_drifted, "runtime match unexpectedly accepted contract issuer drift");

    auto contract_receiver_drifted = loaded;
    contract_receiver_drifted.contracts.contracts[0].receiver_faction_id = "drifted_receiver";
    expect_runtime_drift_detected(runtime, contract_receiver_drifted, "runtime match unexpectedly accepted contract receiver drift");

    auto contract_resource_drifted = loaded;
    contract_resource_drifted.contracts.contracts[0].resource_id = "drifted_resource";
    expect_runtime_drift_detected(runtime, contract_resource_drifted, "runtime match unexpectedly accepted contract resource drift");

    auto contract_quantity_drifted = loaded;
    ++contract_quantity_drifted.contracts.contracts[0].quantity;
    expect_runtime_drift_detected(runtime, contract_quantity_drifted, "runtime match unexpectedly accepted contract quantity drift");

    auto contract_reward_drifted = loaded;
    ++contract_reward_drifted.contracts.contracts[0].reward_coins;
    expect_runtime_drift_detected(runtime, contract_reward_drifted, "runtime match unexpectedly accepted contract reward drift");

    auto contract_due_day_drifted = loaded;
    ++contract_due_day_drifted.contracts.contracts[0].due_day;
    expect_runtime_drift_detected(runtime, contract_due_day_drifted, "runtime match unexpectedly accepted contract due day drift");

    auto contract_status_drifted = loaded;
    contract_status_drifted.contracts.contracts[0].status = clc::sim::ContractStatus::cancelled;
    expect_runtime_drift_detected(runtime, contract_status_drifted, "runtime match unexpectedly accepted contract status drift");

    clc::sim::SimulationRuntime failed_target{clc::sim::make_basic_runtime_scenario_registry()};
    const auto invalid_path = directory / "missing" / "runtime_validation.clcs";

    const auto failed_validation = clc::sim::validate_simulation_runtime_save_load_roundtrip(
        runtime,
        failed_target,
        invalid_path
    );

    require(!failed_validation.ok(), "invalid save path unexpectedly succeeded");
    require(!failed_validation.validation.ok(), "invalid save path did not return validation error");

    const auto malformed_path = directory / "malformed_runtime.clcs";
    {
        std::ofstream malformed_file{malformed_path};
        malformed_file << "not a city life runtime save\n";
    }

    clc::sim::SimulationRuntime malformed_target{clc::sim::make_basic_runtime_scenario_registry()};
    const auto malformed_load = clc::sim::load_simulation_runtime_from_file(malformed_path, malformed_target);

    require(!malformed_load.ok(), "malformed runtime file unexpectedly loaded");

    std::filesystem::remove_all(directory);
    return 0;
}
