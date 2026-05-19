#pragma once

#include "clc/sim/SimulationEngine.hpp"

#include <string_view>

namespace clc::sim {

[[nodiscard]] inline SimulationScenarioResult run_scenario_preset_from_catalog(
    SimulationEngine& engine,
    const SimulationScenarioPresetCatalog& catalog,
    std::string_view preset_id
) {
    const auto* preset = scenario_preset_by_id(catalog, preset_id);
    if (preset == nullptr) {
        return SimulationScenarioResult{
            .initial_snapshot = engine.snapshot(),
            .final_snapshot = engine.snapshot(),
        };
    }
    return engine.run_scenario_preset(*preset);
}

} // namespace clc::sim
