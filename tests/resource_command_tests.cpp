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
    const auto load_report = loader.load_string("resource-command-test", R"CLC(
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

[settlement]
id=hillford
display_name=Hillford
starting_population=10
)CLC", registry);

    require(load_report.ok(), "resource command data pack should load");

    clc::sim::SimulationEngine engine{std::move(registry)};
    require(engine.create_settlement("riverwatch").ok(), "engine should create source settlement");
    require(engine.create_settlement("hillford").ok(), "engine should create target settlement");

    require(!engine.add_resource_to_settlement("riverwatch", "unknown", 1).ok(), "add should reject unknown resource id");
    require(engine.settlement_resource_amount("riverwatch", "unknown") == 0, "unknown resource add should not mutate storage");
    require(!engine.add_resource_to_settlement("riverwatch", "grain", 0).ok(), "add should reject zero amount");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 0, "zero add should not mutate storage");

    require(engine.add_resource_to_settlement("riverwatch", "grain", 10).ok(), "engine should add grain to source settlement");
    require(engine.add_resource_to_settlement("hillford", "grain", 2).ok(), "engine should add grain to target settlement");

    require(!engine.remove_resource_from_settlement("", "grain", 1).ok(), "remove should reject empty settlement id");
    require(!engine.remove_resource_from_settlement("missing", "grain", 1).ok(), "remove should reject unknown settlement id");
    require(!engine.remove_resource_from_settlement("riverwatch", "", 1).ok(), "remove should reject empty resource id");
    require(!engine.remove_resource_from_settlement("riverwatch", "unknown", 1).ok(), "remove should reject unknown resource id");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 10, "unknown resource remove should not mutate existing storage");
    require(!engine.remove_resource_from_settlement("riverwatch", "grain", 0).ok(), "remove should reject zero amount");
    require(!engine.remove_resource_from_settlement("riverwatch", "grain", 100).ok(), "remove should reject over-removal");
    require(engine.remove_resource_from_settlement("riverwatch", "grain", 3).ok(), "remove should debit settlement storage");

    auto snapshot = engine.snapshot();
    require(snapshot.settlements.size() == 2, "snapshot should include two settlements");
    require(snapshot.settlements[0].total_stored_resources == 7, "source storage should reflect removal");
    require(snapshot.settlements[1].total_stored_resources == 2, "target storage should be unchanged after removal");
    require(snapshot.market.total_supply == 9, "market supply should reflect removal");

    require(!engine.transfer_resource_between_settlements("", "hillford", "grain", 1).ok(), "transfer should reject empty source id");
    require(!engine.transfer_resource_between_settlements("riverwatch", "", "grain", 1).ok(), "transfer should reject empty target id");
    require(!engine.transfer_resource_between_settlements("riverwatch", "riverwatch", "grain", 1).ok(), "transfer should reject same source and target");
    require(!engine.transfer_resource_between_settlements("missing", "hillford", "grain", 1).ok(), "transfer should reject unknown source");
    require(!engine.transfer_resource_between_settlements("riverwatch", "missing", "grain", 1).ok(), "transfer should reject unknown target");
    require(!engine.transfer_resource_between_settlements("riverwatch", "hillford", "", 1).ok(), "transfer should reject empty resource id");
    require(!engine.transfer_resource_between_settlements("riverwatch", "hillford", "unknown", 1).ok(), "transfer should reject unknown resource id");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 7, "unknown resource transfer should not debit source");
    require(engine.settlement_resource_amount("hillford", "grain") == 2, "unknown resource transfer should not credit target");
    require(!engine.transfer_resource_between_settlements("riverwatch", "hillford", "grain", 0).ok(), "transfer should reject zero amount");
    require(!engine.transfer_resource_between_settlements("hillford", "riverwatch", "grain", 100).ok(), "transfer should reject insufficient source resources");
    require(engine.transfer_resource_between_settlements("riverwatch", "hillford", "grain", 4).ok(), "transfer should move resources between settlements");

    snapshot = engine.snapshot();
    require(snapshot.settlements[0].total_stored_resources == 3, "source storage should reflect transfer debit");
    require(snapshot.settlements[1].total_stored_resources == 6, "target storage should reflect transfer credit");
    require(snapshot.market.total_supply == 9, "market supply should be conserved by transfer");

    return 0;
}
