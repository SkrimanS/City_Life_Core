#pragma once

#include "clc/data/Validation.hpp"
#include "clc/core/EventLog.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct WorldRegionDescriptor final {
    std::string region_id{};
    std::string display_name{};
    std::string shard_id{};
    std::string partition_id{};
};

struct LargeWorldRegionAssignment final {
    std::string object_id{};
    std::string region_id{};
};

struct RegionalFeatureConfig final {
    std::string region_id{};
    std::vector<std::string> enabled_feature_ids{};
    std::vector<std::string> disabled_feature_ids{};
};

struct RegionalDependencyConfig final {
    std::string region_id{};
    std::string dependency_id{};
    std::string source_system_id{};
    std::string target_system_id{};
    bool enabled{true};
};

struct CrossRegionReference final {
    std::string reference_id{};
    std::string source_region_id{};
    std::string target_region_id{};
    std::string reference_type{};
    std::string object_id{};
};

struct LargeWorldCatalog final {
    std::vector<WorldRegionDescriptor> regions{};
    std::vector<LargeWorldRegionAssignment> settlements{};
    std::vector<LargeWorldRegionAssignment> routes{};
    std::vector<LargeWorldRegionAssignment> caravans{};
    std::vector<RegionalFeatureConfig> regional_features{};
    std::vector<RegionalDependencyConfig> regional_dependencies{};
    std::vector<CrossRegionReference> cross_region_references{};
};

struct LargeWorldSummary final {
    std::uint64_t region_count{0};
    std::uint64_t settlement_assignment_count{0};
    std::uint64_t route_assignment_count{0};
    std::uint64_t caravan_assignment_count{0};
    std::uint64_t unassigned_settlement_count{0};
    std::uint64_t unassigned_route_count{0};
    std::uint64_t unassigned_caravan_count{0};
    std::uint64_t regional_feature_config_count{0};
    std::uint64_t regional_dependency_config_count{0};
    std::uint64_t cross_region_reference_count{0};
};

struct RegionalSnapshotSummary final {
    std::string region_id{};
    std::uint64_t visible_settlement_count{0};
    std::uint64_t visible_route_count{0};
    std::uint64_t visible_caravan_count{0};
    std::uint64_t enabled_feature_count{0};
    std::uint64_t disabled_feature_count{0};
    std::uint64_t dependency_count{0};
};

struct RegionalEventStreamSummary final {
    std::string region_id{};
    std::uint64_t event_count{0};
    std::uint64_t first_tick{0};
    std::uint64_t last_tick{0};
};

[[nodiscard]] data::ValidationReport validate_world_region_descriptor(const WorldRegionDescriptor& region);
[[nodiscard]] data::ValidationReport validate_regional_feature_config(const LargeWorldCatalog& catalog, const RegionalFeatureConfig& config);
[[nodiscard]] data::ValidationReport validate_regional_dependency_config(const LargeWorldCatalog& catalog, const RegionalDependencyConfig& config);
[[nodiscard]] data::ValidationReport validate_cross_region_reference(const LargeWorldCatalog& catalog, const CrossRegionReference& reference);
[[nodiscard]] data::ValidationReport add_world_region(LargeWorldCatalog& catalog, WorldRegionDescriptor region);
[[nodiscard]] data::ValidationReport add_regional_feature_config(LargeWorldCatalog& catalog, RegionalFeatureConfig config);
[[nodiscard]] data::ValidationReport add_regional_dependency_config(LargeWorldCatalog& catalog, RegionalDependencyConfig config);
[[nodiscard]] data::ValidationReport add_cross_region_reference(LargeWorldCatalog& catalog, CrossRegionReference reference);
[[nodiscard]] data::ValidationReport assign_settlement_region(LargeWorldCatalog& catalog, std::string settlement_id, std::string region_id);
[[nodiscard]] data::ValidationReport assign_route_region(LargeWorldCatalog& catalog, std::string route_id, std::string region_id);
[[nodiscard]] data::ValidationReport assign_caravan_region(LargeWorldCatalog& catalog, std::string caravan_id, std::string region_id);
[[nodiscard]] const WorldRegionDescriptor* world_region_by_id(const LargeWorldCatalog& catalog, std::string_view region_id) noexcept;
[[nodiscard]] std::string_view assigned_settlement_region(const LargeWorldCatalog& catalog, std::string_view settlement_id) noexcept;
[[nodiscard]] std::string_view assigned_route_region(const LargeWorldCatalog& catalog, std::string_view route_id) noexcept;
[[nodiscard]] std::string_view assigned_caravan_region(const LargeWorldCatalog& catalog, std::string_view caravan_id) noexcept;
[[nodiscard]] LargeWorldSummary make_large_world_summary(const SimulationRuntime& runtime, const LargeWorldCatalog& catalog);
[[nodiscard]] RegionalSnapshotSummary make_regional_snapshot_summary(
    const SimulationRuntime& runtime,
    const LargeWorldCatalog& catalog,
    std::string region_id
);
[[nodiscard]] RegionalEventStreamSummary make_regional_event_stream_summary(
    const LargeWorldCatalog& catalog,
    const clc::EventLog& event_log,
    std::string region_id
);
[[nodiscard]] data::ValidationReport validate_large_world_catalog(const SimulationRuntime& runtime, const LargeWorldCatalog& catalog);
[[nodiscard]] std::string large_world_summary_digest(const LargeWorldSummary& summary);
[[nodiscard]] std::string regional_snapshot_summary_digest(const RegionalSnapshotSummary& summary);
[[nodiscard]] std::string regional_event_stream_summary_digest(const RegionalEventStreamSummary& summary);

} // namespace clc::sim
