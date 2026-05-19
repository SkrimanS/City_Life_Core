#include "clc/sim/Routes.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::SettlementRoute make_route() {
    return clc::sim::SettlementRoute{
        .id = "riverwatch_to_hillford",
        .display_name = "Riverwatch to Hillford",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "hillford",
        .travel_days = 3,
    };
}

std::vector<clc::sim::SettlementState> make_settlements() {
    return {
        clc::sim::SettlementState{.id = "riverwatch", .display_name = "Riverwatch", .population = 120},
        clc::sim::SettlementState{.id = "hillford", .display_name = "Hillford", .population = 80},
        clc::sim::SettlementState{.id = "stonegate", .display_name = "Stonegate", .population = 60},
    };
}

} // namespace

int main() {
    const auto valid_route = make_route();
    require(clc::sim::validate_settlement_route(valid_route).ok(), "valid route should pass validation");
    require(clc::sim::validate_settlement_route_for_settlements(valid_route, make_settlements()).ok(), "valid route should resolve settlements");

    const clc::sim::SettlementRoute empty_route;
    const auto empty_report = clc::sim::validate_settlement_route(empty_route);
    require(!empty_report.ok(), "empty route should fail validation");
    require(empty_report.error_count() == 5, "empty route should report id, name, origin, destination, and travel days errors");

    auto loop_route = valid_route;
    loop_route.destination_settlement_id = loop_route.origin_settlement_id;
    require(!clc::sim::validate_settlement_route(loop_route).ok(), "route should reject same origin and destination");

    auto unknown_route = valid_route;
    unknown_route.destination_settlement_id = "missing";
    const auto unknown_report = clc::sim::validate_settlement_route_for_settlements(unknown_route, make_settlements());
    require(!unknown_report.ok(), "route should reject unknown destination settlement");
    require(unknown_report.error_count() == 1, "unknown destination should report one error");

    clc::sim::SettlementRouteCatalog catalog;
    require(clc::sim::settlement_route_count(catalog) == 0, "empty route catalog should count zero");
    require(clc::sim::settlement_route_by_id(catalog, "riverwatch_to_hillford") == nullptr, "empty route catalog lookup should miss");
    require(clc::sim::add_settlement_route(catalog, valid_route).ok(), "valid route should add to catalog");
    require(clc::sim::settlement_route_count(catalog) == 1, "route catalog should count inserted route");

    const auto* found_route = clc::sim::settlement_route_by_id(catalog, "riverwatch_to_hillford");
    require(found_route != nullptr, "route catalog should find inserted route");
    require(found_route->origin_settlement_id == "riverwatch", "route origin should be preserved");
    require(found_route->destination_settlement_id == "hillford", "route destination should be preserved");
    require(found_route->travel_days == 3, "route travel days should be preserved");

    require(!clc::sim::add_settlement_route(catalog, valid_route).ok(), "duplicate route id should be rejected");
    require(clc::sim::settlement_route_count(catalog) == 1, "duplicate route should not change catalog size");
    require(!clc::sim::add_settlement_route(catalog, empty_route).ok(), "invalid route should be rejected");
    require(clc::sim::settlement_route_count(catalog) == 1, "invalid route should not change catalog size");

    require(clc::sim::add_settlement_route(catalog, clc::sim::SettlementRoute{
        .id = "stonegate_to_riverwatch",
        .display_name = "Stonegate to Riverwatch",
        .origin_settlement_id = "stonegate",
        .destination_settlement_id = "riverwatch",
        .travel_days = 5,
    }).ok(), "second route should add to catalog");

    const auto from_riverwatch = clc::sim::settlement_routes_from(catalog, "riverwatch");
    require(from_riverwatch.size() == 1, "routes_from should return one route from riverwatch");
    require(from_riverwatch[0].id == "riverwatch_to_hillford", "routes_from should return expected route");

    const auto to_riverwatch = clc::sim::settlement_routes_to(catalog, "riverwatch");
    require(to_riverwatch.size() == 1, "routes_to should return one route to riverwatch");
    require(to_riverwatch[0].id == "stonegate_to_riverwatch", "routes_to should return expected route");

    require(clc::sim::settlement_routes_from(catalog, "missing").empty(), "routes_from should return empty for missing settlement id");
    require(clc::sim::settlement_routes_to(catalog, "missing").empty(), "routes_to should return empty for missing settlement id");

    return 0;
}
