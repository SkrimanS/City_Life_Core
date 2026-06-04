#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/SimulationPersistenceReplay.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct SaveReplayFixture final {
    std::string system_id{};
    std::string schema_id{};
    std::uint32_t schema_version{0};
    bool regional{false};
    std::string golden_save_payload{};
    std::string invalid_save_payload{};
    std::string replay_digest{};
};

struct SaveReplayFixtureCatalogReport final {
    bool ready{false};
    std::uint64_t fixture_count{0};
    std::uint64_t ready_fixture_count{0};
    std::uint64_t regional_fixture_count{0};
    std::uint64_t invalid_fixture_count{0};
    std::vector<SaveReplayFixture> fixtures{};
    data::ValidationReport validation{};
};

[[nodiscard]] std::vector<SaveReplayFixture> make_standard_save_replay_fixtures();
[[nodiscard]] data::ValidationReport validate_save_replay_fixture(const SaveReplayFixture& fixture);
[[nodiscard]] SaveReplayFixtureCatalogReport make_save_replay_fixture_catalog_report(std::vector<SaveReplayFixture> fixtures);
[[nodiscard]] std::string save_replay_fixture_catalog_digest(const SaveReplayFixtureCatalogReport& report);
[[nodiscard]] std::string save_replay_fixture_catalog_markdown(const SaveReplayFixtureCatalogReport& report);

} // namespace clc::sim
