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

    require(!engine.create_settlement("").ok(), "engine should reject empty settlement definition id");
    require(!engine.create_settlement("missing_settlement").ok(), "engine should reject unknown settlement definition id");
    require(engine.create_settlement("riverwatch").ok(), "engine should create settlement from definition id");
    require(!engine.create_settlement("riverwatch").ok(), "engine should reject duplicate settlement created from definition id");
    require(engine.settlements().size() == 1, "engine should contain created settlement");

    require(!engine.add_building_to_settlement("", clc::sim::BuildingInstance{.definition_id = "farm", .assigned_workers = 4}).ok(), "engine should reject empty settlement id for building command");
    require(!engine.add_building_to_settlement("missing", clc::sim::BuildingInstance{.definition_id = "farm", .assigned_workers = 4}).ok(), "engine should reject unknown settlement for building command");
    require(engine.add_building_to_settlement("riverwatch", clc::sim::BuildingInstance{.definition_id = "farm", .assigned_workers = 4}).ok(), "engine should add building to settlement by id");

    require(!engine.add_resource_to_settlement("", "grain", 10).ok(), "engine should reject empty settlement id for resource command");
    require(!engine.add_resource_to_settlement("missing", "grain", 10).ok(), "engine should reject unknown settlement for resource command");
    require(engine.add_resource_to_settlement("riverwatch", "grain", 10).ok(), "engine should add starting grain");
    require(engine.add_resource_to_settlement("riverwatch", "wood", 4).ok(), "engine should add starting wood");

    const auto zero_day_reports = engine.run_days(0);
    require(zero_day_reports.empty(), "run_days(0) should return no reports");
    require(engine.current_day() == 0, "run_days(0) should not advance current day");
    const auto empty_summary = clc::sim::summarize_day_reports(zero_day_reports);
    require(empty_summary.days_run == 0, "empty scenario summary should report zero days");
    require(empty_summary.first_day == 0, "empty scenario summary should report first day 0");
    require(empty_summary.last_day == 0, "empty scenario summary should report last day 0");
    require(empty_summary.events == 0, "empty scenario summary should report zero events");
    require(empty_summary.warnings == 0, "empty scenario summary should report zero warnings");

    const auto initial_snapshot = engine.snapshot();
    require(initial_snapshot.day == 0, "initial snapshot should report day 0");
    require(engine.current_day() == 0, "snapshot should not advance engine day");
    require(initial_snapshot.settlements.size() == 1, "initial snapshot should include settlement");
    require(initial_snapshot.settlements[0].total_stored_resources == 14, "initial snapshot should include starting storage");
    require(initial_snapshot.market.total_supply == 14, "initial snapshot should aggregate starting supply");
    require(initial_snapshot.market.total_demand == 20, "initial snapshot should include market demand");

    const auto day_report = engine.advance_day();
    require(day_report.day == 1, "first simulation day report should be day 1");
    require(engine.current_day() == 1, "engine day should advance");
    require(day_report.settlement_ticks.size() == 1, "day report should include settlement tick");
    require(day_report.settlements.size() == 1, "day report should include settlement snapshot");
    require(day_report.events.size() == 3, "day report should include start, settlement, and completion events");
    require(day_report.events[0].type == "simulation.day.started", "first event should be day started");
    require(day_report.events[1].type == "simulation.settlement.advanced", "second event should be settlement advanced");
    require(day_report.events[2].type == "simulation.day.completed", "third event should be day completed");
    require(day_report.warnings.empty(), "first day should not produce engine warnings");
    require(day_report.settlement_ticks[0].consumed_food == 4, "population 40 should consume 4 grain");
    require(day_report.settlement_ticks[0].consumed_inputs == 4, "farm should consume 4 wood");
    require(day_report.settlement_ticks[0].produced_resources == 4, "farm should produce 4 grain");
    require(day_report.settlements[0].id == "riverwatch", "settlement snapshot should include id");
    require(day_report.settlements[0].total_stored_resources == 10, "settlement should end with 10 total resources");
    require(day_report.market.prices.size() == 1, "market report should include stored grain price");
    require(day_report.market.total_supply == 10, "market report should aggregate settlement storage supply");
    require(day_report.market.total_demand == 20, "market report should include configured demand");

    const auto post_day_snapshot = engine.snapshot();
    require(post_day_snapshot.day == 1, "post-day snapshot should report current day");
    require(engine.current_day() == 1, "post-day snapshot should not advance engine day");
    require(post_day_snapshot.settlements.size() == 1, "post-day snapshot should include settlement");
    require(post_day_snapshot.settlements[0].total_stored_resources == 10, "post-day snapshot should include current storage");
    require(post_day_snapshot.market.total_supply == 10, "post-day snapshot should include current aggregate supply");

    const auto scenario_reports = engine.run_days(3);
    require(scenario_reports.size() == 3, "run_days(3) should return three reports");
    require(scenario_reports[0].day == 2, "first scenario report should continue from current day");
    require(scenario_reports[1].day == 3, "second scenario report should be next day");
    require(scenario_reports[2].day == 4, "third scenario report should be next day");
    require(engine.current_day() == 4, "run_days(3) should advance current day by three");
    require(scenario_reports[0].settlement_ticks[0].skipped_buildings == 1, "farm should be skipped on day 2 without wood");
    require(!scenario_reports[0].warnings.empty(), "day 2 should collect settlement warnings");
    require(scenario_reports[0].warnings[0].find("riverwatch:") == 0, "engine warning should include settlement id prefix");
    require(engine.events_by_type("simulation.day.completed").size() == 4, "run_days should append day completion events to cumulative log");

    const auto summary = clc::sim::summarize_day_reports(scenario_reports);
    require(summary.days_run == 3, "scenario summary should report three days");
    require(summary.first_day == 2, "scenario summary should report first scenario day");
    require(summary.last_day == 4, "scenario summary should report last scenario day");
    require(summary.settlement_ticks == 3, "scenario summary should count settlement ticks");
    require(summary.consumed_food == 6, "scenario summary should aggregate consumed food");
    require(summary.consumed_inputs == 0, "scenario summary should aggregate consumed inputs");
    require(summary.produced_resources == 0, "scenario summary should aggregate produced resources");
    require(summary.active_building_ticks == 0, "scenario summary should aggregate active buildings");
    require(summary.skipped_building_ticks == 3, "scenario summary should aggregate skipped buildings");
    require(summary.events == 9, "scenario summary should aggregate day report events");
    require(summary.warnings == 6, "scenario summary should aggregate engine and tick warnings");

    return 0;
}
