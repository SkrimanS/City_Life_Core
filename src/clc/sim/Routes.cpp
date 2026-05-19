#include "clc/sim/Routes.hpp"

#include <utility>

namespace clc::sim {
namespace {

bool settlement_exists(const std::vector<SettlementState>& settlements, std::string_view settlement_id) {
    for (const auto& settlement : settlements) {
        if (settlement.id == settlement_id) {
            return true;
        }
    }
    return false;
}

} // namespace

data::ValidationReport validate_settlement_route(const SettlementRoute& route) {
    data::ValidationReport report;
    if (route.id.empty()) {
        report.add_error("simulation.route", "route id must not be empty");
    }
    if (route.display_name.empty()) {
        report.add_error("simulation.route." + route.id, "route display_name must not be empty");
    }
    if (route.origin_settlement_id.empty()) {
        report.add_error("simulation.route." + route.id, "origin_settlement_id must not be empty");
    }
    if (route.destination_settlement_id.empty()) {
        report.add_error("simulation.route." + route.id, "destination_settlement_id must not be empty");
    }
    if (!route.origin_settlement_id.empty()
        && !route.destination_settlement_id.empty()
        && route.origin_settlement_id == route.destination_settlement_id) {
        report.add_error("simulation.route." + route.id, "origin and destination settlements must be different");
    }
    if (route.travel_days == 0) {
        report.add_error("simulation.route." + route.id, "travel_days must be greater than zero");
    }
    return report;
}

data::ValidationReport validate_settlement_route_for_settlements(
    const SettlementRoute& route,
    const std::vector<SettlementState>& settlements
) {
    auto report = validate_settlement_route(route);
    if (!report.ok()) {
        return report;
    }

    if (!settlement_exists(settlements, route.origin_settlement_id)) {
        report.add_error("simulation.route." + route.id + ".origin", "unknown origin settlement");
    }
    if (!settlement_exists(settlements, route.destination_settlement_id)) {
        report.add_error("simulation.route." + route.id + ".destination", "unknown destination settlement");
    }
    return report;
}

data::ValidationReport add_settlement_route(SettlementRouteCatalog& catalog, SettlementRoute route) {
    auto report = validate_settlement_route(route);
    if (!report.ok()) {
        return report;
    }

    if (settlement_route_by_id(catalog, route.id) != nullptr) {
        report.add_error("simulation.route." + route.id, "duplicate route id");
        return report;
    }

    catalog.routes.push_back(std::move(route));
    return report;
}

std::uint64_t settlement_route_count(const SettlementRouteCatalog& catalog) noexcept {
    return catalog.routes.size();
}

const SettlementRoute* settlement_route_by_id(const SettlementRouteCatalog& catalog, std::string_view route_id) noexcept {
    for (const auto& route : catalog.routes) {
        if (route.id == route_id) {
            return &route;
        }
    }
    return nullptr;
}

std::vector<SettlementRoute> settlement_routes_from(const SettlementRouteCatalog& catalog, std::string_view settlement_id) {
    std::vector<SettlementRoute> routes;
    for (const auto& route : catalog.routes) {
        if (route.origin_settlement_id == settlement_id) {
            routes.push_back(route);
        }
    }
    return routes;
}

std::vector<SettlementRoute> settlement_routes_to(const SettlementRouteCatalog& catalog, std::string_view settlement_id) {
    std::vector<SettlementRoute> routes;
    for (const auto& route : catalog.routes) {
        if (route.destination_settlement_id == settlement_id) {
            routes.push_back(route);
        }
    }
    return routes;
}

} // namespace clc::sim
