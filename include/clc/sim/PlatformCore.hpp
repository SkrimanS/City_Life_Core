#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

enum class PlatformModuleKind {
    core,
    optional,
    genre,
};

struct PlatformModuleDescriptor final {
    std::string module_id{};
    PlatformModuleKind kind{PlatformModuleKind::core};
    std::vector<std::string> dependency_module_ids{};
    std::vector<std::string> compatible_profile_ids{};
    bool enabled_by_default{true};
};

struct GenreProfileDescriptor final {
    std::string profile_id{};
    std::vector<std::string> enabled_module_ids{};
    std::vector<std::string> default_rule_ids{};
    std::vector<std::string> expected_data_templates{};
    std::vector<std::string> recommended_action_types{};
    std::vector<std::string> recommended_report_ids{};
};

struct ContentPackManifest final {
    std::string content_pack_id{};
    std::uint32_t schema_version{1};
    std::vector<std::string> required_module_ids{};
    std::vector<std::string> compatible_profile_ids{};
    std::vector<std::string> migration_ids{};
};

struct PlatformCoreRegistry final {
    std::vector<PlatformModuleDescriptor> modules{};
    std::vector<GenreProfileDescriptor> genre_profiles{};
};

struct PlatformCoreQueryReport final {
    std::uint64_t module_count{0};
    std::uint64_t core_module_count{0};
    std::uint64_t optional_module_count{0};
    std::uint64_t genre_module_count{0};
    std::uint64_t genre_profile_count{0};
    std::uint64_t content_template_count{0};
    std::uint64_t recommended_report_count{0};
    data::ValidationReport validation{};
};

struct PlatformDiagnosticsReport final {
    bool ready{false};
    PlatformCoreQueryReport query{};
    data::ValidationReport content_pipeline_validation{};
    std::uint64_t runtime_diagnostic_count{0};
    std::uint64_t economy_diagnostic_count{0};
    std::uint64_t action_diagnostic_count{0};
    std::uint64_t replay_diagnostic_count{0};
    std::uint64_t migration_diagnostic_count{0};
};

[[nodiscard]] std::string_view platform_module_kind_name(PlatformModuleKind kind) noexcept;
[[nodiscard]] PlatformCoreRegistry make_standard_platform_core_registry();
[[nodiscard]] data::ValidationReport validate_platform_core_registry(const PlatformCoreRegistry& registry);
[[nodiscard]] data::ValidationReport validate_content_pack_manifest(
    const ContentPackManifest& manifest,
    const PlatformCoreRegistry& registry
);
[[nodiscard]] PlatformCoreQueryReport query_platform_core_registry(const PlatformCoreRegistry& registry);
[[nodiscard]] PlatformDiagnosticsReport make_platform_diagnostics_report(
    const PlatformCoreRegistry& registry,
    const ContentPackManifest& manifest
);
[[nodiscard]] ContentPackManifest make_standard_platform_content_pack_manifest();
[[nodiscard]] std::string platform_core_query_digest(const PlatformCoreQueryReport& report);
[[nodiscard]] std::string platform_diagnostics_digest(const PlatformDiagnosticsReport& report);
[[nodiscard]] std::string platform_diagnostics_markdown(const PlatformDiagnosticsReport& report);

} // namespace clc::sim
