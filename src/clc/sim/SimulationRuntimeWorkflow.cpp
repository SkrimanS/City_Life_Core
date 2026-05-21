#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <algorithm>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

struct DeliveryResourceAggregate final {
    std::string destination_settlement_id{};
    std::string resource_id{};
    std::uint64_t amount{0};
};

CaravanState* mutable_caravan_by_id(CaravanFleet& fleet, std::string_view caravan_id) noexcept {
    for (auto& caravan : fleet.caravans) {
        if (caravan.id == caravan_id) {
            return &caravan;
        }
    }
    return nullptr;
}

const CaravanState* caravan_by_id(const CaravanFleet& fleet, std::string_view caravan_id) noexcept {
    for (const auto& caravan : fleet.caravans) {
        if (caravan.id == caravan_id) {
            return &caravan;
        }
    }
    return nullptr;
}

bool invalid_resource_request(data::ValidationReport& report, std::string_view path, std::string_view resource_id, std::uint64_t amount) {
    bool invalid = false;
    if (resource_id.empty()) {
        report.add_error(std::string{path}, "resource_id must not be empty");
        invalid = true;
    }
    if (amount == 0) {
        report.add_error(std::string{path}, "amount must be greater than zero");
        invalid = true;
    }
    return invalid;
}

ContractFulfillmentResult missing_caravan_contract_result(std::string_view contract_id, std::string_view caravan_id) {
    ContractFulfillmentResult result;
    result.contract_id = std::string{contract_id};
    result.validation.add_error("simulation.caravan." + std::string{caravan_id}, "unknown caravan");
    return result;
}

void rollback_delivered_cargo(SimulationRuntime& runtime, CaravanState& caravan, const std::vector<RuntimeCargoDeliveryEntry>& delivered) {
    for (auto delivered_it = delivered.rbegin(); delivered_it != delivered.rend(); ++delivered_it) {
        const auto removed = runtime.engine.remove_resource_from_settlement(
            caravan.destination_settlement_id,
            delivered_it->resource_id,
            delivered_it->amount
        );
        (void)removed;

        const auto restored = caravan.cargo.add(delivered_it->resource_id, delivered_it->amount);
        (void)restored;
    }
}

void append_validation_messages(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::warning) {
            target.add_warning(message.path, message.message);
        } else {
            target.add_error(message.path, message.message);
        }
    }
}

void add_delivery_aggregate(
    std::vector<DeliveryResourceAggregate>& aggregates,
    std::string_view destination_settlement_id,
    std::string_view resource_id,
    std::uint64_t amount
) {
    for (auto& aggregate : aggregates) {
        if (aggregate.destination_settlement_id == destination_settlement_id && aggregate.resource_id == resource_id) {
            aggregate.amount += amount;
            return;
        }
    }

    aggregates.push_back(DeliveryResourceAggregate{
        .destination_settlement_id = std::string{destination_settlement_id},
        .resource_id = std::string{resource_id},
        .amount = amount,
    });
}

} // namespace

data::ValidationReport create_runtime_settlement(
    SimulationRuntime& runtime,
    std::string settlement_definition_id
) {
    return runtime.engine.create_settlement(std::move(settlement_definition_id));
}

data::ValidationReport add_runtime_route(
    SimulationRuntime& runtime,
    SettlementRoute route
) {
    auto report = validate_settlement_route_for_settlements(route, runtime.engine.settlements());
    if (!report.ok()) {
        return report;
    }
    return add_settlement_route(runtime.routes, std::move(route));
}

data::ValidationReport add_runtime_faction(
    SimulationRuntime& runtime,
    FactionState faction
) {
    return add_faction(runtime.factions, std::move(faction));
}

data::ValidationReport set_runtime_faction_reputation(
    SimulationRuntime& runtime,
    std::string from_faction_id,
    std::string to_faction_id,
    std::int64_t value
) {
    return set_faction_reputation(runtime.factions, std::move(from_faction_id), std::move(to_faction_id), value);
}

data::ValidationReport set_runtime_settlement_owner(
    SimulationRuntime& runtime,
    std::string settlement_id,
    std::string faction_id
) {
    SettlementOwnership ownership{.settlement_id = std::move(settlement_id), .faction_id = std::move(faction_id)};
    auto report = validate_settlement_ownership_references(ownership, runtime.engine.settlements(), runtime.factions);
    if (!report.ok()) {
        return report;
    }
    return set_settlement_owner(runtime.ownership, std::move(ownership.settlement_id), std::move(ownership.faction_id));
}

data::ValidationReport set_runtime_caravan_owner(
    SimulationRuntime& runtime,
    std::string caravan_id,
    std::string faction_id
) {
    CaravanOwnership ownership{.caravan_id = std::move(caravan_id), .faction_id = std::move(faction_id)};
    auto report = validate_caravan_ownership_references(ownership, runtime.caravans, runtime.factions);
    if (!report.ok()) {
        return report;
    }
    return set_caravan_owner(runtime.ownership, std::move(ownership.caravan_id), std::move(ownership.faction_id));
}

data::ValidationReport add_runtime_resource_delivery_contract(
    SimulationRuntime& runtime,
    ResourceDeliveryContract contract
) {
    auto report = validate_resource_delivery_contract_for_factions(contract, runtime.factions);
    if (!report.ok()) {
        return report;
    }
    return add_contract(runtime.contracts, std::move(contract));
}

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

data::ValidationReport load_runtime_caravan_at_origin(
    SimulationRuntime& runtime,
    std::string_view caravan_id,
    std::string_view resource_id,
    std::uint64_t amount
) {
    data::ValidationReport report;
    auto* caravan = mutable_caravan_by_id(runtime.caravans, caravan_id);
    if (caravan == nullptr) {
        report.add_error("simulation.caravan." + std::string{caravan_id}, "unknown caravan");
        return report;
    }
    if (invalid_resource_request(report, "simulation.caravan." + caravan->id + ".load", resource_id, amount)) {
        return report;
    }
    if (!runtime.engine.has_settlement(caravan->origin_settlement_id)) {
        report.add_error("simulation.settlement." + caravan->origin_settlement_id, "unknown caravan origin settlement");
        return report;
    }
    if (caravan->days_remaining != caravan->total_travel_days) {
        report.add_error("simulation.caravan." + caravan->id + ".load", "caravan can only load before departure");
        return report;
    }
    if (runtime.engine.settlement_resource_amount(caravan->origin_settlement_id, resource_id) < amount) {
        report.add_error("simulation.caravan." + caravan->id + ".load", "origin settlement does not have enough resource");
        return report;
    }

    auto remove_report = runtime.engine.remove_resource_from_settlement(caravan->origin_settlement_id, std::string{resource_id}, amount);
    if (!remove_report.ok()) {
        return remove_report;
    }

    auto cargo_report = caravan->cargo.add(std::string{resource_id}, amount);
    if (!cargo_report.ok()) {
        const auto rollback_report = runtime.engine.add_resource_to_settlement(caravan->origin_settlement_id, std::string{resource_id}, amount);
        (void)rollback_report;
        return cargo_report;
    }
    return report;
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

data::ValidationReport unload_runtime_caravan_at_destination(
    SimulationRuntime& runtime,
    std::string_view caravan_id,
    std::string_view resource_id,
    std::uint64_t amount
) {
    data::ValidationReport report;
    auto* caravan = mutable_caravan_by_id(runtime.caravans, caravan_id);
    if (caravan == nullptr) {
        report.add_error("simulation.caravan." + std::string{caravan_id}, "unknown caravan");
        return report;
    }
    if (invalid_resource_request(report, "simulation.caravan." + caravan->id + ".unload", resource_id, amount)) {
        return report;
    }
    if (!runtime.engine.has_settlement(caravan->destination_settlement_id)) {
        report.add_error("simulation.settlement." + caravan->destination_settlement_id, "unknown caravan destination settlement");
        return report;
    }
    if (!caravan_arrived(*caravan)) {
        report.add_error("simulation.caravan." + caravan->id + ".unload", "caravan can only unload after arrival");
        return report;
    }
    if (caravan->cargo.amount(resource_id) < amount) {
        report.add_error("simulation.caravan." + caravan->id + ".unload", "caravan cargo does not have enough resource");
        return report;
    }

    if (!caravan->cargo.try_remove(resource_id, amount)) {
        report.add_error("simulation.caravan." + caravan->id + ".unload", "caravan cargo does not have enough resource");
        return report;
    }

    auto destination_report = runtime.engine.add_resource_to_settlement(caravan->destination_settlement_id, std::string{resource_id}, amount);
    if (!destination_report.ok()) {
        const auto rollback_report = caravan->cargo.add(std::string{resource_id}, amount);
        (void)rollback_report;
        return destination_report;
    }
    return report;
}

RuntimeCaravanCargoDeliveryResult deliver_runtime_arrived_caravan_cargo_to_destination(
    SimulationRuntime& runtime,
    std::string_view caravan_id
) {
    RuntimeCaravanCargoDeliveryResult result{};
    result.caravan_id = std::string{caravan_id};

    auto* caravan = mutable_caravan_by_id(runtime.caravans, caravan_id);
    if (caravan == nullptr) {
        result.validation.add_error("simulation.caravan." + std::string{caravan_id}, "unknown caravan");
        return result;
    }

    result.destination_settlement_id = caravan->destination_settlement_id;

    if (!runtime.engine.has_settlement(caravan->destination_settlement_id)) {
        result.validation.add_error("simulation.settlement." + caravan->destination_settlement_id, "unknown caravan destination settlement");
        return result;
    }

    if (!caravan_arrived(*caravan)) {
        result.validation.add_error("simulation.caravan." + caravan->id + ".deliver", "caravan can only deliver cargo after arrival");
        return result;
    }

    std::vector<RuntimeCargoDeliveryEntry> entries{};
    entries.reserve(caravan->cargo.entries().size());
    for (const auto& [resource_id, amount] : caravan->cargo.entries()) {
        if (amount == 0) {
            continue;
        }
        entries.push_back(RuntimeCargoDeliveryEntry{.resource_id = resource_id, .amount = amount});
    }
    std::sort(entries.begin(), entries.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.resource_id < rhs.resource_id;
    });

    for (const auto& entry : entries) {
        if (!caravan->cargo.try_remove(entry.resource_id, entry.amount)) {
            rollback_delivered_cargo(runtime, *caravan, result.delivered);
            result.validation.add_error("simulation.caravan." + caravan->id + ".deliver", "caravan cargo changed during delivery");
            return result;
        }

        auto destination_report = runtime.engine.add_resource_to_settlement(
            caravan->destination_settlement_id,
            entry.resource_id,
            entry.amount
        );
        if (!destination_report.ok()) {
            const auto restored = caravan->cargo.add(entry.resource_id, entry.amount);
            (void)restored;
            rollback_delivered_cargo(runtime, *caravan, result.delivered);
            result.validation = destination_report;
            return result;
        }

        result.delivered.push_back(entry);
        result.total_amount += entry.amount;
    }

    return result;
}

RuntimeBulkCargoDeliveryResult deliver_all_runtime_arrived_caravan_cargo_to_destinations(
    SimulationRuntime& runtime
) {
    RuntimeBulkCargoDeliveryResult result{};
    std::vector<std::string> caravan_ids{};
    caravan_ids.reserve(runtime.caravans.caravans.size());

    for (const auto& caravan : runtime.caravans.caravans) {
        if (!caravan_arrived(caravan) || caravan.cargo.empty()) {
            continue;
        }
        if (!runtime.engine.has_settlement(caravan.destination_settlement_id)) {
            result.validation.add_error("simulation.settlement." + caravan.destination_settlement_id, "unknown caravan destination settlement");
            return result;
        }
        caravan_ids.push_back(caravan.id);
    }

    for (const auto& caravan_id : caravan_ids) {
        auto delivery = deliver_runtime_arrived_caravan_cargo_to_destination(runtime, caravan_id);
        if (!delivery.ok()) {
            result.validation = delivery.validation;
            return result;
        }

        if (delivery.total_amount > 0) {
            ++result.delivered_caravans;
            result.total_amount += delivery.total_amount;
            result.deliveries.push_back(std::move(delivery));
        }
    }

    const auto validation = validate_runtime_bulk_cargo_delivery_result_for_runtime(runtime, result);
    if (!validation.ok()) {
        result.validation = validation;
    }

    return result;
}

data::ValidationReport validate_runtime_caravan_cargo_delivery_result(
    const RuntimeCaravanCargoDeliveryResult& result
) {
    data::ValidationReport report{};

    if (!result.validation.ok()) {
        append_validation_messages(report, result.validation);
        return report;
    }

    if (result.caravan_id.empty()) {
        report.add_error("runtime.cargo_delivery.caravan_id", "caravan_id must not be empty");
    }

    if (result.destination_settlement_id.empty()) {
        report.add_error("runtime.cargo_delivery.destination_settlement_id", "destination_settlement_id must not be empty");
    }

    std::uint64_t calculated_total = 0;
    for (const auto& entry : result.delivered) {
        if (entry.resource_id.empty()) {
            report.add_error("runtime.cargo_delivery.delivered.resource_id", "resource_id must not be empty");
        }
        if (entry.amount == 0) {
            report.add_error("runtime.cargo_delivery.delivered.amount", "amount must be greater than zero");
        }
        calculated_total += entry.amount;
    }

    if (calculated_total != result.total_amount) {
        report.add_error("runtime.cargo_delivery.total_amount", "total_amount must equal delivered entry sum");
    }

    if (result.delivered.empty() && result.total_amount != 0) {
        report.add_error("runtime.cargo_delivery.total_amount", "total_amount must be zero when no resources were delivered");
    }

    return report;
}

data::ValidationReport validate_runtime_bulk_cargo_delivery_result(
    const RuntimeBulkCargoDeliveryResult& result
) {
    data::ValidationReport report{};

    if (!result.validation.ok()) {
        append_validation_messages(report, result.validation);
        return report;
    }

    std::uint64_t calculated_total = 0;
    for (const auto& delivery : result.deliveries) {
        auto delivery_report = validate_runtime_caravan_cargo_delivery_result(delivery);
        if (!delivery_report.ok()) {
            append_validation_messages(report, delivery_report);
        }
        calculated_total += delivery.total_amount;
    }

    if (calculated_total != result.total_amount) {
        report.add_error("runtime.bulk_cargo_delivery.total_amount", "total_amount must equal delivery total sum");
    }

    if (result.delivered_caravans != result.deliveries.size()) {
        report.add_error("runtime.bulk_cargo_delivery.delivered_caravans", "delivered_caravans must equal delivery count");
    }

    if (result.deliveries.empty() && result.total_amount != 0) {
        report.add_error("runtime.bulk_cargo_delivery.total_amount", "total_amount must be zero when no deliveries were recorded");
    }

    if (result.deliveries.empty() && result.delivered_caravans != 0) {
        report.add_error("runtime.bulk_cargo_delivery.delivered_caravans", "delivered_caravans must be zero when no deliveries were recorded");
    }

    return report;
}

data::ValidationReport validate_runtime_caravan_cargo_delivery_result_for_runtime(
    const SimulationRuntime& runtime,
    const RuntimeCaravanCargoDeliveryResult& result
) {
    auto report = validate_runtime_caravan_cargo_delivery_result(result);
    if (!report.ok()) {
        return report;
    }

    const auto* caravan = caravan_by_id(runtime.caravans, result.caravan_id);
    if (caravan == nullptr) {
        report.add_error("runtime.cargo_delivery.caravan_id", "caravan_id must reference an existing caravan");
        return report;
    }

    if (caravan->destination_settlement_id != result.destination_settlement_id) {
        report.add_error("runtime.cargo_delivery.destination_settlement_id", "destination_settlement_id must match caravan destination");
    }

    if (!runtime.engine.has_settlement(result.destination_settlement_id)) {
        report.add_error("runtime.cargo_delivery.destination_settlement_id", "destination_settlement_id must reference an existing settlement");
    }

    if (!caravan_arrived(*caravan)) {
        report.add_error("runtime.cargo_delivery.caravan_id", "caravan must be arrived after delivery");
    }

    if (!result.delivered.empty() && !caravan->cargo.empty()) {
        report.add_error("runtime.cargo_delivery.caravan_id", "delivered caravan cargo must be empty after delivery");
    }

    for (const auto& entry : result.delivered) {
        if (runtime.engine.settlement_resource_amount(result.destination_settlement_id, entry.resource_id) < entry.amount) {
            report.add_error("runtime.cargo_delivery.delivered.amount", "destination settlement must contain at least delivered amount");
        }
    }

    return report;
}

data::ValidationReport validate_runtime_bulk_cargo_delivery_result_for_runtime(
    const SimulationRuntime& runtime,
    const RuntimeBulkCargoDeliveryResult& result
) {
    auto report = validate_runtime_bulk_cargo_delivery_result(result);
    if (!report.ok()) {
        return report;
    }

    std::vector<DeliveryResourceAggregate> aggregates{};
    for (const auto& delivery : result.deliveries) {
        auto delivery_report = validate_runtime_caravan_cargo_delivery_result_for_runtime(runtime, delivery);
        if (!delivery_report.ok()) {
            append_validation_messages(report, delivery_report);
        }

        for (const auto& entry : delivery.delivered) {
            add_delivery_aggregate(
                aggregates,
                delivery.destination_settlement_id,
                entry.resource_id,
                entry.amount
            );
        }
    }

    for (const auto& aggregate : aggregates) {
        if (runtime.engine.settlement_resource_amount(aggregate.destination_settlement_id, aggregate.resource_id) < aggregate.amount) {
            report.add_error("runtime.bulk_cargo_delivery.delivered.amount", "destination settlement must contain at least aggregate delivered amount");
        }
    }

    return report;
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

ContractFulfillmentResult fulfill_first_runtime_contract_for_owned_arrived_caravan_with_reward_and_ledger(
    SimulationRuntime& runtime,
    std::string_view caravan_id,
    std::string_view expected_faction_id
) {
    auto* caravan = mutable_caravan_by_id(runtime.caravans, caravan_id);
    if (caravan == nullptr) {
        return missing_caravan_contract_result({}, caravan_id);
    }

    for (const auto& contract : runtime.contracts.contracts) {
        if (!contract_is_open(contract)) {
            continue;
        }
        if (caravan->cargo.amount(contract.resource_id) < contract.quantity) {
            continue;
        }

        auto result = fulfill_contract_from_owned_arrived_caravan_with_reward_and_ledger(
            runtime.contracts,
            contract.id,
            *caravan,
            runtime.ownership,
            expected_faction_id,
            runtime.wallet,
            runtime.ledger
        );
        if (result.ok()) {
            return result;
        }
    }

    ContractFulfillmentResult result;
    result.validation.add_error("simulation.contract.auto_fulfill", "no matching open contract for caravan cargo");
    return result;
}

} // namespace clc::sim
