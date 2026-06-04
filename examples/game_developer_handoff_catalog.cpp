#include "clc/CityLifeCore.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    const auto report = clc::sim::make_standard_game_developer_handoff_catalog_report();
    std::cout << clc::sim::game_developer_handoff_catalog_digest(report) << '\n';
    std::cout << clc::sim::game_developer_handoff_catalog_markdown(report) << '\n';
    return report.ready ? EXIT_SUCCESS : 2;
}
