#include "clc/sim/SimulationPersistence.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

std::string max_u64() {
    return std::to_string(std::numeric_limits<std::uint64_t>::max());
}

} // namespace

int main() {
    const auto settlement_overflow = std::string{}
        + "CLC_SIM_WORLD_STATE\t1\n"
        + "day\t0\n"
        + "time\t0\n"
        + "wallet\t0\n"
        + "settlement\triverwatch\tRiverwatch\t10\n"
        + "settlement_storage\triverwatch\tgrain\t" + max_u64() + "\n"
        + "settlement_storage\triverwatch\tgrain\t1\n";

    const auto settlement_result = clc::sim::deserialize_simulation_world_state(settlement_overflow);
    require(!settlement_result.validation.ok(), "deserialization should reject settlement storage overflow rows");

    const auto caravan_overflow = std::string{}
        + "CLC_SIM_WORLD_STATE\t1\n"
        + "day\t0\n"
        + "time\t0\n"
        + "wallet\t0\n"
        + "caravan\tc1\tCaravan\tr1\ta\tb\t1\t1\t86400\t86400\n"
        + "caravan_cargo\tc1\tgrain\t" + max_u64() + "\n"
        + "caravan_cargo\tc1\tgrain\t1\n";

    const auto caravan_result = clc::sim::deserialize_simulation_world_state(caravan_overflow);
    require(!caravan_result.validation.ok(), "deserialization should reject caravan cargo overflow rows");

    return 0;
}
