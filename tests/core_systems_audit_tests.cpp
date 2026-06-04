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
    const auto report = clc::sim::make_standard_core_systems_audit_report();
    require(report.ready, "standard core systems audit should be ready");
    require(report.ready_area_count == report.area_count, "all standard audit areas should be ready");
    require(clc::sim::core_systems_audit_digest(report).find("ready=yes") != std::string::npos, "digest should report ready");
    require(clc::sim::core_systems_audit_markdown(report).find("Core Systems Audit") != std::string::npos, "markdown should include heading");

    auto broken = clc::sim::make_standard_core_processor_registry();
    broken.processors.front().engine_agnostic = false;
    const auto broken_report = clc::sim::make_core_systems_audit_report(broken);
    require(!broken_report.ready, "non-engine-agnostic processor should fail audit readiness");

    return 0;
}
