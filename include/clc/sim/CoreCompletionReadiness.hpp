#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/DeepReplayCoverage.hpp"
#include "clc/sim/PlatformReadiness.hpp"
#include "clc/sim/RuntimeCoreSystems.hpp"
#include "clc/sim/SdkHandoffManifest.hpp"
#include "clc/sim/SimulationProcessor.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

enum class CoreCompletionStatus {
    missing,
    partial,
    ready,
};

struct CoreCompletionArea final {
    std::string area_id{};
    std::string title{};
    CoreCompletionStatus status{CoreCompletionStatus::missing};
    std::uint64_t completed_items{0};
    std::uint64_t total_items{0};
    std::vector<std::string> remaining_work{};
};

struct CoreCompletionReadinessInput final {
    bool architecture_ready{false};
    bool deep_systems_ready{false};
    bool runtime_integration_ready{false};
    bool save_load_replay_ready{false};
    bool authority_ready{false};
    bool regional_ready{false};
    bool c_abi_ready{false};
    bool developer_handoff_ready{false};
    bool packaging_ready{false};
};

struct CoreCompletionReadinessReport final {
    bool ready{false};
    std::uint64_t ready_area_count{0};
    std::uint64_t partial_area_count{0};
    std::uint64_t missing_area_count{0};
    std::vector<CoreCompletionArea> areas{};
    data::ValidationReport validation{};
};

struct CoreCompletionEvidence final {
    DeepSimulationCoreReadinessReport processor{};
    PlatformReadinessReport platform{};
    DeepReplayCoverageReport replay_coverage{};
    RuntimeCoreSystemsReport runtime_core_systems{};
    SdkHandoffManifestReport sdk_handoff_manifest{};
    bool deep_domain_reports_available{false};
    bool regional_reports_available{false};
    bool runtime_workflow_available{false};
    bool save_replay_available{false};
    bool authority_available{false};
    bool c_abi_handoff_available{false};
    bool developer_handoff_available{false};
    bool package_validation_available{false};
};

[[nodiscard]] std::string_view core_completion_status_name(CoreCompletionStatus status) noexcept;
[[nodiscard]] CoreCompletionReadinessInput make_core_completion_readiness_input(const CoreCompletionEvidence& evidence);
[[nodiscard]] CoreCompletionReadinessReport make_core_completion_readiness_report(const CoreCompletionReadinessInput& input);
[[nodiscard]] CoreCompletionReadinessReport make_core_completion_readiness_report(const CoreCompletionEvidence& evidence);
[[nodiscard]] std::string core_completion_readiness_digest(const CoreCompletionReadinessReport& report);
[[nodiscard]] std::string core_completion_readiness_markdown(const CoreCompletionReadinessReport& report);

} // namespace clc::sim
