#include "clc/sim/SimulationPersistence.hpp"

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

clc::sim::SimulationWorldState make_world_state_with_remainder(clc::sim::SettlementTickRemainder remainder) {
    clc::sim::SimulationWorldState state{};
    clc::sim::SettlementState settlement{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .population = 120,
    };
    settlement.tick_remainders.push_back(std::move(remainder));
    state.engine.settlements.push_back(std::move(settlement));
    return state;
}

} // namespace

int main() {
    auto valid_state = make_world_state_with_remainder(clc::sim::SettlementTickRemainder{
        .key = "food:grain",
        .numerator = clc::hours_to_ticks(3),
    });
    require(clc::sim::validate_simulation_world_state(valid_state).ok(), "valid settlement tick remainder should validate");

    auto empty_key_state = make_world_state_with_remainder(clc::sim::SettlementTickRemainder{
        .key = "",
        .numerator = clc::hours_to_ticks(3),
    });
    require(!clc::sim::validate_simulation_world_state(empty_key_state).ok(), "empty settlement tick remainder key should fail validation");

    auto too_large_state = make_world_state_with_remainder(clc::sim::SettlementTickRemainder{
        .key = "food:grain",
        .numerator = clc::ticks_per_day(),
    });
    require(!clc::sim::validate_simulation_world_state(too_large_state).ok(), "settlement tick remainder numerator >= ticks_per_day should fail validation");

    auto duplicate_state = make_world_state_with_remainder(clc::sim::SettlementTickRemainder{
        .key = "food:grain",
        .numerator = clc::hours_to_ticks(1),
    });
    duplicate_state.engine.settlements[0].tick_remainders.push_back(clc::sim::SettlementTickRemainder{
        .key = "food:grain",
        .numerator = clc::hours_to_ticks(2),
    });
    require(!clc::sim::validate_simulation_world_state(duplicate_state).ok(), "duplicate settlement tick remainder key should fail validation");

    return 0;
}
