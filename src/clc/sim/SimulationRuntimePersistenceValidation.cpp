#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"

#include <string_view>

namespace clc::sim {

namespace {

void add_mismatch(data::ValidationReport& report, bool condition, const char* message) {
    if (!condition) {
        report.add_error(message);
    }
}

void append_report(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::error) {
            target.add_error(message.path, message.message);
        } else {
            target.add_warning(message.path, message.message);
        }
    }
}

void add_registry_count_mismatches(
    data::ValidationReport& report,
    const data::DataRegistry& expected,
    const data::DataRegistry& actual
) {
    add_mismatch(report, expected.resource_count() == actual.resource_count(), "runtime registry resource count mismatch");
    add_mismatch(report, expected.currency_count() == actual.currency_count(), "runtime registry currency count mismatch");
    add_mismatch(report, expected.building_count() == actual.building_count(), "runtime registry building count mismatch");
    add_mismatch(report, expected.profession_count() == actual.profession_count(), "runtime registry profession count mismatch");
    add_mismatch(report, expected.settlement_count() == actual.settlement_count(), "runtime registry settlement count mismatch");
}

void add_resource_definition_mismatch(
    data::ValidationReport& report,
    const data::DataRegistry& expected,
    const data::DataRegistry& actual,
    std::string_view resource_id
) {
    if (resource_id.empty()) {
        return;
    }

    const auto* expected_resource = expected.resource(resource_id);
    const auto* actual_resource = actual.resource(resource_id);
    add_mismatch(report, (expected_resource != nullptr) == (actual_resource != nullptr), "runtime registry resource definition presence mismatch");

    if (expected_resource == nullptr || actual_resource == nullptr) {
        return;
    }

    add_mismatch(report, expected_resource->id == actual_resource->id, "runtime registry resource id mismatch");
    add_mismatch(report, expected_resource->display_name == actual_resource->display_name, "runtime registry resource display name mismatch");
    add_mismatch(report, expected_resource->category == actual_resource->category, "runtime registry resource category mismatch");
    add_mismatch(report, expected_resource->base_value == actual_resource->base_value, "runtime registry resource base value mismatch");
}

void add_settlement_definition_mismatch(
    data::ValidationReport& report,
    const data::DataRegistry& expected,
    const data::DataRegistry& actual,
    std::string_view settlement_id
) {
    if (settlement_id.empty()) {
        return;
    }

    const auto* expected_settlement = expected.settlement(settlement_id);
    const auto* actual_settlement = actual.settlement(settlement_id);
    add_mismatch(report, (expected_settlement != nullptr) == (actual_settlement != nullptr), "runtime registry settlement definition presence mismatch");

    if (expected_settlement == nullptr || actual_settlement == nullptr) {
        return;
    }

    add_mismatch(report, expected_settlement->id == actual_settlement->id, "runtime registry settlement id mismatch");
    add_mismatch(report, expected_settlement->display_name == actual_settlement->display_name, "runtime registry settlement display name mismatch");
    add_mismatch(report, expected_settlement->starting_population == actual_settlement->starting_population, "runtime registry settlement starting population mismatch");
}

void add_building_definition_mismatch(
    data::ValidationReport& report,
    const data::DataRegistry& expected,
    const data::DataRegistry& actual,
    std::string_view building_id
) {
    if (building_id.empty()) {
        return;
    }

    const auto* expected_building = expected.building(building_id);
    const auto* actual_building = actual.building(building_id);
    add_mismatch(report, (expected_building != nullptr) == (actual_building != nullptr), "runtime registry building definition presence mismatch");

    if (expected_building == nullptr || actual_building == nullptr) {
        return;
    }

    add_mismatch(report, expected_building->id == actual_building->id, "runtime registry building id mismatch");
    add_mismatch(report, expected_building->display_name == actual_building->display_name, "runtime registry building display name mismatch");
    add_mismatch(report, expected_building->category == actual_building->category, "runtime registry building category mismatch");
    add_mismatch(report, expected_building->worker_slots == actual_building->worker_slots, "runtime registry building worker slots mismatch");
    add_mismatch(report, expected_building->required_profession_id == actual_building->required_profession_id, "runtime registry building required profession mismatch");
    add_mismatch(report, expected_building->input_resource_ids == actual_building->input_resource_ids, "runtime registry building input resources mismatch");
    add_mismatch(report, expected_building->output_resource_ids == actual_building->output_resource_ids, "runtime registry building output resources mismatch");
}

void add_registry_semantic_mismatches(
    data::ValidationReport& report,
    const data::DataRegistry& expected_registry,
    const data::DataRegistry& actual_registry,
    const SimulationWorldState& expected_state
) {
    for (const auto& settlement : expected_state.engine.settlements) {
        add_settlement_definition_mismatch(report, expected_registry, actual_registry, settlement.id);
        for (const auto& [resource_id, amount] : settlement.storage.entries()) {
            (void)amount;
            add_resource_definition_mismatch(report, expected_registry, actual_registry, resource_id);
        }
        for (const auto& building : settlement.buildings) {
            add_building_definition_mismatch(report, expected_registry, actual_registry, building.definition_id);
        }
    }

    for (const auto& demand : expected_state.engine.market_demands) {
        add_resource_definition_mismatch(report, expected_registry, actual_registry, demand.resource_id);
    }

    for (const auto& route : expected_state.routes.routes) {
        add_settlement_definition_mismatch(report, expected_registry, actual_registry, route.origin_settlement_id);
        add_settlement_definition_mismatch(report, expected_registry, actual_registry, route.destination_settlement_id);
    }

    for (const auto& caravan : expected_state.caravans.caravans) {
        add_settlement_definition_mismatch(report, expected_registry, actual_registry, caravan.origin_settlement_id);
        add_settlement_definition_mismatch(report, expected_registry, actual_registry, caravan.destination_settlement_id);
        for (const auto& [resource_id, amount] : caravan.cargo.entries()) {
            (void)amount;
            add_resource_definition_mismatch(report, expected_registry, actual_registry, resource_id);
        }
    }

    for (const auto& contract : expected_state.contracts.contracts) {
        add_resource_definition_mismatch(report, expected_registry, actual_registry, contract.resource_id);
    }

    for (const auto& entry : expected_state.ledger_entries) {
        add_resource_definition_mismatch(report, expected_registry, actual_registry, entry.resource_id);
    }
}

} // namespace

data::ValidationReport validate_simulation_runtimes_match(
    const SimulationRuntime& expected,
    const SimulationRuntime& actual
) {
    data::ValidationReport report{};

    const auto expected_state = capture_simulation_world_state(expected);
    const auto actual_state = capture_simulation_world_state(actual);

    append_report(report, validate_simulation_world_state(expected_state));
    append_report(report, validate_simulation_world_state(actual_state));

    add_mismatch(
        report,
        serialize_simulation_world_state(expected_state) == serialize_simulation_world_state(actual_state),
        "runtime world state serialization mismatch"
    );

    const auto& expected_registry = expected.engine.registry();
    const auto& actual_registry = actual.engine.registry();
    add_registry_count_mismatches(report, expected_registry, actual_registry);
    add_registry_semantic_mismatches(report, expected_registry, actual_registry, expected_state);

    return report;
}

SimulationRuntimeSaveLoadValidationResult validate_simulation_runtime_save_load_roundtrip(
    const SimulationRuntime& source,
    SimulationRuntime& target,
    const std::filesystem::path& path
) {
    SimulationRuntimeSaveLoadValidationResult result{};

    auto save_report = save_simulation_runtime_to_file(source, path);
    if (!save_report.ok()) {
        result.validation = save_report;
        return result;
    }

    result.load = load_simulation_runtime_from_file(path, target);
    if (!result.load.ok()) {
        return result;
    }

    result.validation = validate_simulation_world_state(capture_simulation_world_state(target));

    if (!result.validation.ok()) {
        return result;
    }

    result.validation = validate_simulation_runtimes_match(source, target);
    return result;
}

} // namespace clc::sim
