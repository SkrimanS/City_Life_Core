#include "clc/CityLifeCore.hpp"
#include "clc/data/DataPackLoader.hpp"

#include <iostream>
#include <string_view>

namespace {

constexpr std::string_view kDataPack = R"clcd(
schema_version = 0.2.2

[resource]
id = grain
display_name = Grain
category = food
base_value = 10

[resource]
id = flour
display_name = Flour
category = food
base_value = 18

[profession]
id = miller
display_name = Miller
category = production

[building]
id = mill
display_name = Mill
category = production
worker_slots = 3
required_profession_id = miller
input_resource_ids = grain
output_resource_ids = flour

[settlement]
id = riverwatch
display_name = Riverwatch
starting_population = 120
)clcd";

void print_validation(const clc::data::ValidationReport& report) {
    for (const auto& message : report.messages()) {
        std::cerr << message.path << ": " << message.message << '\n';
    }
}

} // namespace

int main() {
    clc::data::DataRegistry registry;
    clc::data::DataPackLoader loader;

    const auto load_report = loader.load_string("inline_example.clcd", kDataPack, registry);
    if (!load_report.ok()) {
        print_validation(load_report);
        return 1;
    }

    clc::sim::SimulationRuntime runtime{registry};
    const auto settlement_report = clc::sim::create_runtime_settlement(runtime, "riverwatch");
    if (!settlement_report.ok()) {
        print_validation(settlement_report);
        return 1;
    }

    const auto building_report = runtime.engine.add_building_to_settlement(
        "riverwatch",
        clc::sim::BuildingInstance{.definition_id = "mill", .assigned_workers = 3}
    );
    if (!building_report.ok()) {
        print_validation(building_report);
        return 1;
    }

    if (!runtime.engine.add_resource_to_settlement("riverwatch", "grain", 100).ok()) {
        return 1;
    }

    const auto tick_report = runtime.engine.advance_day();
    const auto flour = runtime.engine.settlement_resource_amount("riverwatch", "flour");

    std::cout << "settlements=" << tick_report.settlements.size() << '\n';
    std::cout << "flour=" << flour << '\n';
    std::cout << "resources=" << registry.resource_count() << '\n';
    std::cout << "buildings=" << registry.building_count() << '\n';

    return flour == 0 ? 1 : 0;
}
