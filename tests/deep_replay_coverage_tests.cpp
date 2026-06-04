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
    const auto missing = clc::sim::make_deep_replay_coverage_report(
        clc::sim::make_standard_deep_replay_coverage_baseline(false)
    );
    require(!missing.ready, "uncovered baseline should not be ready");
    require(missing.missing_save_load_count > 0, "uncovered baseline should count missing save/load");
    require(clc::sim::deep_replay_coverage_digest(missing).find("ready=no") != std::string::npos, "missing digest should report not ready");

    const auto ready = clc::sim::make_deep_replay_coverage_report(
        clc::sim::make_standard_deep_replay_coverage_baseline(true)
    );
    require(ready.ready, "covered baseline should be ready");
    require(ready.ready_system_count == ready.system_count, "all covered systems should be ready");
    require(clc::sim::deep_replay_coverage_markdown(ready).find("Ready: yes") != std::string::npos, "ready markdown should report ready");

    return 0;
}
