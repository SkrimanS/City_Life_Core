#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    const auto partial = clc::sim::make_core_completion_readiness_report({
        .architecture_ready = true,
        .deep_systems_ready = false,
        .runtime_integration_ready = false,
        .save_load_replay_ready = false,
        .authority_ready = true,
        .regional_ready = false,
        .c_abi_ready = false,
        .developer_handoff_ready = true,
        .packaging_ready = false,
    });
    require(!partial.ready, "partial readiness should not be final-ready");
    require(partial.partial_area_count > 0, "partial report should count partial areas");
    require(clc::sim::core_completion_readiness_digest(partial).find("ready=no") != std::string::npos, "partial digest should report not ready");

    const auto ready = clc::sim::make_core_completion_readiness_report({
        .architecture_ready = true,
        .deep_systems_ready = true,
        .runtime_integration_ready = true,
        .save_load_replay_ready = true,
        .authority_ready = true,
        .regional_ready = true,
        .c_abi_ready = true,
        .developer_handoff_ready = true,
        .packaging_ready = true,
    });
    require(ready.ready, "all-ready input should be final-ready");
    require(ready.ready_area_count == ready.areas.size(), "all areas should be ready");
    require(clc::sim::core_completion_readiness_markdown(ready).find("Ready: yes") != std::string::npos, "ready markdown should report ready");

    clc::sim::SimulationProcessorRegistry processors;
    require(clc::sim::add_simulation_feature(processors, {.feature_id = "runtime", .enabled = true}).ok(), "feature should add");
    require(clc::sim::add_simulation_dependency(processors, {
        .dependency_id = "runtime.deep",
        .source_system_id = "runtime",
        .target_system_id = "deep",
        .enabled = true,
    }).ok(), "dependency should add");
    require(clc::sim::add_simulation_processor(processors, {
        .processor_id = "deep",
        .deterministic_order = 10,
        .required_feature_ids = {"runtime"},
        .required_dependency_ids = {"runtime.deep"},
    }).ok(), "processor should add");
    const auto processor_readiness = clc::sim::make_deep_simulation_core_readiness_report(processors);
    const auto standard_processors = clc::sim::make_standard_core_processor_registry();
    const auto standard_readiness = clc::sim::make_deep_simulation_core_readiness_report(standard_processors);
    require(standard_readiness.processor_architecture_ready, "standard processor registry should be architecture-ready");
    require(standard_readiness.processor_count >= 20, "standard processor registry should cover major core systems");
    require(clc::sim::enabled_simulation_processors_in_order(standard_processors).size() == standard_readiness.processor_count, "standard processors should all be enabled");
    const auto platform = clc::sim::make_platform_readiness_report({
        .module_boundaries_documented = true,
        .profile_requirements_documented = true,
        .content_pipeline_requirements_documented = true,
        .editor_admin_api_requirements_documented = true,
        .diagnostics_api_requirements_documented = true,
        .cxx_api_ready = true,
        .c_abi_readonly_diagnostics_ready = true,
        .save_load_migration_ready = true,
        .replay_migration_ready = true,
        .regional_large_world_ready = true,
        .supported_profile_ids = {"native_cpp_game"},
    });
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "basic runtime should bootstrap for evidence");
    const auto evidence_report = clc::sim::make_core_completion_readiness_report({
        .processor = processor_readiness,
        .platform = platform,
        .replay_coverage = clc::sim::make_deep_replay_coverage_report(clc::sim::make_standard_deep_replay_coverage_baseline(true)),
        .runtime_core_systems = clc::sim::evaluate_runtime_core_systems(bootstrap.runtime),
        .sdk_handoff_manifest = clc::sim::make_sdk_handoff_manifest_report({
            .sdk_version = "4.0.0",
            .c_interface_version = 8,
            .artifacts = clc::sim::make_standard_sdk_handoff_artifacts(true),
        }),
        .deep_domain_reports_available = true,
        .regional_reports_available = true,
        .runtime_workflow_available = true,
        .save_replay_available = true,
        .authority_available = true,
        .c_abi_handoff_available = true,
        .developer_handoff_available = true,
        .package_validation_available = true,
    });
    require(evidence_report.ready, "complete evidence should produce ready report");

    return 0;
}
