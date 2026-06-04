#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct PlatformReadinessInput final {
    bool module_boundaries_documented{false};
    bool profile_requirements_documented{false};
    bool content_pipeline_requirements_documented{false};
    bool editor_admin_api_requirements_documented{false};
    bool diagnostics_api_requirements_documented{false};
    bool cxx_api_ready{false};
    bool c_abi_readonly_diagnostics_ready{false};
    bool save_load_migration_ready{false};
    bool replay_migration_ready{false};
    bool regional_large_world_ready{false};
    std::vector<std::string> supported_profile_ids{};
    std::vector<std::string> known_gaps{};
};

struct PlatformReadinessReport final {
    std::string target_version{"4.0.0"};
    bool ready{false};
    std::uint64_t supported_profile_count{0};
    std::uint64_t known_gap_count{0};
    data::ValidationReport validation{};
};

[[nodiscard]] data::ValidationReport validate_platform_readiness_input(const PlatformReadinessInput& input);
[[nodiscard]] PlatformReadinessReport make_platform_readiness_report(const PlatformReadinessInput& input);
[[nodiscard]] std::string platform_readiness_digest(const PlatformReadinessReport& report);
[[nodiscard]] std::string platform_readiness_markdown(const PlatformReadinessReport& report);

} // namespace clc::sim
