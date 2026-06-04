#include "clc/sim/LargeWorld.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

LargeWorldRegionAssignment* assignment_by_id(std::vector<LargeWorldRegionAssignment>& assignments, std::string_view object_id) noexcept {
    for (auto& assignment : assignments) {
        if (assignment.object_id == object_id) {
            return &assignment;
        }
    }
    return nullptr;
}

std::string_view assignment_region(const std::vector<LargeWorldRegionAssignment>& assignments, std::string_view object_id) noexcept {
    for (const auto& assignment : assignments) {
        if (assignment.object_id == object_id) {
            return assignment.region_id;
        }
    }
    return {};
}

data::ValidationReport assign_region(
    LargeWorldCatalog& catalog,
    std::vector<LargeWorldRegionAssignment>& assignments,
    std::string object_id,
    std::string region_id,
    std::string_view path
) {
    data::ValidationReport report;
    if (object_id.empty()) {
        report.add_error(std::string{path} + ".object_id", "object_id must not be empty");
    }
    if (region_id.empty()) {
        report.add_error(std::string{path} + ".region_id", "region_id must not be empty");
    }
    if (world_region_by_id(catalog, region_id) == nullptr) {
        report.add_error(std::string{path} + ".region_id", "region_id is not registered");
    }
    if (!report.ok()) {
        return report;
    }
    if (auto* existing = assignment_by_id(assignments, object_id)) {
        existing->region_id = std::move(region_id);
        return report;
    }
    assignments.push_back({.object_id = std::move(object_id), .region_id = std::move(region_id)});
    return report;
}

} // namespace

data::ValidationReport validate_world_region_descriptor(const WorldRegionDescriptor& region) {
    data::ValidationReport report;
    if (region.region_id.empty()) {
        report.add_error("large_world.region.region_id", "region_id must not be empty");
    }
    if (region.display_name.empty()) {
        report.add_warning("large_world.region.display_name", "display_name is empty");
    }
    return report;
}

data::ValidationReport validate_regional_feature_config(const LargeWorldCatalog& catalog, const RegionalFeatureConfig& config) {
    data::ValidationReport report;
    if (world_region_by_id(catalog, config.region_id) == nullptr) {
        report.add_error("large_world.regional_features.region_id", "region_id is not registered");
    }
    for (const auto& feature_id : config.enabled_feature_ids) {
        if (feature_id.empty()) {
            report.add_error("large_world.regional_features.enabled_feature_ids", "enabled feature ids must not be empty");
        }
    }
    for (const auto& feature_id : config.disabled_feature_ids) {
        if (feature_id.empty()) {
            report.add_error("large_world.regional_features.disabled_feature_ids", "disabled feature ids must not be empty");
        }
    }
    return report;
}

data::ValidationReport validate_regional_dependency_config(const LargeWorldCatalog& catalog, const RegionalDependencyConfig& config) {
    data::ValidationReport report;
    if (world_region_by_id(catalog, config.region_id) == nullptr) {
        report.add_error("large_world.regional_dependencies.region_id", "region_id is not registered");
    }
    if (config.dependency_id.empty()) {
        report.add_error("large_world.regional_dependencies.dependency_id", "dependency_id must not be empty");
    }
    if (config.source_system_id.empty()) {
        report.add_warning("large_world.regional_dependencies.source_system_id", "source_system_id is empty");
    }
    if (config.target_system_id.empty()) {
        report.add_warning("large_world.regional_dependencies.target_system_id", "target_system_id is empty");
    }
    return report;
}

data::ValidationReport validate_cross_region_reference(const LargeWorldCatalog& catalog, const CrossRegionReference& reference) {
    data::ValidationReport report;
    if (reference.reference_id.empty()) {
        report.add_error("large_world.cross_region_reference.reference_id", "reference_id must not be empty");
    }
    if (world_region_by_id(catalog, reference.source_region_id) == nullptr) {
        report.add_error("large_world.cross_region_reference.source_region_id", "source region is not registered");
    }
    if (world_region_by_id(catalog, reference.target_region_id) == nullptr) {
        report.add_error("large_world.cross_region_reference.target_region_id", "target region is not registered");
    }
    if (reference.reference_type.empty()) {
        report.add_error("large_world.cross_region_reference.reference_type", "reference_type must not be empty");
    }
    if (reference.object_id.empty()) {
        report.add_warning("large_world.cross_region_reference.object_id", "object_id is empty");
    }
    return report;
}

data::ValidationReport add_world_region(LargeWorldCatalog& catalog, WorldRegionDescriptor region) {
    auto report = validate_world_region_descriptor(region);
    if (!report.ok()) {
        return report;
    }
    if (world_region_by_id(catalog, region.region_id) != nullptr) {
        report.add_error("large_world.region.region_id", "region_id already exists");
        return report;
    }
    catalog.regions.push_back(std::move(region));
    return report;
}

data::ValidationReport add_regional_feature_config(LargeWorldCatalog& catalog, RegionalFeatureConfig config) {
    auto report = validate_regional_feature_config(catalog, config);
    if (!report.ok()) {
        return report;
    }
    for (auto& existing : catalog.regional_features) {
        if (existing.region_id == config.region_id) {
            existing = std::move(config);
            return report;
        }
    }
    catalog.regional_features.push_back(std::move(config));
    return report;
}

data::ValidationReport add_regional_dependency_config(LargeWorldCatalog& catalog, RegionalDependencyConfig config) {
    auto report = validate_regional_dependency_config(catalog, config);
    if (!report.ok()) {
        return report;
    }
    for (auto& existing : catalog.regional_dependencies) {
        if (existing.region_id == config.region_id && existing.dependency_id == config.dependency_id) {
            existing = std::move(config);
            return report;
        }
    }
    catalog.regional_dependencies.push_back(std::move(config));
    return report;
}

data::ValidationReport add_cross_region_reference(LargeWorldCatalog& catalog, CrossRegionReference reference) {
    auto report = validate_cross_region_reference(catalog, reference);
    if (!report.ok()) {
        return report;
    }
    for (const auto& existing : catalog.cross_region_references) {
        if (existing.reference_id == reference.reference_id) {
            report.add_error("large_world.cross_region_reference.reference_id", "reference_id already exists");
            return report;
        }
    }
    catalog.cross_region_references.push_back(std::move(reference));
    return report;
}

data::ValidationReport assign_settlement_region(LargeWorldCatalog& catalog, std::string settlement_id, std::string region_id) {
    return assign_region(catalog, catalog.settlements, std::move(settlement_id), std::move(region_id), "large_world.settlement");
}

data::ValidationReport assign_route_region(LargeWorldCatalog& catalog, std::string route_id, std::string region_id) {
    return assign_region(catalog, catalog.routes, std::move(route_id), std::move(region_id), "large_world.route");
}

data::ValidationReport assign_caravan_region(LargeWorldCatalog& catalog, std::string caravan_id, std::string region_id) {
    return assign_region(catalog, catalog.caravans, std::move(caravan_id), std::move(region_id), "large_world.caravan");
}

const WorldRegionDescriptor* world_region_by_id(const LargeWorldCatalog& catalog, std::string_view region_id) noexcept {
    for (const auto& region : catalog.regions) {
        if (region.region_id == region_id) {
            return &region;
        }
    }
    return nullptr;
}

std::string_view assigned_settlement_region(const LargeWorldCatalog& catalog, std::string_view settlement_id) noexcept {
    return assignment_region(catalog.settlements, settlement_id);
}

std::string_view assigned_route_region(const LargeWorldCatalog& catalog, std::string_view route_id) noexcept {
    return assignment_region(catalog.routes, route_id);
}

std::string_view assigned_caravan_region(const LargeWorldCatalog& catalog, std::string_view caravan_id) noexcept {
    return assignment_region(catalog.caravans, caravan_id);
}

LargeWorldSummary make_large_world_summary(const SimulationRuntime& runtime, const LargeWorldCatalog& catalog) {
    LargeWorldSummary summary{
        .region_count = catalog.regions.size(),
        .settlement_assignment_count = catalog.settlements.size(),
        .route_assignment_count = catalog.routes.size(),
        .caravan_assignment_count = catalog.caravans.size(),
    };
    summary.regional_feature_config_count = catalog.regional_features.size();
    summary.regional_dependency_config_count = catalog.regional_dependencies.size();
    summary.cross_region_reference_count = catalog.cross_region_references.size();
    for (const auto& settlement : runtime.engine.settlements()) {
        if (assigned_settlement_region(catalog, settlement.id).empty()) {
            ++summary.unassigned_settlement_count;
        }
    }
    for (const auto& route : runtime.routes.routes) {
        if (assigned_route_region(catalog, route.id).empty()) {
            ++summary.unassigned_route_count;
        }
    }
    for (const auto& caravan : runtime.caravans.caravans) {
        if (assigned_caravan_region(catalog, caravan.id).empty()) {
            ++summary.unassigned_caravan_count;
        }
    }
    return summary;
}

RegionalSnapshotSummary make_regional_snapshot_summary(
    const SimulationRuntime& runtime,
    const LargeWorldCatalog& catalog,
    std::string region_id
) {
    RegionalSnapshotSummary summary{.region_id = std::move(region_id)};
    for (const auto& settlement : runtime.engine.settlements()) {
        if (assigned_settlement_region(catalog, settlement.id) == summary.region_id) {
            ++summary.visible_settlement_count;
        }
    }
    for (const auto& route : runtime.routes.routes) {
        if (assigned_route_region(catalog, route.id) == summary.region_id) {
            ++summary.visible_route_count;
        }
    }
    for (const auto& caravan : runtime.caravans.caravans) {
        if (assigned_caravan_region(catalog, caravan.id) == summary.region_id) {
            ++summary.visible_caravan_count;
        }
    }
    for (const auto& config : catalog.regional_features) {
        if (config.region_id == summary.region_id) {
            summary.enabled_feature_count += config.enabled_feature_ids.size();
            summary.disabled_feature_count += config.disabled_feature_ids.size();
        }
    }
    for (const auto& dependency : catalog.regional_dependencies) {
        if (dependency.region_id == summary.region_id) {
            ++summary.dependency_count;
        }
    }
    return summary;
}

RegionalEventStreamSummary make_regional_event_stream_summary(
    const LargeWorldCatalog& catalog,
    const clc::EventLog& event_log,
    std::string region_id
) {
    RegionalEventStreamSummary summary{.region_id = std::move(region_id)};
    if (world_region_by_id(catalog, summary.region_id) == nullptr) {
        return summary;
    }
    for (const auto& event : event_log.events()) {
        if (event.payload.find(summary.region_id) == std::string::npos && event.type.find(summary.region_id) == std::string::npos) {
            continue;
        }
        if (summary.event_count == 0) {
            summary.first_tick = event.tick;
        }
        summary.last_tick = event.tick;
        ++summary.event_count;
    }
    return summary;
}

data::ValidationReport validate_large_world_catalog(const SimulationRuntime& runtime, const LargeWorldCatalog& catalog) {
    data::ValidationReport report;
    for (const auto& region : catalog.regions) {
        auto region_report = validate_world_region_descriptor(region);
        for (const auto& message : region_report.messages()) {
            if (message.severity == data::ValidationSeverity::warning) {
                report.add_warning(message.path, message.message);
            } else {
                report.add_error(message.path, message.message);
            }
        }
    }
    for (const auto& assignment : catalog.settlements) {
        if (!runtime.engine.has_settlement(assignment.object_id)) {
            report.add_error("large_world.settlement.object_id", "assigned settlement does not exist");
        }
        if (world_region_by_id(catalog, assignment.region_id) == nullptr) {
            report.add_error("large_world.settlement.region_id", "assigned settlement region does not exist");
        }
    }
    for (const auto& assignment : catalog.routes) {
        if (settlement_route_by_id(runtime.routes, assignment.object_id) == nullptr) {
            report.add_error("large_world.route.object_id", "assigned route does not exist");
        }
        if (world_region_by_id(catalog, assignment.region_id) == nullptr) {
            report.add_error("large_world.route.region_id", "assigned route region does not exist");
        }
    }
    for (const auto& assignment : catalog.caravans) {
        if (caravan_by_id(runtime.caravans, assignment.object_id) == nullptr) {
            report.add_error("large_world.caravan.object_id", "assigned caravan does not exist");
        }
        if (world_region_by_id(catalog, assignment.region_id) == nullptr) {
            report.add_error("large_world.caravan.region_id", "assigned caravan region does not exist");
        }
    }
    for (const auto& config : catalog.regional_features) {
        const auto config_report = validate_regional_feature_config(catalog, config);
        for (const auto& message : config_report.messages()) {
            if (message.severity == data::ValidationSeverity::warning) {
                report.add_warning(message.path, message.message);
            } else {
                report.add_error(message.path, message.message);
            }
        }
    }
    for (const auto& config : catalog.regional_dependencies) {
        const auto config_report = validate_regional_dependency_config(catalog, config);
        for (const auto& message : config_report.messages()) {
            if (message.severity == data::ValidationSeverity::warning) {
                report.add_warning(message.path, message.message);
            } else {
                report.add_error(message.path, message.message);
            }
        }
    }
    for (const auto& reference : catalog.cross_region_references) {
        const auto reference_report = validate_cross_region_reference(catalog, reference);
        for (const auto& message : reference_report.messages()) {
            if (message.severity == data::ValidationSeverity::warning) {
                report.add_warning(message.path, message.message);
            } else {
                report.add_error(message.path, message.message);
            }
        }
    }
    return report;
}

std::string large_world_summary_digest(const LargeWorldSummary& summary) {
    std::ostringstream out;
    out << "large_world"
        << ";regions=" << summary.region_count
        << ";settlement_assignments=" << summary.settlement_assignment_count
        << ";route_assignments=" << summary.route_assignment_count
        << ";caravan_assignments=" << summary.caravan_assignment_count
        << ";unassigned_settlements=" << summary.unassigned_settlement_count
        << ";unassigned_routes=" << summary.unassigned_route_count
        << ";unassigned_caravans=" << summary.unassigned_caravan_count
        << ";regional_features=" << summary.regional_feature_config_count
        << ";regional_dependencies=" << summary.regional_dependency_config_count
        << ";cross_region_refs=" << summary.cross_region_reference_count;
    return out.str();
}

std::string regional_snapshot_summary_digest(const RegionalSnapshotSummary& summary) {
    std::ostringstream out;
    out << "regional_snapshot"
        << ";region=" << summary.region_id
        << ";settlements=" << summary.visible_settlement_count
        << ";routes=" << summary.visible_route_count
        << ";caravans=" << summary.visible_caravan_count
        << ";features_enabled=" << summary.enabled_feature_count
        << ";features_disabled=" << summary.disabled_feature_count
        << ";dependencies=" << summary.dependency_count;
    return out.str();
}

std::string regional_event_stream_summary_digest(const RegionalEventStreamSummary& summary) {
    std::ostringstream out;
    out << "regional_events"
        << ";region=" << summary.region_id
        << ";events=" << summary.event_count
        << ";first_tick=" << summary.first_tick
        << ";last_tick=" << summary.last_tick;
    return out.str();
}

} // namespace clc::sim
