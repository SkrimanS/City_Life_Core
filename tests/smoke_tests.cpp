#include "clc/core/Version.hpp"
#include "clc/core/World.hpp"

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
    require(clc::core_version_string() == std::string_view{"0.1.0"}, "version string should be 0.1.0");

    clc::World world{clc::WorldConfig{.name = "Smoke Test World", .seed = 42}};
    require(world.time().current_tick() == 0, "new world should start at tick 0");
    require(world.event_log().size() == 1, "world creation should emit one event");

    const auto zero_tick_result = world.advance(0);
    require(!zero_tick_result, "advancing by zero ticks should fail");

    const auto advance_result = world.advance(5);
    require(advance_result.ok(), "advancing by positive ticks should succeed");
    require(world.time().current_tick() == 5, "world should advance to tick 5");
    require(world.event_log().size() == 2, "world advance should emit one event");

    return 0;
}
