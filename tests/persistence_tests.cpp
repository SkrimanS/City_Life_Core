#include "clc/sim/SimulationPersistence.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::SimulationSnapshot make_test_snapshot() {
    clc::sim::SimulationSnapshot snapshot;
    snapshot.day = 12;
    snapshot.settlements.push_back(clc::sim::SettlementReport{
        .id = "riverwatch",
        .display_name = "River\tWatch 100%",
        .population = 120,
        .storage = {
            clc::sim::ResourceAmount{.resource_id = "grain", .amount = 16},
            clc::sim::ResourceAmount{.resource_id = "wood", .amount = 8},
        },
        .buildings = {
            clc::sim::BuildingReport{.definition_id = "farm", .display_name = "Farm\nNorth", .assigned_workers = 8, .worker_slots = 8},
        },
        .total_stored_resources = 24,
    });
    snapshot.market.total_supply = 24;
    snapshot.market.total_demand = 10;
    snapshot.market.average_price = 7;
    snapshot.market.min_price = 6;
    snapshot.market.max_price = 8;
    snapshot.market.prices.push_back(clc::economy::MarketPrice{
        .resource_id = "grain",
        .base_value = 10,
        .supply = 16,
        .demand = 10,
        .price = 6,
        .reason = "supply>demand",
    });
    snapshot.events.push_back(clc::sim::SimulationEvent{
        .day = 12,
        .type = "simulation.day.completed",
        .message = "done\r\n100%",
    });
    return snapshot;
}

void require_same_snapshot(const clc::sim::SimulationSnapshot& left, const clc::sim::SimulationSnapshot& right) {
    require(left.day == right.day, "snapshot day should round-trip");
    require(left.settlements.size() == right.settlements.size(), "settlement count should round-trip");
    require(right.settlements.size() == 1, "test snapshot should contain one settlement");
    require(left.settlements[0].id == right.settlements[0].id, "settlement id should round-trip");
    require(left.settlements[0].display_name == right.settlements[0].display_name, "settlement display name should round-trip");
    require(left.settlements[0].population == right.settlements[0].population, "settlement population should round-trip");
    require(left.settlements[0].total_stored_resources == right.settlements[0].total_stored_resources, "settlement total storage should round-trip");
    require(left.settlements[0].storage.size() == right.settlements[0].storage.size(), "storage count should round-trip");
    require(left.settlements[0].storage[0].resource_id == right.settlements[0].storage[0].resource_id, "storage resource id should round-trip");
    require(left.settlements[0].storage[0].amount == right.settlements[0].storage[0].amount, "storage amount should round-trip");
    require(left.settlements[0].buildings.size() == right.settlements[0].buildings.size(), "building count should round-trip");
    require(left.settlements[0].buildings[0].definition_id == right.settlements[0].buildings[0].definition_id, "building definition id should round-trip");
    require(left.settlements[0].buildings[0].display_name == right.settlements[0].buildings[0].display_name, "building display name should round-trip");
    require(left.settlements[0].buildings[0].assigned_workers == right.settlements[0].buildings[0].assigned_workers, "assigned workers should round-trip");
    require(left.settlements[0].buildings[0].worker_slots == right.settlements[0].buildings[0].worker_slots, "worker slots should round-trip");
    require(left.market.total_supply == right.market.total_supply, "market total supply should round-trip");
    require(left.market.total_demand == right.market.total_demand, "market total demand should round-trip");
    require(left.market.average_price == right.market.average_price, "market average price should round-trip");
    require(left.market.min_price == right.market.min_price, "market min price should round-trip");
    require(left.market.max_price == right.market.max_price, "market max price should round-trip");
    require(left.market.prices.size() == right.market.prices.size(), "market price count should round-trip");
    require(left.market.prices[0].resource_id == right.market.prices[0].resource_id, "market price resource id should round-trip");
    require(left.market.prices[0].reason == right.market.prices[0].reason, "market price reason should round-trip");
    require(left.events.size() == right.events.size(), "event count should round-trip");
    require(left.events[0].day == right.events[0].day, "event day should round-trip");
    require(left.events[0].type == right.events[0].type, "event type should round-trip");
    require(left.events[0].message == right.events[0].message, "event message should round-trip");
}

} // namespace

int main() {
    const auto snapshot = make_test_snapshot();
    const auto serialized = clc::sim::serialize_simulation_snapshot(snapshot);
    require(serialized.find("CLC_SIM_SNAPSHOT\t1\n") == 0, "serialized snapshot should include format header");
    require(serialized.find("River%09Watch 100%25") != std::string::npos, "serialized snapshot should escape tabs and percent signs");
    require(serialized.find("Farm%0ANorth") != std::string::npos, "serialized snapshot should escape newlines");

    const auto loaded = clc::sim::deserialize_simulation_snapshot(serialized);
    require(loaded.ok(), "serialized snapshot should deserialize successfully");
    require_same_snapshot(snapshot, loaded.snapshot);

    const auto path = std::filesystem::temp_directory_path() / "clc_snapshot_persistence_test.clcs";
    std::filesystem::remove(path);
    const auto save_report = clc::sim::save_simulation_snapshot_to_file(snapshot, path);
    require(save_report.ok(), "snapshot should save to file");
    const auto loaded_file = clc::sim::load_simulation_snapshot_from_file(path);
    require(loaded_file.ok(), "snapshot should load from file");
    require_same_snapshot(snapshot, loaded_file.snapshot);
    std::filesystem::remove(path);

    const auto invalid = clc::sim::deserialize_simulation_snapshot("day\t1\n");
    require(!invalid.ok(), "missing header should fail validation");

    const auto bad_storage = clc::sim::deserialize_simulation_snapshot("CLC_SIM_SNAPSHOT\t1\nday\t1\nstorage\tmissing\tgrain\t3\nmarket\t0\t0\t0\t0\t0\n");
    require(!bad_storage.ok(), "storage row should require known settlement");

    const auto missing_file = clc::sim::load_simulation_snapshot_from_file(path);
    require(!missing_file.ok(), "missing snapshot file should fail validation");

    return 0;
}
