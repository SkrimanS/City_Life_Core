#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"

namespace clc::sim {

SimulationRuntimeSaveLoadValidationResult validate_simulation_runtime_save_load_roundtrip(
    const SimulationRuntime& source,
    SimulationRuntime& target,
    const std::filesystem::path& path
) {
    SimulationRuntimeSaveLoadValidationResult result{};

    auto save_report = save_simulation_runtime_to_file(source, path);
    if (!save_report.ok()) {
        result.validation = save_report;
        return result;
    }

    result.load = load_simulation_runtime_from_file(path, target);
    if (!result.load.ok()) {
        return result;
    }

    result.validation = validate_simulation_world_state(capture_simulation_world_state(target));
    return result;
}

} // namespace clc::sim
