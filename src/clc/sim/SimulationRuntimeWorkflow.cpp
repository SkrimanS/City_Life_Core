#include "clc/sim/SimulationRuntimeWorkflow.hpp"

namespace clc::sim {
namespace {

CaravanState* mutable_caravan_by_id(CaravanFleet& fleet, std::string_view caravan_id) noexcept {
    for (auto& caravan : fleet.caravans) {
        if (caravan.id == caravan_id) {
            return &caravan;
        }
    }
    return nullptr;
}

ContractFulfillmentResult missing_caravan_contract_result(std::string_view contract_id, std::string_view caravan_id) {
    ContractFulfillmentResult result;
    result.contract_id = std::string{contract_id};
    result.validation.add_error("simulation.caravan." + std::string{caravan_id}, "unknown caravan");
    return result;
}

} // namespace

RuntimeCaravanCreationResult create_runtime_caravan_for_route(
    SimulationRuntime& runtime,
    std::string_view route_id,
    std::string caravan_id,
    std::string display_name,
    ResourceStorage cargo
) {
    RuntimeCaravanCreationResult result;
    result.caravan_id = caravan_id;

    const auto* route = settlement_route_by_id(runtime.routes, route_id);
    if (route == nullptr) {
        result.validation.add_error("simulation.route." + std::string{route_id}, "unknown route");
        return result;
    }

    auto caravan = create_caravan_for_route(*route, std::move(caravan_id), std::move(display_name), std::move(cargo));
    auto report = add_caravan(runtime.caravans, std::move(caravan));
    if (!report.ok()) {
        result.validation = report;
        return result;
    }

    result.created = true;
    return result;
}

RuntimeCaravanAdvanceResult advance_runtime_caravan_day(
    SimulationRuntime& runtime,
    std::string_view caravan_id
) {
    RuntimeCaravanAdvanceResult result;
    auto* caravan = mutable_caravan_by_id(runtime.caravans, caravan_id);
    if (caravan == nullptr) {
        result.validation.add_error("simulation.caravan." + std::string{caravan_id}, "unknown caravan");
        return result;
    }

    result.report = advance_caravan_day(*caravan);
    return result;
}

ContractFulfillmentResult fulfill_runtime_contract_from_arrived_caravan_with_reward_and_ledger(
    SimulationRuntime& runtime,
    std::string_view contract_id,
    std::string_view caravan_id
) {
    auto* caravan = mutable_caravan_by_id(runtime.caravans, caravan_id);
    if (caravan == nullptr) {
        return missing_caravan_contract_result(contract_id, caravan_id);
    }

    return fulfill_contract_from_arrived_caravan_with_reward_and_ledger(
        runtime.contracts,
        contract_id,
        *caravan,
        runtime.wallet,
        runtime.ledger
    );
}

ContractFulfillmentResult fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
    SimulationRuntime& runtime,
    std::string_view contract_id,
    std::string_view caravan_id,
    std::string_view expected_faction_id
) {
    auto* caravan = mutable_caravan_by_id(runtime.caravans, caravan_id);
    if (caravan == nullptr) {
        return missing_caravan_contract_result(contract_id, caravan_id);
    }

    return fulfill_contract_from_owned_arrived_caravan_with_reward_and_ledger(
        runtime.contracts,
        contract_id,
        *caravan,
        runtime.ownership,
        expected_faction_id,
        runtime.wallet,
        runtime.ledger
    );
}

} // namespace clc::sim
