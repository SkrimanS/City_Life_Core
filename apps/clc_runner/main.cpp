#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"

#include <iostream>

int main() {
    clc::World world{clc::WorldConfig{.name = "Bootstrap World", .seed = 1}};

    const auto advanced = world.advance(1);
    if (!advanced) {
        std::cerr << "Failed to advance world: " << advanced.error().message << '\n';
        return 1;
    }

    std::cout << "City Life Core " << clc::core_version_string() << '\n';
    std::cout << "World: " << world.config().name << '\n';
    std::cout << "Tick: " << world.time().current_tick() << '\n';
    std::cout << "Events: " << world.event_log().size() << '\n';

    return 0;
}
