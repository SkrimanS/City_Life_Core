#pragma once

#include "clc/sim/Contracts.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <string>
#include <string_view>

namespace clc::sim {

struct RuntimeCaravanCreationResult final {
    std::string caravan_id{};
    bool created{false};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return created && validation.ok();
    }
};

struct RuntimeCaravanAdvanceResult final {
    CaravanAdvanceReport report{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

[[nodiscard]] RuntimeCaravanCreationResult create_runtime_caravan_for_route(
    SimulationRuntime& runtime,
    std::string_view route_id,
    std::string caravan_id,
    std::string display_name,
    ResourceStorage cargo = {}
);

[[nodiscard]] RuntimeCaravanAdvanceResult advance_runtime_caravan_day(
    SimulationRuntime& runtime,
    std::string_view caravan_id
);

[[nodiscard]] ContractFulfillmentResult fulfill_runtime_contract_from_arrived_caravan_with_reward_and_ledger(
    SimulationRuntime& runtime,
    std::string_view contract_id,
    std::string_view caravan_id
);

[[nodiscard]] ContractFulfillmentResult fulfill_runtime_contract_from_owned_arrived_caravan_with_reward_and_ledger(
    SimulationRuntime& runtime,
    std::string_view contract_id,
    std::string_view caravan_id,
    std::string_view expected_faction_id
);

} // namespace clc::sim
