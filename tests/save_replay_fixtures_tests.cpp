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
    const auto fixtures = clc::sim::make_standard_save_replay_fixtures();
    require(fixtures.size() >= 20, "standard fixture catalog should cover deep and regional systems");

    const auto report = clc::sim::make_save_replay_fixture_catalog_report(fixtures);
    require(report.ready, "standard fixture catalog should be ready");
    require(report.ready_fixture_count == report.fixture_count, "all standard fixtures should be ready");
    require(report.regional_fixture_count > 0, "regional fixtures should be present");
    require(report.invalid_fixture_count == report.fixture_count, "every fixture should include a rejected invalid payload");
    require(clc::sim::save_replay_fixture_catalog_digest(report).find("ready=yes") != std::string::npos, "digest should report readiness");
    require(clc::sim::save_replay_fixture_catalog_markdown(report).find("Save Replay Fixtures") != std::string::npos, "markdown should include heading");

    auto broken = fixtures.front();
    broken.golden_save_payload = "broken\n";
    require(!clc::sim::validate_save_replay_fixture(broken).ok(), "broken golden payload should be rejected");

    return 0;
}
