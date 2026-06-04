#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct SimulationFeatureToggle final {
    std::string feature_id{};
    bool enabled{true};
};

struct SimulationDependencyToggle final {
    std::string dependency_id{};
    std::string source_system_id{};
    std::string target_system_id{};
    bool enabled{true};
};

struct SimulationProcessorDescriptor final {
    std::string processor_id{};
    std::string display_name{};
    std::uint32_t deterministic_order{0};
    std::vector<std::string> required_feature_ids{};
    std::vector<std::string> required_dependency_ids{};
    bool enabled{true};
    bool save_load_aware{true};
    bool replay_aware{true};
    bool diagnostics_friendly{true};
    bool engine_agnostic{true};
};

struct SimulationProcessorRegistry final {
    std::vector<SimulationProcessorDescriptor> processors{};
    std::vector<SimulationFeatureToggle> features{};
    std::vector<SimulationDependencyToggle> dependencies{};
};

struct SimulationProcessorDiagnostic final {
    std::string processor_id{};
    std::string severity{"info"};
    std::string message{};
};

struct SimulationCauseEntry final {
    std::string processor_id{};
    std::string rule_id{};
    std::string dependency_id{};
    std::string target_system_id{};
    std::string cause{};
};

struct SimulationProcessorRunReport final {
    std::uint64_t enabled_processor_count{0};
    std::uint64_t skipped_processor_count{0};
    std::uint64_t disabled_dependency_count{0};
    std::vector<std::string> deterministic_order{};
    std::vector<SimulationProcessorDiagnostic> diagnostics{};
    std::vector<SimulationCauseEntry> causes{};
};

struct DeepSimulationCoreReadinessReport final {
    bool processor_architecture_ready{false};
    bool feature_toggles_ready{false};
    bool dependency_toggles_ready{false};
    bool save_load_ready{false};
    bool replay_ready{false};
    bool diagnostics_ready{false};
    bool engine_agnostic_ready{false};
    std::uint64_t processor_count{0};
    std::uint64_t issue_count{0};
    data::ValidationReport validation{};
};

[[nodiscard]] data::ValidationReport validate_simulation_feature_toggle(const SimulationFeatureToggle& feature);
[[nodiscard]] data::ValidationReport validate_simulation_dependency_toggle(const SimulationDependencyToggle& dependency);
[[nodiscard]] data::ValidationReport validate_simulation_processor_descriptor(const SimulationProcessorDescriptor& processor);
[[nodiscard]] data::ValidationReport add_simulation_feature(SimulationProcessorRegistry& registry, SimulationFeatureToggle feature);
[[nodiscard]] data::ValidationReport add_simulation_dependency(SimulationProcessorRegistry& registry, SimulationDependencyToggle dependency);
[[nodiscard]] data::ValidationReport add_simulation_processor(SimulationProcessorRegistry& registry, SimulationProcessorDescriptor processor);
[[nodiscard]] SimulationProcessorRegistry make_standard_core_processor_registry();
[[nodiscard]] bool simulation_feature_enabled(const SimulationProcessorRegistry& registry, std::string_view feature_id) noexcept;
[[nodiscard]] bool simulation_dependency_enabled(const SimulationProcessorRegistry& registry, std::string_view dependency_id) noexcept;
[[nodiscard]] std::vector<SimulationProcessorDescriptor> enabled_simulation_processors_in_order(const SimulationProcessorRegistry& registry);
[[nodiscard]] SimulationProcessorRunReport make_simulation_processor_run_report(const SimulationProcessorRegistry& registry);
[[nodiscard]] DeepSimulationCoreReadinessReport make_deep_simulation_core_readiness_report(const SimulationProcessorRegistry& registry);
[[nodiscard]] std::string simulation_processor_run_report_digest(const SimulationProcessorRunReport& report);
[[nodiscard]] std::string deep_simulation_core_readiness_digest(const DeepSimulationCoreReadinessReport& report);

} // namespace clc::sim
