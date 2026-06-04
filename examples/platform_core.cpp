#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    const auto registry = clc::sim::make_standard_platform_core_registry();
    const auto diagnostics = clc::sim::make_platform_diagnostics_report(
        registry,
        clc::sim::make_standard_platform_content_pack_manifest()
    );

    std::cout << clc::sim::platform_diagnostics_digest(diagnostics) << '\n';
    std::cout << clc::sim::platform_diagnostics_markdown(diagnostics) << '\n';
    return diagnostics.ready ? EXIT_SUCCESS : 2;
}
