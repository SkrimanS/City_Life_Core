#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/SimulationEngine.hpp"

#include <filesystem>
#include <string>
#include <string_view>

namespace clc::sim {

struct SimulationSnapshotLoadResult final {
    SimulationSnapshot snapshot{};
    data::ValidationReport validation{};

    [[nodiscard]] bool ok() const noexcept {
        return validation.ok();
    }
};

[[nodiscard]] std::string serialize_simulation_snapshot(const SimulationSnapshot& snapshot);
[[nodiscard]] SimulationSnapshotLoadResult deserialize_simulation_snapshot(std::string_view content);

[[nodiscard]] data::ValidationReport save_simulation_snapshot_to_file(
    const SimulationSnapshot& snapshot,
    const std::filesystem::path& path
);

[[nodiscard]] SimulationSnapshotLoadResult load_simulation_snapshot_from_file(const std::filesystem::path& path);

} // namespace clc::sim
