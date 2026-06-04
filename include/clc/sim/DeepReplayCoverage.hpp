#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct DeepReplaySystemCoverage final {
    std::string system_id{};
    bool save_load_covered{false};
    bool replay_covered{false};
    bool migration_covered{false};
    bool golden_fixture_available{false};
    bool invalid_fixture_available{false};
    bool diagnostics_available{false};
};

struct DeepReplayCoverageReport final {
    bool ready{false};
    std::uint64_t system_count{0};
    std::uint64_t ready_system_count{0};
    std::uint64_t missing_save_load_count{0};
    std::uint64_t missing_replay_count{0};
    std::uint64_t missing_fixture_count{0};
    std::vector<DeepReplaySystemCoverage> systems{};
    data::ValidationReport validation{};
};

[[nodiscard]] data::ValidationReport validate_deep_replay_system_coverage(const DeepReplaySystemCoverage& coverage);
[[nodiscard]] DeepReplayCoverageReport make_deep_replay_coverage_report(std::vector<DeepReplaySystemCoverage> systems);
[[nodiscard]] std::vector<DeepReplaySystemCoverage> make_standard_deep_replay_coverage_baseline(bool covered);
[[nodiscard]] std::string deep_replay_coverage_digest(const DeepReplayCoverageReport& report);
[[nodiscard]] std::string deep_replay_coverage_markdown(const DeepReplayCoverageReport& report);

} // namespace clc::sim
