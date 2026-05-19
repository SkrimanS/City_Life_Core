#pragma once

#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace clc::sim {

struct RuntimeScenarioBootstrapConfig final {
    std::string origin_settlement_id{"riverwatch"};
    std::string destination_settlement_id{"hillford"};
    std::string origin_faction_id{"riverwatch"};
    std::string receiver_faction_id{"traders_guild"};
    std::string route_id{"riverwatch_to_hillford"};
    std::string contract_id{"grain_delivery_runtime"};
    std::string resource_id{"grain"};
    std::uint64_t origin_starting_resource{50};
    std::uint64_t contract_quantity{30};
    std::uint64_t contract_reward_coins{75};
    std::uint64_t contract_due_day{8};
    std::uint64_t route_travel_days{2};
    std::uint64_t wallet_coins{10};
};

struct RuntimeScenarioBootstrapResult final {
    SimulationRuntime runtime;
    data::ValidationReport validation{};

    explicit RuntimeScenarioBootstrapResult(data::DataRegistry registry)
        : runtime(std::move(registry)) {}

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

[[nodiscard]] data::DataRegistry make_basic_runtime_scenario_registry();

[[nodiscard]] RuntimeScenarioBootstrapResult make_basic_runtime_scenario(
    RuntimeScenarioBootstrapConfig config = {}
);

} // namespace clc::sim
