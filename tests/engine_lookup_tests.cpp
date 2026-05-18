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
    const auto load_report = loader.load_string("lookup-test", R"CLC(
schema_version=0.2.2

[resource]
id=grain
display_name=Grain
category=food
base_value=10

[settlement]
id=riverwatch
display_name=Riverwatch
starting_population=10
)CLC", registry);

    require(load_report.ok(), "lookup test data pack should load");

    clc::sim::SimulationEngine engine{std::move(registry)};
    require(!engine.has_settlement("riverwatch"), "engine should not report missing settlement as present");
    require(engine.settlement("riverwatch") == nullptr, "missing settlement lookup should return null");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 0, "missing settlement resource lookup should return zero");

    require(engine.create_settlement("riverwatch").ok(), "engine should create settlement");
    require(engine.has_settlement("riverwatch"), "engine should report existing settlement as present");
    require(!engine.has_settlement("missing"), "engine should report unknown settlement as absent");

    const auto* settlement = engine.settlement("riverwatch");
    require(settlement != nullptr, "existing settlement lookup should return pointer");
    require(settlement->id == "riverwatch", "settlement lookup should return requested settlement");
    require(engine.settlement("missing") == nullptr, "unknown settlement lookup should return null");

    require(engine.settlement_resource_amount("riverwatch", "grain") == 0, "missing resource amount should be zero");
    require(engine.add_resource_to_settlement("riverwatch", "grain", 7).ok(), "engine should add resources");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 7, "resource amount lookup should report stored amount");
    require(engine.settlement_resource_amount("riverwatch", "wood") == 0, "unknown resource amount should be zero");

    return 0;
}
