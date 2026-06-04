#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    const auto processors = clc::sim::make_standard_core_processor_registry();

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
        .supported_profile_ids = {"native_cpp_game", "backend_service"},
    });
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    const auto runtime_report = bootstrap.ok()
        ? clc::sim::evaluate_runtime_core_systems(bootstrap.runtime)
        : clc::sim::RuntimeCoreSystemsReport{};

    const auto current = clc::sim::make_core_completion_readiness_report({
        .processor = clc::sim::make_deep_simulation_core_readiness_report(processors),
        .platform = platform,
        .runtime_core_systems = runtime_report,
        .deep_domain_reports_available = true,
        .regional_reports_available = true,
        .runtime_workflow_available = false,
        .save_replay_available = false,
        .authority_available = true,
        .c_abi_handoff_available = true,
        .developer_handoff_available = true,
        .package_validation_available = false,
    });

    std::cout << clc::sim::core_completion_readiness_digest(current) << '\n';
    std::cout << clc::sim::core_completion_readiness_markdown(current) << '\n';
    return current.ready ? EXIT_SUCCESS : 2;
}
