#include "clc/sim/Settlement.hpp"

#include <algorithm>
#include <utility>

namespace clc::sim {
namespace {

constexpr std::uint64_t k_people_per_food_unit = 10;
constexpr std::uint64_t k_output_per_worker_per_day = 1;

std::uint64_t food_needed_for_population(std::uint64_t population) {
    return (population + k_people_per_food_unit - 1) / k_people_per_food_unit;
}

} // namespace

SettlementState create_settlement_from_definition(const data::SettlementDefinition& definition) {
    return SettlementState{
        .id = definition.id,
        .display_name = definition.display_name,
        .population = definition.starting_population,
    };
}

data::ValidationReport add_building(SettlementState& settlement, const data::DataRegistry& registry, BuildingInstance building) {
    data::ValidationReport report;
    const auto* building_definition = registry.building(building.definition_id);
    if (building_definition == nullptr) {
        report.add_error("settlement." + settlement.id, "unknown building definition: " + building.definition_id);
        return report;
    }

    if (building.assigned_workers > building_definition->worker_slots) {
        report.add_error("settlement." + settlement.id + ".building." + building.definition_id, "assigned_workers exceeds worker_slots");
        return report;
    }

    settlement.buildings.push_back(std::move(building));
    return report;
}

SettlementTickReport advance_settlement_day(SettlementState& settlement, const data::DataRegistry& registry) {
    SettlementTickReport report{.settlement_id = settlement.id};

    const auto food_needed = food_needed_for_population(settlement.population);
    const auto consumed = settlement.storage.remove_up_to("grain", food_needed);
    report.consumed_food = consumed;

    if (consumed < food_needed) {
        report.warnings.push_back("not enough food for population");
    }

    for (const auto& building : settlement.buildings) {
        const auto* building_definition = registry.building(building.definition_id);
        if (building_definition == nullptr) {
            report.warnings.push_back("skipped unknown building: " + building.definition_id);
            continue;
        }

        const auto workers = std::min(building.assigned_workers, building_definition->worker_slots);
        for (const auto& output_resource_id : building_definition->output_resource_ids) {
            const auto produced = static_cast<std::uint64_t>(workers) * k_output_per_worker_per_day;
            const auto add_report = settlement.storage.add(output_resource_id, produced);
            if (!add_report.ok()) {
                report.warnings.push_back("failed to store produced resource: " + output_resource_id);
                continue;
            }
            report.produced_resources += produced;
        }
    }

    return report;
}

} // namespace clc::sim
