#include "clc/sim/SimulationRuntimeScenario.hpp"

namespace clc::sim {

namespace {

void merge_report(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::error) {
            target.add_error(message.path, message.message);
        } else {
            target.add_warning(message.path, message.message);
        }
    }
}

} // namespace

data::DataRegistry make_basic_runtime_scenario_registry() {
    data::DataRegistry registry;

    auto report = registry.add(data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    });
    (void)report;

    report = registry.add(data::SettlementDefinition{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .starting_population = 120,
    });
    (void)report;

    report = registry.add(data::SettlementDefinition{
        .id = "hillford",
        .display_name = "Hillford",
        .starting_population = 80,
    });
    (void)report;

    return registry;
}

RuntimeScenarioBootstrapResult make_basic_runtime_scenario(
    RuntimeScenarioBootstrapConfig config
) {
    RuntimeScenarioBootstrapResult result{make_basic_runtime_scenario_registry()};

    merge_report(result.validation, create_runtime_settlement(result.runtime, config.origin_settlement_id));
    merge_report(result.validation, create_runtime_settlement(result.runtime, config.destination_settlement_id));

    merge_report(result.validation, add_runtime_route(result.runtime, SettlementRoute{
        .id = config.route_id,
        .display_name = "Runtime Trade Route",
        .origin_settlement_id = config.origin_settlement_id,
        .destination_settlement_id = config.destination_settlement_id,
        .travel_days = config.route_travel_days,
    }));

    merge_report(result.validation, add_runtime_faction(result.runtime, FactionState{
        .id = config.origin_faction_id,
        .display_name = "Origin Faction",
    }));

    merge_report(result.validation, add_runtime_faction(result.runtime, FactionState{
        .id = config.receiver_faction_id,
        .display_name = "Receiver Faction",
    }));

    merge_report(result.validation, set_runtime_faction_reputation(
        result.runtime,
        config.origin_faction_id,
        config.receiver_faction_id,
        10
    ));

    merge_report(result.validation, set_runtime_settlement_owner(
        result.runtime,
        config.origin_settlement_id,
        config.origin_faction_id
    ));

    merge_report(result.validation, add_runtime_resource_delivery_contract(result.runtime, ResourceDeliveryContract{
        .id = config.contract_id,
        .display_name = "Runtime Grain Delivery",
        .issuer_faction_id = config.origin_faction_id,
        .receiver_faction_id = config.receiver_faction_id,
        .resource_id = config.resource_id,
        .quantity = config.contract_quantity,
        .reward_coins = config.contract_reward_coins,
        .due_day = config.contract_due_day,
    }));

    merge_report(result.validation, result.runtime.engine.add_resource_to_settlement(
        config.origin_settlement_id,
        config.resource_id,
        config.origin_starting_resource
    ));

    result.runtime.wallet.coins = config.wallet_coins;
    return result;
}

} // namespace clc::sim
