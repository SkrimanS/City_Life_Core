#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"
#include "clc/data/DataRegistry.hpp"

#include <iostream>

int main() {
    clc::World world{clc::WorldConfig{.name = "Bootstrap World", .seed = 1}};

    const auto advanced = world.advance(1);
    if (!advanced) {
        std::cerr << "Failed to advance world: " << advanced.error().message << '\n';
        return 1;
    }

    clc::data::DataRegistry registry;
    auto resource_report = registry.add(clc::data::ResourceDefinition{
        .id = "grain",
        .display_name = "Grain",
        .category = "food",
        .base_value = 10,
    });

    if (!resource_report.ok()) {
        std::cerr << "Failed to register bootstrap resource\n";
        return 1;
    }

    std::cout << "City Life Core " << clc::core_version_string() << '\n';
    std::cout << "World: " << world.config().name << '\n';
    std::cout << "Tick: " << world.time().current_tick() << '\n';
    std::cout << "Events: " << world.event_log().size() << '\n';
    std::cout << "Resources: " << registry.resource_count() << '\n';

    return 0;
}
