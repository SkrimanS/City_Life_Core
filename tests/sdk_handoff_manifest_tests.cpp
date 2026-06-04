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
    const auto missing = clc::sim::make_sdk_handoff_manifest_report({
        .sdk_version = "4.0.0",
        .c_interface_version = 8,
        .artifacts = clc::sim::make_standard_sdk_handoff_artifacts(false),
    });
    require(!missing.ready, "missing required artifacts should not be ready");
    require(missing.missing_required_artifact_count > 0, "missing report should count required gaps");
    require(missing.validation.error_count() == missing.missing_required_artifact_count, "required gaps should be validation errors");
    require(clc::sim::sdk_handoff_manifest_digest(missing).find("ready=no") != std::string::npos, "missing digest should report not ready");

    const auto ready = clc::sim::make_sdk_handoff_manifest_report({
        .sdk_version = "4.0.0",
        .c_interface_version = 8,
        .artifacts = clc::sim::make_standard_sdk_handoff_artifacts(true),
    });
    require(ready.ready, "standard present artifacts should be ready");
    require(ready.present_required_artifact_count == ready.required_artifact_count, "all required artifacts should be present");
    require(clc::sim::sdk_handoff_manifest_digest(ready).find("ready=yes") != std::string::npos, "ready digest should report ready");
    require(clc::sim::sdk_handoff_manifest_markdown(ready).find("SDK version: 4.0.0") != std::string::npos, "markdown should include SDK version");

    return 0;
}
