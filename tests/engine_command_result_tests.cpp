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
}

int main() {
    clc::data::DataRegistry registry;
    clc::data::DataPackLoader loader;
    const auto load_report = loader.load_string("command-result-test", R"CLC(
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
    require(load_report.ok(), "data pack should load");

    clc::sim::SimulationEngine engine{std::move(registry)};
    require(engine.events().empty(), "new engine event log should be empty");

    const auto failed_create = engine.create_settlement_command("missing");
    require(!failed_create.ok, "missing create should fail");
    require(failed_create.command == "create_settlement", "create command name");
    require(failed_create.subject_id == "missing", "create subject id");
    require(!failed_create.validation.ok(), "create validation failure");
    require(engine.events().size() == 1, "failed command should append event");
    require(engine.events()[0].type == "simulation.command.failed", "failed command event type");

    const auto created = engine.create_settlement_command("riverwatch");
    require(created.ok, "create should pass");
    require(created.validation.ok(), "create validation ok");
    require(created.subject_id == "riverwatch", "created subject id");
    require(engine.events()[1].type == "simulation.command.succeeded", "successful command event type");
    require(engine.create_settlement_command("hillford").ok, "second create should pass");

    const auto added = engine.add_resource_to_settlement_command("riverwatch", "grain", 7);
    require(added.ok, "add resource should pass");
    require(added.command == "add_resource_to_settlement", "add command name");
    require(added.subject_id == "riverwatch", "add subject id");
    require(added.resource_id == "grain", "add resource id");
    require(added.amount == 7, "add amount");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 7, "amount after add");

    const auto removed = engine.remove_resource_from_settlement_command("riverwatch", "grain", 2);
    require(removed.ok, "remove should pass");
    require(removed.command == "remove_resource_from_settlement", "remove command name");
    require(removed.amount == 2, "remove amount");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 5, "amount after remove");

    const auto failed_remove = engine.remove_resource_from_settlement_command("riverwatch", "grain", 100);
    require(!failed_remove.ok, "large remove should fail");
    require(!failed_remove.validation.ok(), "large remove validation failure");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 5, "amount after failed remove");

    const auto moved = engine.transfer_resource_between_settlements_command("riverwatch", "hillford", "grain", 3);
    require(moved.ok, "transfer should pass");
    require(moved.command == "transfer_resource_between_settlements", "transfer command name");
    require(moved.subject_id == "riverwatch", "transfer subject id");
    require(moved.target_id == "hillford", "transfer target id");
    require(moved.resource_id == "grain", "transfer resource id");
    require(moved.amount == 3, "transfer amount");
    require(engine.settlement_resource_amount("riverwatch", "grain") == 2, "source amount after transfer");
    require(engine.settlement_resource_amount("hillford", "grain") == 3, "target amount after transfer");
    require(engine.events().size() == 7, "all command wrappers should append events");

    const auto snapshot = engine.snapshot();
    require(snapshot.events.size() == engine.events().size(), "snapshot should include cumulative engine events");

    const auto day_report = engine.advance_day();
    require(day_report.events.size() == 4, "day report should include day and settlement events");
    require(engine.events().size() == 11, "advance_day should append day events to cumulative log");

    return 0;
}
