#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/MultiplayerLoadDiagnostics.hpp"

#include <cstdint>
#include <string>

namespace clc::sim {

struct Pre3ReadinessReport final {
    bool snapshot_model_ready{false};
    bool authority_model_ready{false};
    bool replay_model_ready{false};
    bool economy_safety_ready{false};
    bool load_diagnostics_ready{false};
    bool large_world_plan_ready{false};
    std::uint64_t ready_count{0};
    std::uint64_t missing_count{0};
    std::string status{};
};

[[nodiscard]] Pre3ReadinessReport make_pre3_readiness_report(
    bool snapshot_model_ready,
    bool authority_model_ready,
    bool replay_model_ready,
    bool economy_safety_ready,
    const MultiplayerLoadSnapshot& load_snapshot,
    bool large_world_plan_ready
);
[[nodiscard]] data::ValidationReport validate_pre3_readiness_report(const Pre3ReadinessReport& report);
[[nodiscard]] std::string pre3_readiness_report_digest(const Pre3ReadinessReport& report);

} // namespace clc::sim
