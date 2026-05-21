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
    return clc::sim::make_settlement_route_days(
        "riverwatch_to_hillford",
        "Riverwatch to Hillford",
        "riverwatch",
        "hillford",
        3
    );
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
    require(caravan.total_travel_ticks == clc::days_to_ticks(3), "caravan total travel ticks should come from route");
    require(caravan.ticks_remaining == clc::days_to_ticks(3), "new caravan should start with full travel ticks remaining");
    require(caravan.cargo.amount("grain") == 20, "caravan cargo grain should be preserved");
    require(caravan.cargo.amount("wood") == 5, "caravan cargo wood should be preserved");
    require(!clc::sim::caravan_arrived(caravan), "new caravan should not be arrived");

    const auto hour_route = clc::sim::make_settlement_route_ticks(
        "riverwatch_to_hillford_hours",
        "Riverwatch to Hillford Hours",
        "riverwatch",
        "hillford",
        clc::hours_to_ticks(3)
    );
    auto hourly_caravan = clc::sim::create_caravan_for_route(hour_route, "hourly_caravan", "Hourly Caravan");
    require(clc::sim::validate_caravan(hourly_caravan).ok(), "hourly caravan should pass validation");
    require(clc::sim::validate_caravan_for_route(hourly_caravan, hour_route).ok(), "hourly caravan should match tick route");
    require(hourly_caravan.total_travel_days == 0, "hourly caravan should not require day travel duration");
    require(hourly_caravan.days_remaining == 0, "hourly caravan day compatibility field should remain zero");
    require(hourly_caravan.total_travel_ticks == clc::hours_to_ticks(3), "hourly caravan total ticks should match route");
    require(hourly_caravan.ticks_remaining == clc::hours_to_ticks(3), "hourly caravan remaining ticks should match route");
    const auto first_hour = clc::sim::advance_caravan_ticks(hourly_caravan, clc::hours_to_ticks(1));
    require(first_hour.ticks_remaining_before == clc::hours_to_ticks(3), "hourly caravan first report should expose starting ticks");
    require(first_hour.ticks_remaining_after == clc::hours_to_ticks(2), "hourly caravan should move by one hour");
    require(first_hour.ticks_elapsed == clc::hours_to_ticks(1), "hourly caravan should expose elapsed ticks");
    require(first_hour.moved, "hourly caravan should move on first hour");
    require(!first_hour.arrived, "hourly caravan should not arrive after one hour");
    const auto final_hours = clc::sim::advance_caravan_ticks(hourly_caravan, clc::hours_to_ticks(2));
    require(final_hours.ticks_remaining_after == 0, "hourly caravan should arrive after remaining hours");
    require(final_hours.arrived, "hourly caravan should arrive after tick duration elapsed");
    require(clc::sim::caravan_arrived(hourly_caravan), "hourly caravan should be arrived after tick travel elapsed");

    clc::sim::SettlementState origin{.id = "riverwatch", .display_name = "Riverwatch", .population = 120};
    clc::sim::SettlementState destination{.id = "hillford", .display_name = "Hillford", .population = 80};
    clc::sim::SettlementState wrong_settlement{.id = "stonegate", .display_name = "Stonegate", .population = 60};
    require(origin.storage.add("grain", 50).ok(), "origin should accept grain");

    auto logistics_caravan = clc::sim::create_caravan_for_route(route, "caravan_logistics", "Logistics Caravan");
    require(clc::sim::load_caravan_at_origin(logistics_caravan, origin, "grain", 30).ok(), "caravan should load cargo at origin before departure");
    require(origin.storage.amount("grain") == 20, "loading should debit origin storage");
    require(logistics_caravan.cargo.amount("grain") == 30, "loading should credit caravan cargo");
    require(!clc::sim::load_caravan_at_origin(logistics_caravan, wrong_settlement, "grain", 1).ok(), "caravan should reject loading at non-origin settlement");
    require(!clc::sim::unload_caravan_at_destination(logistics_caravan, destination, "grain", 1).ok(), "caravan should reject unloading before arrival");
    require(clc::sim::advance_caravan_day(logistics_caravan).days_remaining_after == 2, "logistics caravan should move day one");
    require(!clc::sim::load_caravan_at_origin(logistics_caravan, origin, "grain", 1).ok(), "caravan should reject loading after departure");
    require(clc::sim::advance_caravan_day(logistics_caravan).days_remaining_after == 1, "logistics caravan should move day two");
    require(clc::sim::advance_caravan_day(logistics_caravan).arrived, "logistics caravan should arrive on final day");
    require(!clc::sim::unload_caravan_at_destination(logistics_caravan, wrong_settlement, "grain", 1).ok(), "caravan should reject unloading at non-destination settlement");
    require(!clc::sim::unload_caravan_at_destination(logistics_caravan, destination, "grain", 31).ok(), "caravan should reject unloading more cargo than available");
    require(clc::sim::unload_caravan_at_destination(logistics_caravan, destination, "grain", 30).ok(), "caravan should unload cargo after arrival at destination");
    require(logistics_caravan.cargo.amount("grain") == 0, "unloading should debit caravan cargo");
    require(destination.storage.amount("grain") == 30, "unloading should credit destination storage");
    require(!clc::sim::load_caravan_at_origin(logistics_caravan, origin, "", 1).ok(), "loading should reject empty resource id");
    require(!clc::sim::load_caravan_at_origin(logistics_caravan, origin, "grain", 0).ok(), "loading should reject zero amount");

    const auto first_day = clc::sim::advance_caravan_day(caravan);
    require(first_day.caravan_id == "caravan_001", "advance report should include caravan id");
    require(first_day.route_id == route.id, "advance report should include route id");
    require(first_day.days_remaining_before == 3, "first day should start with 3 days remaining");
    require(first_day.days_remaining_after == 2, "first day should reduce remaining days");
    require(first_day.ticks_remaining_before == clc::days_to_ticks(3), "first day should start with 3 days in ticks");
    require(first_day.ticks_remaining_after == clc::days_to_ticks(2), "first day should reduce remaining ticks by one day");
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
    require(empty_report.error_count() == 6, "empty caravan should report all required field errors");

    auto over_remaining = active_caravan;
    over_remaining.ticks_remaining = over_remaining.total_travel_ticks + 1;
    require(!clc::sim::validate_caravan(over_remaining).ok(), "caravan should reject ticks_remaining above total_travel_ticks");

    auto wrong_route = route;
    wrong_route.destination_settlement_id = "stonegate";
    require(!clc::sim::validate_caravan_for_route(active_caravan, wrong_route).ok(), "caravan should reject mismatched route metadata");

    return 0;
}
