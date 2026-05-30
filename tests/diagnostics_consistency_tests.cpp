#include "clc/data/DataPackLoader.hpp"
#include "clc/sim/ActionBridge.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

void require_diagnostics_are_useful(const clc::data::ValidationReport& report, std::string_view message) {
    require(!report.ok(), message);
    require(!report.messages().empty(), "diagnostic report should contain messages");
    for (const auto& diagnostic : report.messages()) {
        require(diagnostic.severity == clc::data::ValidationSeverity::error || diagnostic.severity == clc::data::ValidationSeverity::warning, "diagnostic should have a known severity");
        require(!diagnostic.path.empty(), "diagnostic path should not be empty");
        require(!diagnostic.message.empty(), "diagnostic message should not be empty");
    }
}

} // namespace

int main() {
    {
        clc::data::DataRegistry registry;
        clc::data::DataPackLoader loader;
        const auto report = loader.load_string("diagnostics-consistency", R"CLC(
schema_version=0.2.2

[resource]
id=bad_resource
display_name=Bad Resource
category=test
base_value=0
)CLC", registry);

        require_diagnostics_are_useful(report, "invalid data pack should produce useful diagnostics");
    }

    {
        auto bootstrap = clc::sim::make_basic_runtime_scenario();
        require(bootstrap.ok(), "scenario bootstrap failed");
        auto& engine = bootstrap.runtime.engine;

        const auto rejected = clc::sim::dispatch_runtime_action_json(
            engine,
            R"({"action_id":"diagnostics-action","type":"remove_resource","payload":{"target_id":"riverwatch","resource_id":"grain","amount":999999}})"
        );

        require(!rejected.accepted, "runtime-rejected action should not be accepted");
        require(rejected.validation_status == clc::sim::runtime_action_status_rejected, "runtime-rejected action should use rejected status");
        require(rejected.error_code == clc::sim::runtime_action_error_action_rejected, "runtime-rejected action should use action_rejected error code");
        require_diagnostics_are_useful(rejected.validation, "runtime-rejected action should provide useful diagnostics");

        const auto result_json = clc::sim::runtime_action_result_to_json(rejected);
        require(result_json.find("\"diagnostics_detail\":[") != std::string::npos, "result JSON should include diagnostics detail");
        require(result_json.find("\"severity\":\"error\"") != std::string::npos, "result JSON should include diagnostic severity");
        require(result_json.find("\"path\":") != std::string::npos, "result JSON should include diagnostic path");
        require(result_json.find("\"message\":") != std::string::npos, "result JSON should include diagnostic message");
    }

    return 0;
}
