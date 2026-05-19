#include "clc/sim/Ownership.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::FactionCatalog make_factions() {
    clc::sim::FactionCatalog factions;
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "riverwatch faction should add");
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "traders_guild", .display_name = "Traders Guild"}).ok(), "traders guild faction should add");
    return factions;
}

std::vector<clc::sim::SettlementState> make_settlements() {
    return {
        clc::sim::SettlementState{.id = "riverwatch", .display_name = "Riverwatch", .population = 120},
        clc::sim::SettlementState{.id = "hillford", .display_name = "Hillford", .population = 80},
    };
}

clc::sim::CaravanFleet make_caravans() {
    clc::sim::CaravanFleet fleet;
    const auto route = clc::sim::SettlementRoute{
        .id = "riverwatch_to_hillford",
        .display_name = "Riverwatch to Hillford",
        .origin_settlement_id = "riverwatch",
        .destination_settlement_id = "hillford",
        .travel_days = 3,
    };
    require(clc::sim::add_caravan(fleet, clc::sim::create_caravan_for_route(route, "caravan_001", "First Caravan")).ok(), "first caravan should add");
    require(clc::sim::add_caravan(fleet, clc::sim::create_caravan_for_route(route, "caravan_002", "Second Caravan")).ok(), "second caravan should add");
    return fleet;
}

} // namespace

int main() {
    const clc::sim::SettlementOwnership empty_settlement_ownership;
    const auto empty_settlement_report = clc::sim::validate_settlement_ownership(empty_settlement_ownership);
    require(!empty_settlement_report.ok(), "empty settlement ownership should fail validation");
    require(empty_settlement_report.error_count() == 2, "empty settlement ownership should report settlement and faction errors");

    const clc::sim::CaravanOwnership empty_caravan_ownership;
    const auto empty_caravan_report = clc::sim::validate_caravan_ownership(empty_caravan_ownership);
    require(!empty_caravan_report.ok(), "empty caravan ownership should fail validation");
    require(empty_caravan_report.error_count() == 2, "empty caravan ownership should report caravan and faction errors");

    const auto factions = make_factions();
    const auto settlements = make_settlements();
    const auto caravans = make_caravans();

    require(clc::sim::validate_settlement_ownership_references(
        clc::sim::SettlementOwnership{.settlement_id = "riverwatch", .faction_id = "riverwatch"}, settlements, factions).ok(), "valid settlement ownership should resolve references");
    require(clc::sim::validate_caravan_ownership_references(
        clc::sim::CaravanOwnership{.caravan_id = "caravan_001", .faction_id = "traders_guild"}, caravans, factions).ok(), "valid caravan ownership should resolve references");

    const auto unknown_settlement_report = clc::sim::validate_settlement_ownership_references(
        clc::sim::SettlementOwnership{.settlement_id = "missing", .faction_id = "missing_faction"}, settlements, factions);
    require(!unknown_settlement_report.ok(), "unknown settlement ownership references should fail");
    require(unknown_settlement_report.error_count() == 2, "unknown settlement and faction should both report errors");

    const auto unknown_caravan_report = clc::sim::validate_caravan_ownership_references(
        clc::sim::CaravanOwnership{.caravan_id = "missing", .faction_id = "missing_faction"}, caravans, factions);
    require(!unknown_caravan_report.ok(), "unknown caravan ownership references should fail");
    require(unknown_caravan_report.error_count() == 2, "unknown caravan and faction should both report errors");

    clc::sim::OwnershipCatalog catalog;
    require(clc::sim::settlement_ownership_count(catalog) == 0, "empty ownership catalog should have zero settlement owners");
    require(clc::sim::caravan_ownership_count(catalog) == 0, "empty ownership catalog should have zero caravan owners");
    require(clc::sim::settlement_owner(catalog, "riverwatch").empty(), "missing settlement owner should be empty");
    require(clc::sim::caravan_owner(catalog, "caravan_001").empty(), "missing caravan owner should be empty");

    require(clc::sim::set_settlement_owner(catalog, "riverwatch", "riverwatch").ok(), "settlement owner should set");
    require(clc::sim::set_settlement_owner(catalog, "hillford", "traders_guild").ok(), "second settlement owner should set");
    require(clc::sim::settlement_ownership_count(catalog) == 2, "catalog should count settlement owners");
    require(clc::sim::settlement_owner(catalog, "riverwatch") == std::string_view{"riverwatch"}, "settlement owner should be lookupable");
    require(clc::sim::set_settlement_owner(catalog, "riverwatch", "traders_guild").ok(), "settlement owner should update");
    require(clc::sim::settlement_ownership_count(catalog) == 2, "settlement owner update should not duplicate entries");
    require(clc::sim::settlement_owner(catalog, "riverwatch") == std::string_view{"traders_guild"}, "settlement owner update should replace faction");
    require(!clc::sim::set_settlement_owner(catalog, "", "riverwatch").ok(), "empty settlement id should reject owner set");
    require(!clc::sim::set_settlement_owner(catalog, "riverwatch", "").ok(), "empty settlement faction id should reject owner set");

    require(clc::sim::set_caravan_owner(catalog, "caravan_001", "riverwatch").ok(), "caravan owner should set");
    require(clc::sim::set_caravan_owner(catalog, "caravan_002", "traders_guild").ok(), "second caravan owner should set");
    require(clc::sim::caravan_ownership_count(catalog) == 2, "catalog should count caravan owners");
    require(clc::sim::caravan_owner(catalog, "caravan_001") == std::string_view{"riverwatch"}, "caravan owner should be lookupable");
    require(clc::sim::set_caravan_owner(catalog, "caravan_001", "traders_guild").ok(), "caravan owner should update");
    require(clc::sim::caravan_ownership_count(catalog) == 2, "caravan owner update should not duplicate entries");
    require(clc::sim::caravan_owner(catalog, "caravan_001") == std::string_view{"traders_guild"}, "caravan owner update should replace faction");
    require(!clc::sim::set_caravan_owner(catalog, "", "riverwatch").ok(), "empty caravan id should reject owner set");
    require(!clc::sim::set_caravan_owner(catalog, "caravan_001", "").ok(), "empty caravan faction id should reject owner set");

    const auto trader_settlements = clc::sim::settlements_owned_by(catalog, "traders_guild");
    require(trader_settlements.size() == 2, "traders guild should own two settlements after update");
    const auto trader_caravans = clc::sim::caravans_owned_by(catalog, "traders_guild");
    require(trader_caravans.size() == 2, "traders guild should own two caravans after update");
    require(clc::sim::settlements_owned_by(catalog, "missing").empty(), "missing faction should own no settlements");
    require(clc::sim::caravans_owned_by(catalog, "missing").empty(), "missing faction should own no caravans");

    return 0;
}
