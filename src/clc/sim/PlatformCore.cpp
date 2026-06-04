#include "clc/sim/PlatformCore.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace clc::sim {
namespace {

bool contains_string(const std::vector<std::string>& values, std::string_view value) {
    return std::find(values.begin(), values.end(), value) != values.end();
}

const PlatformModuleDescriptor* module_by_id(const PlatformCoreRegistry& registry, std::string_view module_id) {
    for (const auto& module : registry.modules) {
        if (module.module_id == module_id) {
            return &module;
        }
    }
    return nullptr;
}

const GenreProfileDescriptor* genre_profile_by_id(const PlatformCoreRegistry& registry, std::string_view profile_id) {
    for (const auto& profile : registry.genre_profiles) {
        if (profile.profile_id == profile_id) {
            return &profile;
        }
    }
    return nullptr;
}

void append_validation(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::warning) {
            target.add_warning(message.path, message.message);
        } else {
            target.add_error(message.path, message.message);
        }
    }
}

} // namespace

std::string_view platform_module_kind_name(PlatformModuleKind kind) noexcept {
    switch (kind) {
    case PlatformModuleKind::core:
        return "core";
    case PlatformModuleKind::optional:
        return "optional";
    case PlatformModuleKind::genre:
        return "genre";
    }
    return "core";
}

PlatformCoreRegistry make_standard_platform_core_registry() {
    return {
        .modules = {
            {.module_id = "runtime", .kind = PlatformModuleKind::core},
            {.module_id = "resources", .kind = PlatformModuleKind::core, .dependency_module_ids = {"runtime"}},
            {.module_id = "production", .kind = PlatformModuleKind::core, .dependency_module_ids = {"resources"}},
            {.module_id = "population", .kind = PlatformModuleKind::core, .dependency_module_ids = {"resources"}},
            {.module_id = "market", .kind = PlatformModuleKind::core, .dependency_module_ids = {"resources"}},
            {.module_id = "logistics", .kind = PlatformModuleKind::core, .dependency_module_ids = {"runtime", "market"}},
            {.module_id = "contracts", .kind = PlatformModuleKind::core, .dependency_module_ids = {"market", "logistics"}},
            {.module_id = "factions", .kind = PlatformModuleKind::core},
            {.module_id = "save_replay", .kind = PlatformModuleKind::core, .dependency_module_ids = {"runtime"}},
            {.module_id = "diagnostics", .kind = PlatformModuleKind::core, .dependency_module_ids = {"runtime"}},
            {.module_id = "regional", .kind = PlatformModuleKind::optional, .dependency_module_ids = {"runtime", "market", "logistics"}},
            {.module_id = "authority", .kind = PlatformModuleKind::optional, .dependency_module_ids = {"runtime", "factions"}},
            {.module_id = "editor_admin", .kind = PlatformModuleKind::optional, .dependency_module_ids = {"diagnostics"}},
            {.module_id = "idle_tycoon", .kind = PlatformModuleKind::genre, .dependency_module_ids = {"production", "market"}, .compatible_profile_ids = {"idle_tycoon"}},
            {.module_id = "city_builder", .kind = PlatformModuleKind::genre, .dependency_module_ids = {"population", "production", "logistics"}, .compatible_profile_ids = {"city_builder"}},
            {.module_id = "strategy", .kind = PlatformModuleKind::genre, .dependency_module_ids = {"factions", "contracts", "regional"}, .compatible_profile_ids = {"strategy"}},
            {.module_id = "settlement_rpg", .kind = PlatformModuleKind::genre, .dependency_module_ids = {"population", "contracts", "factions"}, .compatible_profile_ids = {"settlement_rpg"}},
            {.module_id = "survival_economy", .kind = PlatformModuleKind::genre, .dependency_module_ids = {"resources", "production", "logistics"}, .compatible_profile_ids = {"survival_economy"}},
        },
        .genre_profiles = {
            {.profile_id = "idle_tycoon", .enabled_module_ids = {"runtime", "resources", "production", "market", "diagnostics", "idle_tycoon"}, .default_rule_ids = {"slow_growth", "offline_progression"}, .expected_data_templates = {"resource_chain", "building_upgrade"}, .recommended_action_types = {"advance_ticks", "request_report"}, .recommended_report_ids = {"production", "market"}},
            {.profile_id = "city_builder", .enabled_module_ids = {"runtime", "resources", "production", "population", "logistics", "market", "diagnostics", "city_builder"}, .default_rule_ids = {"housing_pressure", "service_needs"}, .expected_data_templates = {"settlement", "district", "route"}, .recommended_action_types = {"build", "dispatch_caravan", "request_report"}, .recommended_report_ids = {"population", "logistics", "market"}},
            {.profile_id = "strategy", .enabled_module_ids = {"runtime", "resources", "market", "logistics", "contracts", "factions", "regional", "authority", "diagnostics", "strategy"}, .default_rule_ids = {"territory_pressure", "war_economy"}, .expected_data_templates = {"region", "faction", "contract"}, .recommended_action_types = {"change_policy", "issue_contract", "request_report"}, .recommended_report_ids = {"regional", "factions", "contracts"}},
            {.profile_id = "settlement_rpg", .enabled_module_ids = {"runtime", "resources", "population", "contracts", "factions", "diagnostics", "settlement_rpg"}, .default_rule_ids = {"local_reputation", "quest_economy"}, .expected_data_templates = {"settlement", "npc_faction", "contract"}, .recommended_action_types = {"issue_contract", "request_report"}, .recommended_report_ids = {"population", "factions", "contracts"}},
            {.profile_id = "survival_economy", .enabled_module_ids = {"runtime", "resources", "production", "logistics", "market", "diagnostics", "survival_economy"}, .default_rule_ids = {"scarcity", "weather_pressure"}, .expected_data_templates = {"resource", "recipe", "route"}, .recommended_action_types = {"advance_ticks", "dispatch_caravan", "request_report"}, .recommended_report_ids = {"resources", "production", "logistics"}},
        },
    };
}

data::ValidationReport validate_platform_core_registry(const PlatformCoreRegistry& registry) {
    data::ValidationReport report;
    if (registry.modules.empty()) {
        report.add_error("platform_core.modules", "module registry must not be empty");
    }
    if (registry.genre_profiles.empty()) {
        report.add_error("platform_core.genre_profiles", "genre profiles must not be empty");
    }
    for (const auto& module : registry.modules) {
        if (module.module_id.empty()) {
            report.add_error("platform_core.module_id", "module_id must not be empty");
        }
        for (const auto& dependency_id : module.dependency_module_ids) {
            if (module_by_id(registry, dependency_id) == nullptr) {
                report.add_error("platform_core." + module.module_id + ".dependency", "dependency module is not registered");
            }
        }
    }
    for (const auto& profile : registry.genre_profiles) {
        if (profile.profile_id.empty()) {
            report.add_error("platform_core.profile_id", "profile_id must not be empty");
        }
        if (profile.enabled_module_ids.empty()) {
            report.add_error("platform_core." + profile.profile_id + ".modules", "genre profile must enable modules");
        }
        if (profile.expected_data_templates.empty()) {
            report.add_error("platform_core." + profile.profile_id + ".templates", "genre profile must declare expected data templates");
        }
        if (profile.recommended_action_types.empty() || profile.recommended_report_ids.empty()) {
            report.add_error("platform_core." + profile.profile_id + ".actions_reports", "genre profile must declare recommended actions and reports");
        }
        for (const auto& module_id : profile.enabled_module_ids) {
            if (module_by_id(registry, module_id) == nullptr) {
                report.add_error("platform_core." + profile.profile_id + ".module", "enabled module is not registered");
            }
        }
    }
    return report;
}

data::ValidationReport validate_content_pack_manifest(
    const ContentPackManifest& manifest,
    const PlatformCoreRegistry& registry
) {
    data::ValidationReport report;
    if (manifest.content_pack_id.empty()) {
        report.add_error("content_pack.id", "content_pack_id must not be empty");
    }
    if (manifest.schema_version == 0) {
        report.add_error("content_pack.schema_version", "schema_version must be greater than zero");
    }
    if (manifest.required_module_ids.empty()) {
        report.add_error("content_pack.required_modules", "content pack must require at least one module");
    }
    for (const auto& module_id : manifest.required_module_ids) {
        if (module_by_id(registry, module_id) == nullptr) {
            report.add_error("content_pack.required_modules", "required module is not registered");
        }
    }
    for (const auto& profile_id : manifest.compatible_profile_ids) {
        if (genre_profile_by_id(registry, profile_id) == nullptr) {
            report.add_error("content_pack.compatible_profiles", "compatible profile is not registered");
        }
    }
    for (const auto& migration_id : manifest.migration_ids) {
        if (migration_id.empty()) {
            report.add_error("content_pack.migrations", "migration ids must not be empty");
        }
    }
    return report;
}

PlatformCoreQueryReport query_platform_core_registry(const PlatformCoreRegistry& registry) {
    PlatformCoreQueryReport report;
    report.module_count = registry.modules.size();
    report.genre_profile_count = registry.genre_profiles.size();
    report.validation = validate_platform_core_registry(registry);
    for (const auto& module : registry.modules) {
        switch (module.kind) {
        case PlatformModuleKind::core:
            ++report.core_module_count;
            break;
        case PlatformModuleKind::optional:
            ++report.optional_module_count;
            break;
        case PlatformModuleKind::genre:
            ++report.genre_module_count;
            break;
        }
    }
    for (const auto& profile : registry.genre_profiles) {
        report.content_template_count += profile.expected_data_templates.size();
        report.recommended_report_count += profile.recommended_report_ids.size();
    }
    return report;
}

PlatformDiagnosticsReport make_platform_diagnostics_report(
    const PlatformCoreRegistry& registry,
    const ContentPackManifest& manifest
) {
    PlatformDiagnosticsReport report;
    report.query = query_platform_core_registry(registry);
    report.content_pipeline_validation = validate_content_pack_manifest(manifest, registry);
    report.runtime_diagnostic_count = report.query.module_count;
    report.economy_diagnostic_count = contains_string(manifest.required_module_ids, "market") ? 1 : 0;
    report.action_diagnostic_count = report.query.genre_profile_count;
    report.replay_diagnostic_count = contains_string(manifest.required_module_ids, "save_replay") ? 1 : 0;
    report.migration_diagnostic_count = manifest.migration_ids.size();
    report.ready = report.query.validation.ok()
        && report.content_pipeline_validation.ok()
        && report.runtime_diagnostic_count > 0
        && report.action_diagnostic_count > 0;
    return report;
}

ContentPackManifest make_standard_platform_content_pack_manifest() {
    return {
        .content_pack_id = "standard_platform_demo",
        .schema_version = 1,
        .required_module_ids = {"runtime", "resources", "production", "market", "save_replay", "diagnostics"},
        .compatible_profile_ids = {"idle_tycoon", "city_builder", "survival_economy"},
        .migration_ids = {"v3_to_v4_content_pack_manifest"},
    };
}

std::string platform_core_query_digest(const PlatformCoreQueryReport& report) {
    std::ostringstream out;
    out << "platform_core_query"
        << ";modules=" << report.module_count
        << ";core=" << report.core_module_count
        << ";optional=" << report.optional_module_count
        << ";genre=" << report.genre_module_count
        << ";profiles=" << report.genre_profile_count
        << ";templates=" << report.content_template_count
        << ";reports=" << report.recommended_report_count
        << ";diagnostics=" << report.validation.messages().size();
    return out.str();
}

std::string platform_diagnostics_digest(const PlatformDiagnosticsReport& report) {
    std::ostringstream out;
    out << "platform_diagnostics"
        << ";ready=" << (report.ready ? "yes" : "no")
        << ";query={" << platform_core_query_digest(report.query) << "}"
        << ";content_diagnostics=" << report.content_pipeline_validation.messages().size()
        << ";runtime=" << report.runtime_diagnostic_count
        << ";economy=" << report.economy_diagnostic_count
        << ";actions=" << report.action_diagnostic_count
        << ";replay=" << report.replay_diagnostic_count
        << ";migration=" << report.migration_diagnostic_count;
    return out.str();
}

std::string platform_diagnostics_markdown(const PlatformDiagnosticsReport& report) {
    std::ostringstream out;
    out << "# Platform Diagnostics\n\n";
    out << "- Digest: `" << platform_diagnostics_digest(report) << "`\n";
    out << "- Ready: " << (report.ready ? "yes" : "no") << "\n";
    out << "- Modules: " << report.query.module_count << "\n";
    out << "- Genre profiles: " << report.query.genre_profile_count << "\n";
    out << "- Content diagnostics: " << report.content_pipeline_validation.messages().size() << "\n";
    return out.str();
}

} // namespace clc::sim
