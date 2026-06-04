#include "clc/sim/Production.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

std::uint64_t saturating_add(std::uint64_t lhs, std::uint64_t rhs) noexcept {
    if (rhs > std::numeric_limits<std::uint64_t>::max() - lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs + rhs;
}

ProductionResourceSignal make_resource_signal(
    const SettlementState& settlement,
    const economy::MarketSnapshot* market_snapshot,
    std::string building_definition_id,
    std::string resource_id,
    ProductionResourceRole role
) {
    ProductionResourceSignal signal{
        .building_definition_id = std::move(building_definition_id),
        .resource_id = std::move(resource_id),
        .role = role,
    };

    signal.stored_amount = settlement.storage.amount(signal.resource_id);
    signal.available_locally = signal.stored_amount > 0;

    if (market_snapshot != nullptr) {
        if (const auto* market_signal = economy::market_signal_by_resource(*market_snapshot, signal.resource_id)) {
            signal.market_supply = market_signal->supply;
            signal.market_demand = market_signal->demand;
            signal.market_pressure_basis_points = market_signal->pressure_basis_points;
            signal.market_pressure = market_signal->pressure;
            signal.market_shortage = market_signal->pressure == economy::MarketPressureLevel::shortage
                || market_signal->pressure == economy::MarketPressureLevel::depleted;
        }
    }

    return signal;
}

void sort_building_resources(BuildingProductionSignal& building) {
    std::sort(building.resources.begin(), building.resources.end(), [](const ProductionResourceSignal& lhs, const ProductionResourceSignal& rhs) {
        if (lhs.resource_id != rhs.resource_id) {
            return lhs.resource_id < rhs.resource_id;
        }
        return static_cast<int>(lhs.role) < static_cast<int>(rhs.role);
    });
}

} // namespace

std::string_view production_resource_role_name(ProductionResourceRole role) noexcept {
    switch (role) {
    case ProductionResourceRole::input:
        return "input";
    case ProductionResourceRole::output:
        return "output";
    }
    return "unknown";
}

SettlementProductionSnapshot make_settlement_production_snapshot(
    const SettlementState& settlement,
    const data::DataRegistry& registry
) {
    return make_settlement_production_snapshot_with_market(settlement, registry, economy::MarketSnapshot{});
}

SettlementProductionSnapshot make_settlement_production_snapshot_with_market(
    const SettlementState& settlement,
    const data::DataRegistry& registry,
    const economy::MarketSnapshot& market_snapshot
) {
    const auto* market_ptr = market_snapshot.signals.empty() ? nullptr : &market_snapshot;
    SettlementProductionSnapshot snapshot{
        .settlement_id = settlement.id,
        .display_name = settlement.display_name,
        .population = settlement.population,
        .building_count = settlement.buildings.size(),
    };

    for (const auto& [resource_id, amount] : settlement.storage.entries()) {
        (void)resource_id;
        snapshot.total_stored_resources = saturating_add(snapshot.total_stored_resources, amount);
    }

    std::uint64_t highest_output_pressure{};

    for (const auto& building_instance : settlement.buildings) {
        BuildingProductionSignal building{
            .definition_id = building_instance.definition_id,
            .assigned_workers = building_instance.assigned_workers,
        };

        const auto* definition = registry.building(building_instance.definition_id);
        if (definition == nullptr) {
            ++snapshot.unknown_building_count;
            ++snapshot.blocked_building_count;
            building.reason = "unknown building definition";
            snapshot.buildings.push_back(std::move(building));
            continue;
        }

        building.display_name = definition->display_name;
        building.worker_slots = definition->worker_slots;
        building.known_definition = true;
        building.has_workers = building.assigned_workers > 0;
        building.idle_worker_slots = building.assigned_workers >= building.worker_slots ? 0 : building.worker_slots - building.assigned_workers;

        snapshot.total_worker_slots = saturating_add(snapshot.total_worker_slots, building.worker_slots);
        snapshot.assigned_workers = saturating_add(snapshot.assigned_workers, building.assigned_workers);
        snapshot.idle_worker_slots = saturating_add(snapshot.idle_worker_slots, building.idle_worker_slots);

        if (!building.has_workers) {
            building.reason = "no assigned workers";
        }

        for (const auto& resource_id : definition->input_resource_ids) {
            auto signal = make_resource_signal(settlement, market_ptr, definition->id, resource_id, ProductionResourceRole::input);
            if (!signal.available_locally) {
                building.has_required_inputs = false;
                ++snapshot.missing_input_count;
            }
            building.resources.push_back(std::move(signal));
        }

        for (const auto& resource_id : definition->output_resource_ids) {
            auto signal = make_resource_signal(settlement, market_ptr, definition->id, resource_id, ProductionResourceRole::output);
            if (signal.market_shortage) {
                building.output_has_market_pressure = true;
                ++snapshot.pressured_output_count;
                if (signal.market_pressure_basis_points > highest_output_pressure) {
                    highest_output_pressure = signal.market_pressure_basis_points;
                    snapshot.highest_pressure_output_resource_id = signal.resource_id;
                }
            }
            building.resources.push_back(std::move(signal));
        }

        if (!building.has_required_inputs && building.reason.empty()) {
            building.reason = "missing input resources";
        }
        if (building.has_workers && building.has_required_inputs) {
            ++snapshot.active_building_count;
            if (building.reason.empty()) {
                building.reason = "active";
            }
        } else {
            ++snapshot.blocked_building_count;
        }

        sort_building_resources(building);
        snapshot.buildings.push_back(std::move(building));
    }

    std::sort(snapshot.buildings.begin(), snapshot.buildings.end(), [](const BuildingProductionSignal& lhs, const BuildingProductionSignal& rhs) {
        return lhs.definition_id < rhs.definition_id;
    });

    return snapshot;
}

const BuildingProductionSignal* production_signal_by_building(
    const SettlementProductionSnapshot& snapshot,
    std::string_view building_definition_id
) noexcept {
    for (const auto& building : snapshot.buildings) {
        if (building.definition_id == building_definition_id) {
            return &building;
        }
    }
    return nullptr;
}

const ProductionResourceSignal* production_resource_signal(
    const BuildingProductionSignal& building,
    std::string_view resource_id,
    ProductionResourceRole role
) noexcept {
    for (const auto& resource : building.resources) {
        if (resource.resource_id == resource_id && resource.role == role) {
            return &resource;
        }
    }
    return nullptr;
}

std::string settlement_production_snapshot_digest(const SettlementProductionSnapshot& snapshot) {
    std::ostringstream out;
    out << "settlement_production"
        << ";settlement=" << snapshot.settlement_id
        << ";buildings=" << snapshot.building_count
        << ";active=" << snapshot.active_building_count
        << ";blocked=" << snapshot.blocked_building_count
        << ";unknown=" << snapshot.unknown_building_count
        << ";worker_slots=" << snapshot.total_worker_slots
        << ";assigned_workers=" << snapshot.assigned_workers
        << ";idle_worker_slots=" << snapshot.idle_worker_slots
        << ";missing_inputs=" << snapshot.missing_input_count
        << ";pressured_outputs=" << snapshot.pressured_output_count;
    if (!snapshot.highest_pressure_output_resource_id.empty()) {
        out << ";highest_pressure_output=" << snapshot.highest_pressure_output_resource_id;
    }
    return out.str();
}

} // namespace clc::sim
