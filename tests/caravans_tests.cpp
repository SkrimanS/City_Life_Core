#include "clc/sim/Caravans.hpp"

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

} // namespace

int main() {
    auto cargo = clc::sim::ResourceStorage{};
    require(cargo.add("grain", 20).ok(), "cargo should accept grain");
    require(cargo.add("wood", 5).ok(), "cargo should accept wood");

    const auto route = make_route();
    auto caravan = clc::sim::create_caravan_for_route(route, "caravan_001", "First Caravan", cargo);
    require(clc::sim::validate_caravan(caravan).ok(), "created caravan should pass validation");
    require(clc::sim::validate_caravan_for_route(caravan, route).ok(), "created caravan should match source route");
    require(caravan.id == "caravan_001", "caravan id should be preserved");
    require(caravan.route_id == route.id, "caravan route id should come from route");
    require(caravan.origin_settlement_id == route.origin_settlement_id, "caravan origin should come from route");
    require(caravan.destination_settlement_id == route.destination_settlement_id, "caravan destination should come from route");
    require(caravan.total_travel_days == 3, "caravan total travel days should come from route");
    require(caravan.days_remaining == 3, "new caravan should start with full travel days remaining");
    require(caravan.cargo.amount("grain") == 20, "caravan cargo grain should be preserved");
    require(caravan.cargo.amount("wood") == 5, "caravan cargo wood should be preserved");
    require(!clc::sim::caravan_arrived(caravan), "new caravan should not be arrived");

    const auto first_day = clc::sim::advance_caravan_day(caravan);
    require(first_day.caravan_id == "caravan_001", "advance report should include caravan id");
    require(first_day.route_id == route.id, "advance report should include route id");
    require(first_day.days_remaining_before == 3, "first day should start with 3 days remaining");
    require(first_day.days_remaining_after == 2, "first day should reduce remaining days");
    require(first_day.moved, "first day should move caravan");
    require(!first_day.arrived, "first day should not arrive");

    require(clc::sim::advance_caravan_day(caravan).days_remaining_after == 1, "second day should reduce to one day");
    const auto final_day = clc::sim::advance_caravan_day(caravan);
    require(final_day.days_remaining_before == 1, "final day should start with one day remaining");
    require(final_day.days_remaining_after == 0, "final day should arrive");
    require(final_day.moved, "final day should move caravan");
    require(final_day.arrived, "final day should mark arrival");
    require(clc::sim::caravan_arrived(caravan), "caravan should be arrived after travel days elapsed");

    const auto after_arrival = clc::sim::advance_caravan_day(caravan);
    require(after_arrival.days_remaining_before == 0, "arrived caravan should stay at zero before advance");
    require(after_arrival.days_remaining_after == 0, "arrived caravan should stay at zero after advance");
    require(!after_arrival.moved, "arrived caravan should not move again");
    require(after_arrival.arrived, "arrived caravan should remain arrived");

    clc::sim::CaravanFleet fleet;
    require(clc::sim::caravan_count(fleet) == 0, "empty fleet should count zero");
    require(clc::sim::add_caravan(fleet, caravan).ok(), "valid caravan should be added to fleet");
    require(clc::sim::caravan_count(fleet) == 1, "fleet should count inserted caravan");
    require(clc::sim::caravan_by_id(fleet, "caravan_001") != nullptr, "fleet lookup should find caravan");
    require(clc::sim::arrived_caravans(fleet).size() == 1, "arrived filter should include arrived caravan");
    require(clc::sim::active_caravans(fleet).empty(), "active filter should not include arrived caravan");

    auto active_caravan = clc::sim::create_caravan_for_route(route, "caravan_002", "Second Caravan");
    require(clc::sim::add_caravan(fleet, active_caravan).ok(), "second active caravan should be added");
    require(clc::sim::active_caravans(fleet).size() == 1, "active filter should include active caravan");
    require(clc::sim::arrived_caravans(fleet).size() == 1, "arrived filter should still include one caravan");
    require(!clc::sim::add_caravan(fleet, active_caravan).ok(), "duplicate caravan id should be rejected");
    require(clc::sim::caravan_count(fleet) == 2, "duplicate caravan should not change fleet size");

    const clc::sim::CaravanState empty_caravan;
    const auto empty_report = clc::sim::validate_caravan(empty_caravan);
    require(!empty_report.ok(), "empty caravan should fail validation");
    require(empty_report.error_count() == 7, "empty caravan should report all required field errors");

    auto over_remaining = active_caravan;
    over_remaining.days_remaining = over_remaining.total_travel_days + 1;
    require(!clc::sim::validate_caravan(over_remaining).ok(), "caravan should reject days_remaining above total_travel_days");

    auto wrong_route = route;
    wrong_route.destination_settlement_id = "stonegate";
    require(!clc::sim::validate_caravan_for_route(active_caravan, wrong_route).ok(), "caravan should reject mismatched route metadata");

    return 0;
}
