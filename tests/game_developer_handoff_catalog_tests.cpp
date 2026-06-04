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
    const auto report = clc::sim::make_standard_game_developer_handoff_catalog_report();
    require(report.ready, "standard handoff catalog should be ready");
    require(report.profile_count > 0, "catalog should include profiles");
    require(report.ready_profile_count > 0, "catalog should include ready profiles");
    require(report.starter_example_count >= report.ready_profile_count, "ready profiles should have starter examples");
    require(report.document_count >= report.ready_profile_count, "ready profiles should have docs");
    require(clc::sim::game_developer_handoff_catalog_digest(report).find("ready=yes") != std::string::npos, "digest should report ready");
    require(clc::sim::game_developer_handoff_catalog_markdown(report).find("Game Developer Handoff Catalog") != std::string::npos, "markdown should include heading");

    const auto missing_manifest = clc::sim::make_game_developer_handoff_catalog_report(clc::sim::make_sdk_handoff_manifest_report({
        .sdk_version = "4.0.0",
        .c_interface_version = 8,
        .artifacts = clc::sim::make_standard_sdk_handoff_artifacts(false),
    }));
    require(!missing_manifest.ready, "missing SDK manifest should block catalog readiness");

    return 0;
}
