#pragma once

#include "clc/sim/SimulationPersistence.hpp"

#include <filesystem>

namespace clc::sim {

struct SimulationRuntimeSaveLoadValidationResult final {
    SimulationWorldStateLoadResult load{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return load.ok() && validation.ok();
    }
};

[[nodiscard]] data::ValidationReport validate_simulation_runtimes_match(
    const SimulationRuntime& expected,
    const SimulationRuntime& actual
);

[[nodiscard]] SimulationRuntimeSaveLoadValidationResult validate_simulation_runtime_save_load_roundtrip(
    const SimulationRuntime& source,
    SimulationRuntime& target,
    const std::filesystem::path& path
);

} // namespace clc::sim
