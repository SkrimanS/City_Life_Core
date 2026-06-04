#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    const auto report = clc::sim::make_save_replay_fixture_catalog_report(
        clc::sim::make_standard_save_replay_fixtures()
    );

    std::cout << clc::sim::save_replay_fixture_catalog_digest(report) << '\n';
    std::cout << clc::sim::save_replay_fixture_catalog_markdown(report) << '\n';
    return report.ready ? EXIT_SUCCESS : 2;
}
