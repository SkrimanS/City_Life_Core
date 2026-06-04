#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    const auto report = clc::sim::make_standard_core_systems_audit_report();
    std::cout << clc::sim::core_systems_audit_digest(report) << '\n';
    std::cout << clc::sim::core_systems_audit_markdown(report) << '\n';
    return report.ready ? EXIT_SUCCESS : 2;
}
