#include "clc/sim/Caravans.hpp"

#include <algorithm>
#include <utility>

namespace clc::sim {
namespace {

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

} // namespace

CaravanState create_caravan_for_route(
    const SettlementRoute& route,
    std::string id,
    std::string display_name,
    ResourceStorage cargo
) {
    const auto travel_ticks = settlement_route_travel_ticks(route);
    return CaravanState{
        .id = std::move(id),
        .display_name = std::move(display_name),
        .route_id = route.id,
        .origin_settlement_id = route.origin_settlement_id,
        .destination_settlement_id = route.destination_settlement_id,
        .total_travel_days = route.travel_days,
        .days_remaining = route.travel_days,
        .total_travel_ticks = travel_ticks,
        .ticks_remaining = travel_ticks,
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
    if (caravan_total_travel_ticks(caravan) == 0) {
        report.add_error("simulation.caravan." + caravan.id, "total_travel_ticks must be greater than zero");
    }
    if (caravan_ticks_remaining(caravan) > caravan_total_travel_ticks(caravan)) {
        report.add_error("simulation.caravan." + caravan.id, "ticks_remaining must not exceed total_travel_ticks");
    }
    if (caravan.total_travel_days > 0 && caravan.total_travel_ticks > 0 && caravan.total_travel_ticks != clc::days_to_ticks(caravan.total_travel_days)) {
        report.add_error("simulation.caravan." + caravan.id, "total_travel_days and total_travel_ticks must be equivalent when both are set");
    }
    if (caravan.days_remaining > 0 && caravan.ticks_remaining > 0 && caravan.ticks_remaining != clc::days_to_ticks(caravan.days_remaining)) {
        report.add_error("simulation.caravan." + caravan.id, "days_remaining and ticks_remaining must be equivalent when both are set");
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
    if (caravan_total_travel_ticks(caravan) != settlement_route_travel_ticks(route)) {
        report.add_error("simulation.caravan." + caravan.id + ".travel_ticks", "caravan travel ticks do not match route travel ticks");
    }
    return report;
}

bool caravan_arrived(const CaravanState& caravan) noexcept {
    return caravan_ticks_remaining(caravan) == 0;
}

CaravanAdvanceReport advance_caravan_ticks(CaravanState& caravan, clc::GameTime::Tick ticks) {
    const auto before_ticks = caravan_ticks_remaining(caravan);
    const auto elapsed = std::min(before_ticks, ticks);
    const auto after_ticks = before_ticks - elapsed;

    CaravanAdvanceReport report{
        .caravan_id = caravan.id,
        .route_id = caravan.route_id,
        .days_remaining_before = before_ticks / clc::ticks_per_day(),
        .days_remaining_after = after_ticks / clc::ticks_per_day(),
        .ticks_remaining_before = before_ticks,
        .ticks_remaining_after = after_ticks,
        .ticks_elapsed = elapsed,
        .moved = elapsed > 0,
        .arrived = after_ticks == 0,
    };

    caravan.ticks_remaining = after_ticks;
    caravan.days_remaining = after_ticks / clc::ticks_per_day();
    return report;
}

CaravanAdvanceReport advance_caravan_day(CaravanState& caravan) {
    return advance_caravan_ticks(caravan, clc::ticks_per_day());
}

data::ValidationReport load_caravan_at_origin(
    CaravanState& caravan,
    SettlementState& origin,
    std::string_view resource_id,
    std::uint64_t amount
) {
    data::ValidationReport report;
    if (invalid_resource_request(report, "simulation.caravan." + caravan.id + ".load", resource_id, amount)) {
        return report;
    }
    if (origin.id != caravan.origin_settlement_id) {
        report.add_error("simulation.caravan." + caravan.id + ".origin", "caravan can only load at its origin settlement");
        return report;
    }
    if (caravan_ticks_remaining(caravan) != caravan_total_travel_ticks(caravan)) {
        report.add_error("simulation.caravan." + caravan.id + ".load", "caravan can only load before departure");
        return report;
    }
    if (!origin.storage.try_remove(resource_id, amount)) {
        report.add_error("simulation.caravan." + caravan.id + ".load", "origin settlement does not have enough resource");
        return report;
    }

    auto cargo_report = caravan.cargo.add(std::string{resource_id}, amount);
    if (!cargo_report.ok()) {
        const auto rollback_report = origin.storage.add(std::string{resource_id}, amount);
        (void)rollback_report;
        return cargo_report;
    }
    return report;
}

data::ValidationReport unload_caravan_at_destination(
    CaravanState& caravan,
    SettlementState& destination,
    std::string_view resource_id,
    std::uint64_t amount
) {
    data::ValidationReport report;
    if (invalid_resource_request(report, "simulation.caravan." + caravan.id + ".unload", resource_id, amount)) {
        return report;
    }
    if (destination.id != caravan.destination_settlement_id) {
        report.add_error("simulation.caravan." + caravan.id + ".destination", "caravan can only unload at its destination settlement");
        return report;
    }
    if (!caravan_arrived(caravan)) {
        report.add_error("simulation.caravan." + caravan.id + ".unload", "caravan can only unload after arrival");
        return report;
    }
    if (!caravan.cargo.try_remove(resource_id, amount)) {
        report.add_error("simulation.caravan." + caravan.id + ".unload", "caravan cargo does not have enough resource");
        return report;
    }

    auto destination_report = destination.storage.add(std::string{resource_id}, amount);
    if (!destination_report.ok()) {
        const auto rollback_report = caravan.cargo.add(std::string{resource_id}, amount);
        (void)rollback_report;
        return destination_report;
    }
    return report;
}

data::ValidationReport add_caravan(CaravanFleet& fleet, CaravanState caravan) {
    if (caravan.total_travel_ticks == 0 && caravan.total_travel_days > 0) {
        caravan.total_travel_ticks = clc::days_to_ticks(caravan.total_travel_days);
    }
    if (caravan.ticks_remaining == 0 && caravan.days_remaining > 0) {
        caravan.ticks_remaining = clc::days_to_ticks(caravan.days_remaining);
    }

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

clc::GameTime::Tick caravan_total_travel_ticks(const CaravanState& caravan) noexcept {
    if (caravan.total_travel_ticks > 0) {
        return caravan.total_travel_ticks;
    }
    return clc::days_to_ticks(caravan.total_travel_days);
}

clc::GameTime::Tick caravan_ticks_remaining(const CaravanState& caravan) noexcept {
    if (caravan.ticks_remaining > 0 || caravan.days_remaining == 0) {
        return caravan.ticks_remaining;
    }
    return clc::days_to_ticks(caravan.days_remaining);
}

} // namespace clc::sim
