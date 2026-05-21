#include "clc/sim/Settlement.hpp"

#include <algorithm>
#include <utility>

namespace clc::sim {
namespace {

constexpr std::uint64_t k_people_per_food_unit = 10;
constexpr std::uint64_t k_output_per_worker_per_day = 1;
constexpr std::uint64_t k_input_per_worker_per_day = 1;

std::uint64_t food_needed_for_population(std::uint64_t population) {
    return (population + k_people_per_food_unit - 1) / k_people_per_food_unit;
}

SettlementTickRemainder* remainder_by_key(SettlementState& settlement, std::string_view key) noexcept {
    for (auto& remainder : settlement.tick_remainders) {
        if (remainder.key == key) {
            return &remainder;
        }
    }
    return nullptr;
}

std::uint64_t consume_scaled_units(
    SettlementState& settlement,
    std::string key,
    std::uint64_t units_per_day,
    clc::GameTime::Tick ticks
) {
    if (units_per_day == 0 || ticks == 0) {
        return 0;
    }

    auto* remainder = remainder_by_key(settlement, key);
    if (remainder == nullptr) {
        settlement.tick_remainders.push_back(SettlementTickRemainder{.key = std::move(key), .numerator = 0});
        remainder = &settlement.tick_remainders.back();
    }

    const auto denominator = clc::ticks_per_day();
    const auto numerator = remainder->numerator + (units_per_day * ticks);
    const auto whole_units = numerator / denominator;
    remainder->numerator = numerator % denominator;
    return whole_units;
}

bool has_required_inputs(
    const ResourceStorage& storage,
    const data::BuildingDefinition& building_definition,
    std::uint64_t required_per_input
) {
    for (const auto& input_resource_id : building_definition.input_resource_ids) {
        if (required_per_input > 0 && storage.amount(input_resource_id) < required_per_input) {
            return false;
        }
    }
    return true;
}

std::uint64_t consume_inputs(
    ResourceStorage& storage,
    const data::BuildingDefinition& building_definition,
    std::uint64_t required_per_input
) {
    std::uint64_t consumed{};
    for (const auto& input_resource_id : building_definition.input_resource_ids) {
        if (required_per_input > 0 && storage.try_remove(input_resource_id, required_per_input)) {
            consumed += required_per_input;
        }
    }
    return consumed;
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

SettlementTickReport advance_settlement_ticks(
    SettlementState& settlement,
    const data::DataRegistry& registry,
    clc::GameTime::Tick ticks
) {
    SettlementTickReport report{.settlement_id = settlement.id, .elapsed_ticks = ticks};

    const auto food_needed = consume_scaled_units(
        settlement,
        "food:grain",
        food_needed_for_population(settlement.population),
        ticks
    );
    const auto consumed = settlement.storage.remove_up_to("grain", food_needed);
    report.consumed_food = consumed;

    if (consumed < food_needed) {
        report.warnings.push_back("not enough food for population");
    }

    for (const auto& building : settlement.buildings) {
        const auto* building_definition = registry.building(building.definition_id);
        if (building_definition == nullptr) {
            ++report.skipped_buildings;
            report.warnings.push_back("skipped unknown building: " + building.definition_id);
            continue;
        }

        const auto workers = std::min(building.assigned_workers, building_definition->worker_slots);
        if (workers == 0) {
            ++report.skipped_buildings;
            report.warnings.push_back("skipped building with no workers: " + building.definition_id);
            continue;
        }

        const auto input_required_per_resource = consume_scaled_units(
            settlement,
            "input:" + building.definition_id,
            static_cast<std::uint64_t>(workers) * k_input_per_worker_per_day,
            ticks
        );

        const auto produced_per_output = consume_scaled_units(
            settlement,
            "output:" + building.definition_id,
            static_cast<std::uint64_t>(workers) * k_output_per_worker_per_day,
            ticks
        );

        if (input_required_per_resource == 0 && produced_per_output == 0) {
            continue;
        }

        if (!has_required_inputs(settlement.storage, *building_definition, input_required_per_resource)) {
            ++report.skipped_buildings;
            report.warnings.push_back("skipped building due to missing inputs: " + building.definition_id);
            continue;
        }

        report.consumed_inputs += consume_inputs(settlement.storage, *building_definition, input_required_per_resource);
        ++report.active_buildings;

        for (const auto& output_resource_id : building_definition->output_resource_ids) {
            if (produced_per_output == 0) {
                continue;
            }
            const auto add_report = settlement.storage.add(output_resource_id, produced_per_output);
            if (!add_report.ok()) {
                report.warnings.push_back("failed to store produced resource: " + output_resource_id);
                continue;
            }
            report.produced_resources += produced_per_output;
        }
    }

    return report;
}

SettlementTickReport advance_settlement_day(SettlementState& settlement, const data::DataRegistry& registry) {
    return advance_settlement_ticks(settlement, registry, clc::ticks_per_day());
}

SettlementReport make_settlement_report(const SettlementState& settlement, const data::DataRegistry& registry) {
    SettlementReport report{
        .id = settlement.id,
        .display_name = settlement.display_name,
        .population = settlement.population,
    };

    report.storage.reserve(settlement.storage.entries().size());
    for (const auto& [resource_id, amount] : settlement.storage.entries()) {
        report.storage.push_back(ResourceAmount{.resource_id = resource_id, .amount = amount});
        report.total_stored_resources += amount;
    }

    std::sort(report.storage.begin(), report.storage.end(), [](const ResourceAmount& lhs, const ResourceAmount& rhs) {
        return lhs.resource_id < rhs.resource_id;
    });

    report.buildings.reserve(settlement.buildings.size());
    for (const auto& building : settlement.buildings) {
        const auto* definition = registry.building(building.definition_id);
        report.buildings.push_back(BuildingReport{
            .definition_id = building.definition_id,
            .display_name = definition == nullptr ? std::string{} : definition->display_name,
            .assigned_workers = building.assigned_workers,
            .worker_slots = definition == nullptr ? 0U : definition->worker_slots,
        });
    }

    std::sort(report.buildings.begin(), report.buildings.end(), [](const BuildingReport& lhs, const BuildingReport& rhs) {
        return lhs.definition_id < rhs.definition_id;
    });

    return report;
}

} // namespace clc::sim
