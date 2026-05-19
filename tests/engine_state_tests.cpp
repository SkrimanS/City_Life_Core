#include "clc/data/DataRegistry.hpp"
#include "clc/sim/SimulationEngine.hpp"

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

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    }).ok(), "grain should register");
    require(registry.add(clc::data::ResourceDefinition{
        .id = "wood",
        .display_name = "Wood",
        .category = "construction",
        .base_value = 6,
    }).ok(), "wood should register");
    require(registry.add(clc::data::SettlementDefinition{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .starting_population = 120,
    }).ok(), "riverwatch should register");
    return registry;
}

} // namespace

int main() {
    auto registry = make_registry();
    clc::sim::SimulationEngine source{registry};

    require(source.create_settlement("riverwatch").ok(), "source should create settlement");
    require(source.add_resource_to_settlement("riverwatch", "grain", 50).ok(), "source should add grain");
    require(source.add_resource_to_settlement("riverwatch", "wood", 20).ok(), "source should add wood");
    require(source.market().set_demand("grain", 30).ok(), "source should set market demand");
    const auto command = source.add_resource_to_settlement_command("riverwatch", "grain", 5);
    require(command.ok, "source command should succeed");
    require(source.advance_day().day == 1, "source should advance to day 1");

    const auto state = source.export_state();
    require(state.current_day == 1, "exported state should include current day");
    require(state.settlements.size() == 1, "exported state should include settlement");
    require(state.events.size() == source.events().size(), "exported state should include engine events");
    require(state.market_demands.size() == 1, "exported state should include market demands");
    require(state.market_demands[0].resource_id == "grain", "exported state should preserve demand resource id");
    require(state.market_demands[0].demand == 30, "exported state should preserve demand amount");

    clc::sim::SimulationEngine restored{registry};
    const auto restore_report = restored.restore_state(state);
    require(restore_report.ok(), "valid exported state should restore");
    require(restored.current_day() == source.current_day(), "restored current day should match source");
    require(restored.settlements().size() == 1, "restored settlement count should match source");
    require(restored.has_settlement("riverwatch"), "restored engine should contain riverwatch");
    require(restored.settlement_resource_amount("riverwatch", "grain") == source.settlement_resource_amount("riverwatch", "grain"), "restored grain amount should match source");
    require(restored.settlement_resource_amount("riverwatch", "wood") == source.settlement_resource_amount("riverwatch", "wood"), "restored wood amount should match source");
    require(restored.events().size() == source.events().size(), "restored events should match source count");
    require(restored.market().demand("grain") == 30, "restored market demand should match source");
    require(restored.snapshot().day == source.snapshot().day, "restored snapshot day should match source");

    const auto restored_day = restored.advance_day();
    require(restored_day.day == 2, "restored engine should continue from restored current day");
    require(restored.current_day() == 2, "restored engine should advance after restore");

    auto duplicate_settlement_state = state;
    duplicate_settlement_state.settlements.push_back(duplicate_settlement_state.settlements.front());
    clc::sim::SimulationEngine invalid_settlement_restore{registry};
    require(!invalid_settlement_restore.restore_state(duplicate_settlement_state).ok(), "duplicate settlement ids should be rejected");
    require(invalid_settlement_restore.current_day() == 0, "failed settlement restore should not mutate current day");
    require(invalid_settlement_restore.settlements().empty(), "failed settlement restore should not mutate settlements");

    auto duplicate_demand_state = state;
    duplicate_demand_state.market_demands.push_back(duplicate_demand_state.market_demands.front());
    clc::sim::SimulationEngine invalid_demand_restore{registry};
    require(!invalid_demand_restore.restore_state(duplicate_demand_state).ok(), "duplicate demand resource ids should be rejected");
    require(invalid_demand_restore.market().demand("grain") == 0, "failed demand restore should not mutate market");

    auto empty_ids_state = state;
    empty_ids_state.settlements.front().id = "";
    empty_ids_state.market_demands.front().resource_id = "";
    clc::sim::SimulationEngine empty_ids_restore{registry};
    const auto empty_ids_report = empty_ids_restore.restore_state(empty_ids_state);
    require(!empty_ids_report.ok(), "empty ids should be rejected during restore");
    require(empty_ids_report.error_count() == 2, "empty settlement and demand ids should both be reported");

    return 0;
}
