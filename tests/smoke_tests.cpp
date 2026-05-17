#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"
#include "clc/data/DataPackLoader.hpp"
#include "clc/data/DataRegistry.hpp"

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
    require(clc::core_version().major == 0, "major version should be 0 during bootstrap");
    require(clc::core_version_string() == std::string_view{"0.2.1"}, "version string should be 0.2.1");

    clc::World world{clc::WorldConfig{.name = "Smoke Test World", .seed = 42}};
    require(world.time().current_tick() == 0, "new world should start at tick 0");
    require(world.event_log().size() == 1, "world creation should emit one event");

    const auto zero_tick_result = world.advance(0);
    require(!zero_tick_result, "advancing by zero ticks should fail");

    const auto advance_result = world.advance(5);
    require(advance_result.ok(), "advancing by positive ticks should succeed");
    require(world.time().current_tick() == 5, "world should advance to tick 5");
    require(world.event_log().size() == 2, "world advance should emit one event");

    clc::data::DataRegistry registry;

    auto grain_report = registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    });
    require(grain_report.ok(), "valid resource should register");
    require(registry.resource_count() == 1, "registry should contain one resource");
    require(registry.resource("grain") != nullptr, "registered resource should be findable");

    auto duplicate_report = registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Duplicate Grain",
        .category = "food",
        .base_value = 11,
    });
    require(!duplicate_report.ok(), "duplicate resource id should fail validation");
    require(registry.resource_count() == 1, "duplicate resource should not be inserted");

    auto empty_currency_report = registry.add(clc::data::CurrencyDefinition{
        .id = "",
        .display_name = "",
        .fractional_digits = 2,
    });
    require(!empty_currency_report.ok(), "empty currency id and name should fail validation");

    auto coin_report = registry.add(clc::data::CurrencyDefinition{
        .id = "coin",
        .display_name = "Coin",
        .fractional_digits = 2,
    });
    require(coin_report.ok(), "valid currency should register");
    require(registry.currency_count() == 1, "registry should contain one currency");

    auto settlement_report = registry.add(clc::data::SettlementDefinition{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .starting_population = 120,
    });
    require(settlement_report.ok(), "valid settlement should register");
    require(registry.settlement_count() == 1, "registry should contain one settlement");

    clc::data::DataRegistry loaded_registry;
    clc::data::DataPackLoader loader;
    const auto load_report = loader.load_string("inline-test", R"CLC(
schema_version=0.2.1

[resource]
id=wood
display_name=Wood
category=construction
base_value=6

[currency]
id=coin
display_name=Coin
fractional_digits=2

[building]
id=farm
display_name=Farm
category=production
worker_slots=8

[profession]
id=farmer
display_name=Farmer
category=production

[settlement]
id=riverwatch
display_name=Riverwatch
starting_population=120
)CLC", loaded_registry);

    require(load_report.ok(), "valid data pack should load");
    require(loaded_registry.resource_count() == 1, "loader should register one resource");
    require(loaded_registry.currency_count() == 1, "loader should register one currency");
    require(loaded_registry.building_count() == 1, "loader should register one building");
    require(loaded_registry.profession_count() == 1, "loader should register one profession");
    require(loaded_registry.settlement_count() == 1, "loader should register one settlement");
    require(loaded_registry.resource("wood") != nullptr, "loaded resource should be findable");

    clc::data::DataRegistry invalid_registry;
    const auto invalid_report = loader.load_string("invalid-test", "schema_version=wrong\n", invalid_registry);
    require(!invalid_report.ok(), "unsupported schema version should fail");

    return 0;
}
