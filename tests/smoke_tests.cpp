#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"
#include "clc/data/DataPackLoader.hpp"
#include "clc/data/DataRegistry.hpp"
#include "clc/sim/Settlement.hpp"
#include "clc/sim/Storage.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
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
    require(clc::core_version_string() == std::string_view{"0.7.9"}, "version string should be 0.7.9");

    clc::World world{clc::WorldConfig{.name = "Smoke Test World", .seed = 42}};
    require(world.time().current_tick() == 0, "new world should start at tick 0");
    require(world.event_log().size() == 1, "world creation should emit one event");

    const auto zero_tick_result = world.advance(0);
    require(!zero_tick_result, "advancing by zero ticks should fail");

    const auto advance_result = world.advance(5);
    require(advance_result.ok(), "advancing by positive ticks should succeed");
    require(world.time().current_tick() == 5, "world should advance to tick 5");
    require(world.event_log().size() == 2, "world advance should emit one event");

    clc::sim::ResourceStorage source_storage;
    clc::sim::ResourceStorage target_storage;
    require(source_storage.add("grain", 10).ok(), "storage should add resources");
    require(source_storage.amount("grain") == 10, "storage should report added amount");
    require(!source_storage.try_remove("grain", 11), "storage should reject over-removal");
    require(source_storage.amount("grain") == 10, "failed removal should not change amount");
    require(source_storage.remove_up_to("grain", 4) == 4, "storage should remove partial amount");
    require(source_storage.amount("grain") == 6, "storage should retain remaining amount");
    require(clc::sim::transfer(source_storage, target_storage, "grain", 6).ok(), "storage transfer should succeed when enough resource exists");
    require(source_storage.amount("grain") == 0, "transfer should debit source");
    require(target_storage.amount("grain") == 6, "transfer should credit target");

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

[currency]
id=coin
display_name=Coin
fractional_digits=2

[profession]
id=farmer
display_name=Farmer
category=production

[building]
id=farm
display_name=Farm
category=production
worker_slots=8
required_profession_id=farmer
input_resource_ids=wood
output_resource_ids=grain

[settlement]
id=riverwatch
display_name=Riverwatch
starting_population=120
)CLC", loaded_registry);

    require(load_report.ok(), "valid data pack should load");
    require(loaded_registry.resource_count() == 2, "loader should register two resources");
    require(loaded_registry.currency_count() == 1, "loader should register one currency");
    require(loaded_registry.building_count() == 1, "loader should register one building");
    require(loaded_registry.profession_count() == 1, "loader should register one profession");
    require(loaded_registry.settlement_count() == 1, "loader should register one settlement");
    require(loaded_registry.resource("grain") != nullptr, "loaded resource should be findable");
    require(loaded_registry.validate_references().ok(), "valid data pack references should pass");

    const auto* settlement_definition = loaded_registry.settlement("riverwatch");
    require(settlement_definition != nullptr, "settlement definition should exist");

    auto settlement = clc::sim::create_settlement_from_definition(*settlement_definition);
    require(settlement.storage.add("grain", 20).ok(), "settlement storage should accept starting grain");
    require(settlement.storage.add("wood", 8).ok(), "settlement storage should accept production input wood");

    const auto add_building_report = clc::sim::add_building(settlement, loaded_registry, clc::sim::BuildingInstance{
        .definition_id = "farm",
        .assigned_workers = 8,
    });
    require(add_building_report.ok(), "valid building instance should be added");

    const auto day_report = clc::sim::advance_settlement_day(settlement, loaded_registry);
    require(day_report.consumed_food == 12, "population 120 should consume 12 grain per day");
    require(day_report.consumed_inputs == 8, "farm should consume 8 wood input units per day");
    require(day_report.produced_resources == 8, "farm should produce 8 resource units per day");
    require(day_report.active_buildings == 1, "farm should be active when inputs exist");
    require(day_report.skipped_buildings == 0, "farm should not be skipped when inputs exist");
    require(settlement.storage.amount("wood") == 0, "wood should be consumed by production");
    require(settlement.storage.amount("grain") == 16, "grain should remain non-negative after consume and produce");

    const auto report = clc::sim::make_settlement_report(settlement, loaded_registry);
    require(report.id == "riverwatch", "settlement report should include id");
    require(report.display_name == "Riverwatch", "settlement report should include display name");
    require(report.population == 120, "settlement report should include population");
    require(report.storage.size() == 1, "settlement report should include non-empty storage entries");
    require(report.storage[0].resource_id == "grain", "settlement report storage should be sorted and include grain");
    require(report.storage[0].amount == 16, "settlement report should include grain amount");
    require(report.total_stored_resources == 16, "settlement report should include total stored resources");
    require(report.buildings.size() == 1, "settlement report should include building entries");
    require(report.buildings[0].definition_id == "farm", "settlement report should include building id");
    require(report.buildings[0].display_name == "Farm", "settlement report should resolve building display name");
    require(report.buildings[0].assigned_workers == 8, "settlement report should include assigned workers");
    require(report.buildings[0].worker_slots == 8, "settlement report should include worker slots");

    auto input_starved_settlement = clc::sim::create_settlement_from_definition(*settlement_definition);
    require(input_starved_settlement.storage.add("grain", 20).ok(), "input-starved settlement should accept grain");
    require(clc::sim::add_building(input_starved_settlement, loaded_registry, clc::sim::BuildingInstance{
        .definition_id = "farm",
        .assigned_workers = 8,
    }).ok(), "input-starved settlement should accept building");
    const auto starved_report = clc::sim::advance_settlement_day(input_starved_settlement, loaded_registry);
    require(starved_report.produced_resources == 0, "farm should not produce when input resources are missing");
    require(starved_report.consumed_inputs == 0, "farm should not partially consume missing inputs");
    require(starved_report.active_buildings == 0, "farm should not be active without inputs");
    require(starved_report.skipped_buildings == 1, "farm should be skipped without inputs");

    clc::data::DataRegistry invalid_registry;
    const auto invalid_report = loader.load_string("invalid-test", "schema_version=wrong\n", invalid_registry);
    require(!invalid_report.ok(), "unsupported schema version should fail");

    clc::data::DataRegistry broken_registry;
    const auto broken_report = loader.load_string("broken-ref-test", R"CLC(
schema_version=0.2.2

[building]
id=broken_farm
display_name=Broken Farm
category=production
worker_slots=8
required_profession_id=missing_profession
output_resource_ids=missing_resource
)CLC", broken_registry);
    require(!broken_report.ok(), "unknown building references should fail validation");

    const auto directory = std::filesystem::temp_directory_path() / "clc_pack_folder_smoke";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    {
        std::ofstream definitions{directory / "01_definitions.clcd"};
        definitions << R"CLC(schema_version=0.2.2

[resource]
id=grain
display_name=Grain
category=food
base_value=10

[profession]
id=farmer
display_name=Farmer
category=production
)CLC";
    }

    {
        std::ofstream buildings{directory / "02_buildings.clcd"};
        buildings << R"CLC(schema_version=0.2.2

[building]
id=farm
display_name=Farm
category=production
worker_slots=8
required_profession_id=farmer
output_resource_ids=grain
)CLC";
    }

    clc::data::DataRegistry directory_registry;
    const auto directory_report = loader.load_directory(directory, directory_registry);
    require(directory_report.ok(), "valid data pack directory should load");
    require(directory_registry.resource_count() == 1, "directory loader should register one resource");
    require(directory_registry.profession_count() == 1, "directory loader should register one profession");
    require(directory_registry.building_count() == 1, "directory loader should register one building");
    require(directory_registry.validate_references().ok(), "directory references should validate");

    std::filesystem::remove_all(directory);

    return 0;
}
