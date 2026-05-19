#include "clc/data/DataRegistry.hpp"
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

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "grain should register");
    require(registry.add(clc::data::ResourceDefinition{.id = "wood", .display_name = "Wood", .category = "construction", .base_value = 6}).ok(), "wood should register");
    require(registry.add(clc::data::SettlementDefinition{.id = "riverwatch", .display_name = "Riverwatch", .starting_population = 120}).ok(), "settlement should register");
    return registry;
}

void require_same_snapshot(const clc::sim::SimulationSnapshot& left, const clc::sim::SimulationSnapshot& right) {
    require(left.day == right.day, "snapshot day should round-trip");
    require(left.settlements.size() == right.settlements.size(), "settlement count should round-trip");
    require(left.settlements[0].display_name == right.settlements[0].display_name, "settlement name should round-trip");
    require(left.settlements[0].storage.size() == right.settlements[0].storage.size(), "storage count should round-trip");
    require(left.settlements[0].buildings[0].display_name == right.settlements[0].buildings[0].display_name, "building name should round-trip");
    require(left.market.prices[0].reason == right.market.prices[0].reason, "market reason should round-trip");
    require(left.events[0].message == right.events[0].message, "event message should round-trip");
}

void test_snapshot_persistence() {
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
    require(clc::sim::save_simulation_snapshot_to_file(snapshot, path).ok(), "snapshot should save to file");
    const auto loaded_file = clc::sim::load_simulation_snapshot_from_file(path);
    require(loaded_file.ok(), "snapshot should load from file");
    require_same_snapshot(snapshot, loaded_file.snapshot);
    std::filesystem::remove(path);

    require(!clc::sim::deserialize_simulation_snapshot("day\t1\n").ok(), "missing header should fail validation");
    require(!clc::sim::deserialize_simulation_snapshot("CLC_SIM_SNAPSHOT\t1\nday\t1\nstorage\tmissing\tgrain\t3\nmarket\t0\t0\t0\t0\t0\n").ok(), "storage row should require known settlement");
    require(!clc::sim::load_simulation_snapshot_from_file(path).ok(), "missing snapshot file should fail validation");
}

void test_engine_state_restore() {
    auto registry = make_registry();
    clc::sim::SimulationEngine source{registry};
    require(source.create_settlement("riverwatch").ok(), "source should create settlement");
    require(source.add_resource_to_settlement("riverwatch", "grain", 50).ok(), "source should add grain");
    require(source.add_resource_to_settlement("riverwatch", "wood", 20).ok(), "source should add wood");
    require(source.market().set_demand("grain", 30).ok(), "source should set demand");
    require(source.add_resource_to_settlement_command("riverwatch", "grain", 5).ok, "source command should succeed");
    require(source.advance_day().day == 1, "source should advance to day 1");

    const auto state = source.export_state();
    require(state.current_day == 1, "state should include current day");
    require(state.settlements.size() == 1, "state should include settlements");
    require(state.events.size() == source.events().size(), "state should include events");
    require(state.market_demands.size() == 1, "state should include market demands");

    clc::sim::SimulationEngine restored{registry};
    require(restored.restore_state(state).ok(), "valid state should restore");
    require(restored.current_day() == source.current_day(), "restored day should match");
    require(restored.settlement_resource_amount("riverwatch", "grain") == source.settlement_resource_amount("riverwatch", "grain"), "restored grain should match");
    require(restored.market().demand("grain") == 30, "restored demand should match");
    require(restored.advance_day().day == 2, "restored engine should continue from restored day");

    auto duplicate_settlement_state = state;
    duplicate_settlement_state.settlements.push_back(duplicate_settlement_state.settlements.front());
    clc::sim::SimulationEngine invalid_settlement_restore{registry};
    require(!invalid_settlement_restore.restore_state(duplicate_settlement_state).ok(), "duplicate settlement ids should be rejected");
    require(invalid_settlement_restore.settlements().empty(), "failed settlement restore should not mutate settlements");

    auto duplicate_demand_state = state;
    duplicate_demand_state.market_demands.push_back(duplicate_demand_state.market_demands.front());
    clc::sim::SimulationEngine invalid_demand_restore{registry};
    require(!invalid_demand_restore.restore_state(duplicate_demand_state).ok(), "duplicate demands should be rejected");
    require(invalid_demand_restore.market().demand("grain") == 0, "failed demand restore should not mutate market");
}

} // namespace

int main() {
    test_snapshot_persistence();
    test_engine_state_restore();
    return 0;
}
