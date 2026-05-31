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

bool contains_message(const clc::data::ValidationReport& report, std::string_view needle) {
    for (const auto& message : report.messages()) {
        if (message.message.find(needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // namespace

int main() {
    clc::sim::SimulationWorldState state;
    state.engine.settlements.push_back(clc::sim::SettlementState{
        .id = "riverwatch",
        .display_name = "Riverwatch",
        .population = 10,
    });

    require(clc::sim::validate_simulation_world_state(state).ok(), "minimal settlement world state should validate");

    auto empty_settlement_storage = state;
    require(!empty_settlement_storage.engine.settlements[0].storage.add("", 1).ok(), "storage API rejects empty resource id directly");

    auto zero_settlement_storage = state;
    zero_settlement_storage.engine.settlements[0].storage = clc::sim::ResourceStorage{};
    const auto zero_settlement_report = zero_settlement_storage.engine.settlements[0].storage.add("grain", 0);
    require(!zero_settlement_report.ok(), "storage API rejects zero amount directly");

    const auto settlement_zero_save = clc::sim::deserialize_simulation_world_state(
        "CLC_SIM_WORLD_STATE\t1\n"
        "day\t0\n"
        "time\t0\n"
        "wallet\t0\n"
        "settlement\triverwatch\tRiverwatch\t10\n"
        "settlement_storage\triverwatch\tgrain\t0\n"
    );
    require(!settlement_zero_save.ok(), "deserialized settlement storage zero amount should fail");
    require(contains_message(settlement_zero_save.validation, "amount"), "settlement zero amount diagnostic should mention amount");

    const auto caravan_zero_save = clc::sim::deserialize_simulation_world_state(
        "CLC_SIM_WORLD_STATE\t1\n"
        "day\t0\n"
        "time\t0\n"
        "wallet\t0\n"
        "caravan\tc1\tCaravan\tr1\ta\tb\t1\t1\t86400\t86400\n"
        "caravan_cargo\tc1\tgrain\t0\n"
    );
    require(!caravan_zero_save.ok(), "deserialized caravan cargo zero amount should fail");
    require(contains_message(caravan_zero_save.validation, "amount"), "caravan zero amount diagnostic should mention amount");

    return 0;
}
