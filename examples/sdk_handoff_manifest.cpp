#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    const auto manifest = clc::sim::make_sdk_handoff_manifest_report({
        .sdk_version = "4.0.0",
        .c_interface_version = 8,
        .artifacts = clc::sim::make_standard_sdk_handoff_artifacts(true),
    });

    std::cout << clc::sim::sdk_handoff_manifest_digest(manifest) << '\n';
    std::cout << clc::sim::sdk_handoff_manifest_markdown(manifest) << '\n';
    return manifest.ready ? EXIT_SUCCESS : 2;
}
