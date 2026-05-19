#pragma once

#include "clc/sim/SimulationEngine.hpp"

#include <string>
#include <string_view>

namespace clc::sim {

[[nodiscard]] inline std::string scenario_catalog_digest(const SimulationScenarioPresetCatalog& catalog) {
    return "scenario catalog presets=" + std::to_string(scenario_preset_count(catalog));
}

[[nodiscard]] inline std::string scenario_catalog_preset_lookup_digest(
    const SimulationScenarioPresetCatalog& catalog,
    std::string_view preset_id
) {
    const auto* preset = scenario_preset_by_id(catalog, preset_id);
    std::string digest = "catalog preset id=" + std::string{preset_id};
    if (preset == nullptr) {
        digest += " status=missing";
        return digest;
    }

    digest += " status=found";
    digest += " name=" + preset->display_name;
    digest += " days=" + std::to_string(preset->day_count);
    return digest;
}

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
