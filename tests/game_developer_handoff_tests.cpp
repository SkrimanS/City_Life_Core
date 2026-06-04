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
    const auto native = clc::sim::make_game_developer_handoff_report("native_cpp_game");
    require(native.found, "native C++ profile should be found");
    require(native.ready_for_game_team, "native C++ profile should be ready for game team");
    require(!native.recommended_examples.empty(), "native handoff should include examples");
    require(!native.required_documents.empty(), "native handoff should include docs");
    require(clc::sim::game_developer_handoff_digest(native).find("ready=yes") != std::string::npos, "native digest should report ready");

    const auto backend = clc::sim::make_game_developer_handoff_report("backend_service");
    require(backend.found, "backend profile should be found");
    require(backend.ready_for_game_team, "backend profile should be ready for game team");
    require(!backend.handoff_warnings.empty(), "backend handoff should warn about external server responsibilities");

    const auto mmo = clc::sim::make_game_developer_handoff_report("mmo_server_authoritative");
    require(mmo.found, "MMO profile should be found");
    require(!mmo.ready_for_game_team, "planned MMO profile should not be marked ready");
    require(!mmo.handoff_warnings.empty(), "planned MMO handoff should include warnings");

    const auto missing = clc::sim::make_game_developer_handoff_report("missing_profile");
    require(!missing.found, "missing profile should not be found");
    require(clc::sim::game_developer_handoff_digest(missing).find("found=no") != std::string::npos, "missing digest should report not found");

    const auto markdown = clc::sim::game_developer_handoff_markdown(native);
    require(markdown.find("examples/game_profiles.cpp") != std::string::npos, "handoff markdown should include examples");

    return 0;
}
