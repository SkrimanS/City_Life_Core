#pragma once

#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct RuntimeCaravanTickReport final {
    std::string caravan_id{};
    CaravanAdvanceReport advance{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

struct SimulationRuntimeDayReport final {
    SimulationDayReport engine{};
    std::vector<RuntimeCaravanTickReport> caravans{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

struct SimulationRuntimeRunSummary final {
    std::uint64_t days_run{0};
    std::uint64_t first_day{0};
    std::uint64_t last_day{0};
    std::uint64_t caravan_ticks{0};
    std::uint64_t caravan_arrivals{0};
    std::uint64_t warnings{0};
};

struct SimulationRuntimeRunResult final {
    std::vector<SimulationRuntimeDayReport> reports{};
    SimulationRuntimeRunSummary summary{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

struct SimulationRuntimeRunUntilArrivalResult final {
    SimulationRuntimeRunResult run{};
    bool arrival_reached{false};
    std::string arrived_caravan_id{};
    std::uint64_t arrival_day{0};

    [[nodiscard]] bool ok() const noexcept {
        return run.ok();
    }
};

struct SimulationRuntimeArrivalContractResult final {
    SimulationRuntimeRunUntilArrivalResult arrival{};
    ContractFulfillmentResult fulfillment{};

    [[nodiscard]] bool ok() const noexcept {
        return arrival.ok() && arrival.arrival_reached && fulfillment.ok();
    }
};

[[nodiscard]] SimulationRuntimeDayReport advance_runtime_day(SimulationRuntime& runtime);
[[nodiscard]] SimulationRuntimeRunSummary summarize_runtime_day_reports(const std::vector<SimulationRuntimeDayReport>& reports);
[[nodiscard]] SimulationRuntimeRunResult run_runtime_days(SimulationRuntime& runtime, std::uint64_t day_count);
[[nodiscard]] SimulationRuntimeRunUntilArrivalResult run_runtime_until_first_caravan_arrival(
    SimulationRuntime& runtime,
    std::uint64_t max_days
);
[[nodiscard]] SimulationRuntimeArrivalContractResult run_runtime_until_first_caravan_arrival_and_fulfill_contract(
    SimulationRuntime& runtime,
    std::uint64_t max_days,
    std::string_view expected_faction_id
);

} // namespace clc::sim
