#pragma once

#include "clc/sim/SimulationRuntimeWorkflow.hpp"

#include <string>
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

[[nodiscard]] SimulationRuntimeDayReport advance_runtime_day(SimulationRuntime& runtime);

} // namespace clc::sim
