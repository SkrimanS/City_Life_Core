#include "clc/data/DataPackLoader.hpp"
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

} // namespace

int main() {
    clc::data::DataRegistry registry;
    clc::data::DataPackLoader loader;
    const auto load_report = loader.load_string("simulation-test", R"CLC(
schema_version=0.2.2

[resource]
id=grain
display_name=Grain
category=food
base_value=10

[resource]
id=wood
display_name=Wood
category=construction
base_value=6

[profession]
id=farmer
display_name=Farmer
category=production

[building]
id=farm
display_name=Farm
category=production
worker_slots=4
required_profession_id=farmer
input_resource_ids=wood
output_resource_ids=grain

[settlement]
id=riverwatch
display_name=Riverwatch
starting_population=40
)CLC", registry);

    require(load_report.ok(), "simulation data pack should load");

    clc::sim::SimulationEngine engine{std::move(registry)};
    require(engine.current_day() == 0, "new simulation should start at day 0");
    require(engine.market().set_demand("grain", 20).ok(), "engine market should accept demand");

    const auto* settlement_definition = engine.registry().settlement("riverwatch");
    require(settlement_definition != nullptr, "settlement definition should exist");

    auto settlement = clc::sim::create_settlement_from_definition(*settlement_definition);
    require(settlement.storage.add("grain", 10).ok(), "settlement should accept starting grain");
    require(settlement.storage.add("wood", 4).ok(), "settlement should accept starting wood");
    require(clc::sim::add_building(settlement, engine.registry(), clc::sim::BuildingInstance{
        .definition_id = "farm",
        .assigned_workers = 4,
    }).ok(), "settlement should accept farm building");

    require(engine.add_settlement(std::move(settlement)).ok(), "engine should accept settlement");
    require(!engine.add_settlement(clc::sim::SettlementState{.id = "riverwatch"}).ok(), "engine should reject duplicate settlement id");

    const auto day_report = engine.advance_day();
    require(day_report.day == 1, "first simulation day report should be day 1");
    require(engine.current_day() == 1, "engine day should advance");
    require(day_report.settlement_ticks.size() == 1, "day report should include settlement tick");
    require(day_report.settlements.size() == 1, "day report should include settlement snapshot");
    require(day_report.settlement_ticks[0].consumed_food == 4, "population 40 should consume 4 grain");
    require(day_report.settlement_ticks[0].consumed_inputs == 4, "farm should consume 4 wood");
    require(day_report.settlement_ticks[0].produced_resources == 4, "farm should produce 4 grain");
    require(day_report.settlements[0].id == "riverwatch", "settlement snapshot should include id");
    require(day_report.settlements[0].total_stored_resources == 10, "settlement should end with 10 total resources");
    require(day_report.market.prices.size() == 1, "market report should include stored grain price");
    require(day_report.market.total_supply == 10, "market report should aggregate settlement storage supply");
    require(day_report.market.total_demand == 20, "market report should include configured demand");

    return 0;
}
