#include "clc/sim/Caravans.hpp"

#include <utility>

namespace clc::sim {

CaravanState create_caravan_for_route(
    const SettlementRoute& route,
    std::string id,
    std::string display_name,
    ResourceStorage cargo
) {
    return CaravanState{
        .id = std::move(id),
        .display_name = std::move(display_name),
        .route_id = route.id,
        .origin_settlement_id = route.origin_settlement_id,
        .destination_settlement_id = route.destination_settlement_id,
        .total_travel_days = route.travel_days,
        .days_remaining = route.travel_days,
        .cargo = std::move(cargo),
    };
}

data::ValidationReport validate_caravan(const CaravanState& caravan) {
    data::ValidationReport report;
    if (caravan.id.empty()) {
        report.add_error("simulation.caravan", "caravan id must not be empty");
    }
    if (caravan.display_name.empty()) {
        report.add_error("simulation.caravan." + caravan.id, "caravan display_name must not be empty");
    }
    if (caravan.route_id.empty()) {
        report.add_error("simulation.caravan." + caravan.id, "route_id must not be empty");
    }
    if (caravan.origin_settlement_id.empty()) {
        report.add_error("simulation.caravan." + caravan.id, "origin_settlement_id must not be empty");
    }
    if (caravan.destination_settlement_id.empty()) {
        report.add_error("simulation.caravan." + caravan.id, "destination_settlement_id must not be empty");
    }
    if (!caravan.origin_settlement_id.empty()
        && !caravan.destination_settlement_id.empty()
        && caravan.origin_settlement_id == caravan.destination_settlement_id) {
        report.add_error("simulation.caravan." + caravan.id, "origin and destination settlements must be different");
    }
    if (caravan.total_travel_days == 0) {
        report.add_error("simulation.caravan." + caravan.id, "total_travel_days must be greater than zero");
    }
    if (caravan.days_remaining > caravan.total_travel_days) {
        report.add_error("simulation.caravan." + caravan.id, "days_remaining must not exceed total_travel_days");
    }
    return report;
}

data::ValidationReport validate_caravan_for_route(const CaravanState& caravan, const SettlementRoute& route) {
    auto report = validate_caravan(caravan);
    if (!report.ok()) {
        return report;
    }

    if (caravan.route_id != route.id) {
        report.add_error("simulation.caravan." + caravan.id + ".route", "caravan route_id does not match route");
    }
    if (caravan.origin_settlement_id != route.origin_settlement_id) {
        report.add_error("simulation.caravan." + caravan.id + ".origin", "caravan origin does not match route origin");
    }
    if (caravan.destination_settlement_id != route.destination_settlement_id) {
        report.add_error("simulation.caravan." + caravan.id + ".destination", "caravan destination does not match route destination");
    }
    if (caravan.total_travel_days != route.travel_days) {
        report.add_error("simulation.caravan." + caravan.id + ".travel_days", "caravan travel days do not match route travel days");
    }
    return report;
}

bool caravan_arrived(const CaravanState& caravan) noexcept {
    return caravan.days_remaining == 0;
}

CaravanAdvanceReport advance_caravan_day(CaravanState& caravan) {
    CaravanAdvanceReport report{
        .caravan_id = caravan.id,
        .route_id = caravan.route_id,
        .days_remaining_before = caravan.days_remaining,
        .days_remaining_after = caravan.days_remaining,
        .moved = false,
        .arrived = caravan_arrived(caravan),
    };

    if (caravan.days_remaining == 0) {
        return report;
    }

    --caravan.days_remaining;
    report.days_remaining_after = caravan.days_remaining;
    report.moved = true;
    report.arrived = caravan_arrived(caravan);
    return report;
}

data::ValidationReport add_caravan(CaravanFleet& fleet, CaravanState caravan) {
    auto report = validate_caravan(caravan);
    if (!report.ok()) {
        return report;
    }

    if (caravan_by_id(fleet, caravan.id) != nullptr) {
        report.add_error("simulation.caravan." + caravan.id, "duplicate caravan id");
        return report;
    }

    fleet.caravans.push_back(std::move(caravan));
    return report;
}

std::uint64_t caravan_count(const CaravanFleet& fleet) noexcept {
    return fleet.caravans.size();
}

const CaravanState* caravan_by_id(const CaravanFleet& fleet, std::string_view caravan_id) noexcept {
    for (const auto& caravan : fleet.caravans) {
        if (caravan.id == caravan_id) {
            return &caravan;
        }
    }
    return nullptr;
}

std::vector<CaravanState> active_caravans(const CaravanFleet& fleet) {
    std::vector<CaravanState> caravans;
    for (const auto& caravan : fleet.caravans) {
        if (!caravan_arrived(caravan)) {
            caravans.push_back(caravan);
        }
    }
    return caravans;
}

std::vector<CaravanState> arrived_caravans(const CaravanFleet& fleet) {
    std::vector<CaravanState> caravans;
    for (const auto& caravan : fleet.caravans) {
        if (caravan_arrived(caravan)) {
            caravans.push_back(caravan);
        }
    }
    return caravans;
}

} // namespace clc::sim
