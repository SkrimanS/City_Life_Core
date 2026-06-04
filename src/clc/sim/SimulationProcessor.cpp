#include "clc/sim/SimulationProcessor.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

void append_validation(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::warning) {
            target.add_warning(message.path, message.message);
        } else {
            target.add_error(message.path, message.message);
        }
    }
}

bool contains_id(const std::vector<std::string>& values, std::string_view id) {
    return std::find(values.begin(), values.end(), id) != values.end();
}

const SimulationFeatureToggle* feature_by_id(const SimulationProcessorRegistry& registry, std::string_view feature_id) noexcept {
    for (const auto& feature : registry.features) {
        if (feature.feature_id == feature_id) {
            return &feature;
        }
    }
    return nullptr;
}

const SimulationDependencyToggle* dependency_by_id(const SimulationProcessorRegistry& registry, std::string_view dependency_id) noexcept {
    for (const auto& dependency : registry.dependencies) {
        if (dependency.dependency_id == dependency_id) {
            return &dependency;
        }
    }
    return nullptr;
}

bool processor_gates_enabled(const SimulationProcessorRegistry& registry, const SimulationProcessorDescriptor& processor) noexcept {
    if (!processor.enabled) {
        return false;
    }
    for (const auto& feature_id : processor.required_feature_ids) {
        if (!simulation_feature_enabled(registry, feature_id)) {
            return false;
        }
    }
    for (const auto& dependency_id : processor.required_dependency_ids) {
        if (!simulation_dependency_enabled(registry, dependency_id)) {
            return false;
        }
    }
    return true;
}

} // namespace

data::ValidationReport validate_simulation_feature_toggle(const SimulationFeatureToggle& feature) {
    data::ValidationReport report;
    if (feature.feature_id.empty()) {
        report.add_error("processor.feature.feature_id", "feature_id must not be empty");
    }
    return report;
}

data::ValidationReport validate_simulation_dependency_toggle(const SimulationDependencyToggle& dependency) {
    data::ValidationReport report;
    if (dependency.dependency_id.empty()) {
        report.add_error("processor.dependency.dependency_id", "dependency_id must not be empty");
    }
    if (dependency.source_system_id.empty()) {
        report.add_warning("processor.dependency.source_system_id", "source_system_id is empty");
    }
    if (dependency.target_system_id.empty()) {
        report.add_warning("processor.dependency.target_system_id", "target_system_id is empty");
    }
    return report;
}

data::ValidationReport validate_simulation_processor_descriptor(const SimulationProcessorDescriptor& processor) {
    data::ValidationReport report;
    if (processor.processor_id.empty()) {
        report.add_error("processor.processor_id", "processor_id must not be empty");
    }
    for (const auto& feature_id : processor.required_feature_ids) {
        if (feature_id.empty()) {
            report.add_error("processor.required_feature_ids", "required feature ids must not be empty");
        }
    }
    for (const auto& dependency_id : processor.required_dependency_ids) {
        if (dependency_id.empty()) {
            report.add_error("processor.required_dependency_ids", "required dependency ids must not be empty");
        }
    }
    if (!processor.save_load_aware) {
        report.add_warning("processor.save_load_aware", "processor is not marked save/load aware");
    }
    if (!processor.replay_aware) {
        report.add_warning("processor.replay_aware", "processor is not marked replay aware");
    }
    if (!processor.diagnostics_friendly) {
        report.add_warning("processor.diagnostics_friendly", "processor is not marked diagnostics friendly");
    }
    if (!processor.engine_agnostic) {
        report.add_error("processor.engine_agnostic", "processor must be engine agnostic");
    }
    return report;
}

data::ValidationReport add_simulation_feature(SimulationProcessorRegistry& registry, SimulationFeatureToggle feature) {
    auto report = validate_simulation_feature_toggle(feature);
    if (!report.ok()) {
        return report;
    }
    if (feature_by_id(registry, feature.feature_id) != nullptr) {
        report.add_error("processor.feature.feature_id", "feature_id already exists");
        return report;
    }
    registry.features.push_back(std::move(feature));
    return report;
}

data::ValidationReport add_simulation_dependency(SimulationProcessorRegistry& registry, SimulationDependencyToggle dependency) {
    auto report = validate_simulation_dependency_toggle(dependency);
    if (!report.ok()) {
        return report;
    }
    if (dependency_by_id(registry, dependency.dependency_id) != nullptr) {
        report.add_error("processor.dependency.dependency_id", "dependency_id already exists");
        return report;
    }
    registry.dependencies.push_back(std::move(dependency));
    return report;
}

data::ValidationReport add_simulation_processor(SimulationProcessorRegistry& registry, SimulationProcessorDescriptor processor) {
    auto report = validate_simulation_processor_descriptor(processor);
    if (!report.ok()) {
        return report;
    }
    for (const auto& existing : registry.processors) {
        if (existing.processor_id == processor.processor_id) {
            report.add_error("processor.processor_id", "processor_id already exists");
            return report;
        }
    }
    for (const auto& feature_id : processor.required_feature_ids) {
        if (feature_by_id(registry, feature_id) == nullptr) {
            report.add_error("processor.required_feature_ids", "required feature is not registered");
        }
    }
    for (const auto& dependency_id : processor.required_dependency_ids) {
        if (dependency_by_id(registry, dependency_id) == nullptr) {
            report.add_error("processor.required_dependency_ids", "required dependency is not registered");
        }
    }
    if (!report.ok()) {
        return report;
    }
    registry.processors.push_back(std::move(processor));
    return report;
}

SimulationProcessorRegistry make_standard_core_processor_registry() {
    SimulationProcessorRegistry registry;
    const std::vector<std::string> features{
        "resources", "production", "population", "weather", "logistics", "market", "policies", "factions",
        "contracts", "ecology", "events", "autonomous_actors", "content_packs", "scenario_runner",
        "runtime", "save_replay", "authority", "snapshots", "regional", "handoff",
    };
    for (const auto& feature_id : features) {
        const auto report = add_simulation_feature(registry, {.feature_id = feature_id, .enabled = true});
        (void)report;
    }

    const std::vector<SimulationDependencyToggle> dependencies{
        {.dependency_id = "resources.production", .source_system_id = "resources", .target_system_id = "production", .enabled = true},
        {.dependency_id = "resources.population", .source_system_id = "resources", .target_system_id = "population", .enabled = true},
        {.dependency_id = "weather.logistics", .source_system_id = "weather", .target_system_id = "logistics", .enabled = true},
        {.dependency_id = "weather.ecology", .source_system_id = "weather", .target_system_id = "ecology", .enabled = true},
        {.dependency_id = "logistics.market", .source_system_id = "logistics", .target_system_id = "market", .enabled = true},
        {.dependency_id = "population.market", .source_system_id = "population", .target_system_id = "market", .enabled = true},
        {.dependency_id = "policies.market", .source_system_id = "policies", .target_system_id = "market", .enabled = true},
        {.dependency_id = "factions.logistics", .source_system_id = "factions", .target_system_id = "logistics", .enabled = true},
        {.dependency_id = "contracts.market", .source_system_id = "contracts", .target_system_id = "market", .enabled = true},
        {.dependency_id = "ecology.market", .source_system_id = "ecology", .target_system_id = "market", .enabled = true},
        {.dependency_id = "events.shocks", .source_system_id = "events", .target_system_id = "market", .enabled = true},
        {.dependency_id = "regional.runtime", .source_system_id = "regional", .target_system_id = "runtime", .enabled = true},
    };
    for (const auto& dependency : dependencies) {
        const auto report = add_simulation_dependency(registry, dependency);
        (void)report;
    }

    const std::vector<SimulationProcessorDescriptor> processors{
        {.processor_id = "resources", .display_name = "Resources, goods, storage, quality and decay", .deterministic_order = 10, .required_feature_ids = {"resources"}},
        {.processor_id = "production", .display_name = "Production chains, workplaces, upkeep and maintenance", .deterministic_order = 20, .required_feature_ids = {"production"}, .required_dependency_ids = {"resources.production"}},
        {.processor_id = "population", .display_name = "Population, needs, workforce, happiness and migration", .deterministic_order = 30, .required_feature_ids = {"population"}, .required_dependency_ids = {"resources.population"}},
        {.processor_id = "weather", .display_name = "Weather, seasons, climate and environmental conditions", .deterministic_order = 40, .required_feature_ids = {"weather"}},
        {.processor_id = "logistics", .display_name = "Routes, transport, caravans and risk", .deterministic_order = 50, .required_feature_ids = {"logistics"}, .required_dependency_ids = {"weather.logistics", "factions.logistics"}},
        {.processor_id = "market", .display_name = "Supply, demand, liquidity, volatility, depth and shocks", .deterministic_order = 60, .required_feature_ids = {"market"}, .required_dependency_ids = {"logistics.market", "population.market", "policies.market", "contracts.market", "ecology.market", "events.shocks"}},
        {.processor_id = "policies", .display_name = "Taxes, fees, policies and economic controls", .deterministic_order = 70, .required_feature_ids = {"policies"}},
        {.processor_id = "factions", .display_name = "Factions, influence, control, law and security", .deterministic_order = 80, .required_feature_ids = {"factions"}},
        {.processor_id = "contracts", .display_name = "Contracts, commitments, orders and obligations", .deterministic_order = 90, .required_feature_ids = {"contracts"}},
        {.processor_id = "ecology", .display_name = "Ecology, agriculture, regeneration and environmental pressure", .deterministic_order = 100, .required_feature_ids = {"ecology"}, .required_dependency_ids = {"weather.ecology"}},
        {.processor_id = "events", .display_name = "Events, crises, chains and shock propagation", .deterministic_order = 110, .required_feature_ids = {"events"}},
        {.processor_id = "autonomous_actors", .display_name = "Autonomous simulation actors and decision profiles", .deterministic_order = 120, .required_feature_ids = {"autonomous_actors"}},
        {.processor_id = "content_packs", .display_name = "Content packs, presets and balance profiles", .deterministic_order = 130, .required_feature_ids = {"content_packs"}},
        {.processor_id = "scenario_runner", .display_name = "Scenario runner, reports, forecasts and comparative simulation", .deterministic_order = 140, .required_feature_ids = {"scenario_runner"}},
        {.processor_id = "runtime", .display_name = "Local runtime orchestration", .deterministic_order = 150, .required_feature_ids = {"runtime"}},
        {.processor_id = "save_replay", .display_name = "Save, load, replay and migration", .deterministic_order = 160, .required_feature_ids = {"save_replay"}},
        {.processor_id = "authority", .display_name = "Authority, ownership and permission safety", .deterministic_order = 170, .required_feature_ids = {"authority"}},
        {.processor_id = "snapshots", .display_name = "Snapshots, visibility and client sync", .deterministic_order = 180, .required_feature_ids = {"snapshots"}},
        {.processor_id = "regional", .display_name = "Regional and large-world simulation systems", .deterministic_order = 190, .required_feature_ids = {"regional"}, .required_dependency_ids = {"regional.runtime"}},
        {.processor_id = "handoff", .display_name = "Platform readiness and game developer handoff", .deterministic_order = 200, .required_feature_ids = {"handoff"}},
    };
    for (const auto& processor : processors) {
        const auto report = add_simulation_processor(registry, processor);
        (void)report;
    }
    return registry;
}

bool simulation_feature_enabled(const SimulationProcessorRegistry& registry, std::string_view feature_id) noexcept {
    const auto* feature = feature_by_id(registry, feature_id);
    return feature != nullptr && feature->enabled;
}

bool simulation_dependency_enabled(const SimulationProcessorRegistry& registry, std::string_view dependency_id) noexcept {
    const auto* dependency = dependency_by_id(registry, dependency_id);
    return dependency != nullptr && dependency->enabled;
}

std::vector<SimulationProcessorDescriptor> enabled_simulation_processors_in_order(const SimulationProcessorRegistry& registry) {
    std::vector<SimulationProcessorDescriptor> processors;
    for (const auto& processor : registry.processors) {
        if (processor_gates_enabled(registry, processor)) {
            processors.push_back(processor);
        }
    }
    std::sort(processors.begin(), processors.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.deterministic_order != rhs.deterministic_order) {
            return lhs.deterministic_order < rhs.deterministic_order;
        }
        return lhs.processor_id < rhs.processor_id;
    });
    return processors;
}

SimulationProcessorRunReport make_simulation_processor_run_report(const SimulationProcessorRegistry& registry) {
    SimulationProcessorRunReport report;
    const auto enabled = enabled_simulation_processors_in_order(registry);
    report.enabled_processor_count = enabled.size();
    for (const auto& processor : enabled) {
        report.deterministic_order.push_back(processor.processor_id);
        report.diagnostics.push_back({
            .processor_id = processor.processor_id,
            .severity = "info",
            .message = "processor enabled",
        });
        for (const auto& dependency_id : processor.required_dependency_ids) {
            report.causes.push_back({
                .processor_id = processor.processor_id,
                .dependency_id = dependency_id,
                .target_system_id = processor.processor_id,
                .cause = "dependency allowed processor",
            });
        }
    }
    for (const auto& processor : registry.processors) {
        if (!processor_gates_enabled(registry, processor)) {
            ++report.skipped_processor_count;
            report.diagnostics.push_back({
                .processor_id = processor.processor_id,
                .severity = "warning",
                .message = "processor skipped by feature/dependency gate",
            });
        }
    }
    for (const auto& dependency : registry.dependencies) {
        if (!dependency.enabled) {
            ++report.disabled_dependency_count;
        }
    }
    return report;
}

DeepSimulationCoreReadinessReport make_deep_simulation_core_readiness_report(const SimulationProcessorRegistry& registry) {
    DeepSimulationCoreReadinessReport report;
    report.processor_count = registry.processors.size();
    report.processor_architecture_ready = !registry.processors.empty();
    report.feature_toggles_ready = !registry.features.empty();
    report.dependency_toggles_ready = !registry.dependencies.empty();
    report.save_load_ready = true;
    report.replay_ready = true;
    report.diagnostics_ready = true;
    report.engine_agnostic_ready = true;
    for (const auto& feature : registry.features) {
        append_validation(report.validation, validate_simulation_feature_toggle(feature));
    }
    for (const auto& dependency : registry.dependencies) {
        append_validation(report.validation, validate_simulation_dependency_toggle(dependency));
    }
    for (const auto& processor : registry.processors) {
        append_validation(report.validation, validate_simulation_processor_descriptor(processor));
        report.save_load_ready = report.save_load_ready && processor.save_load_aware;
        report.replay_ready = report.replay_ready && processor.replay_aware;
        report.diagnostics_ready = report.diagnostics_ready && processor.diagnostics_friendly;
        report.engine_agnostic_ready = report.engine_agnostic_ready && processor.engine_agnostic;
        for (const auto& feature_id : processor.required_feature_ids) {
            if (feature_by_id(registry, feature_id) == nullptr) {
                report.validation.add_error("processor.required_feature_ids", "required feature is not registered");
            }
        }
        for (const auto& dependency_id : processor.required_dependency_ids) {
            if (dependency_by_id(registry, dependency_id) == nullptr) {
                report.validation.add_error("processor.required_dependency_ids", "required dependency is not registered");
            }
        }
    }
    report.issue_count = report.validation.messages().size();
    return report;
}

std::string simulation_processor_run_report_digest(const SimulationProcessorRunReport& report) {
    std::ostringstream out;
    out << "simulation_processors"
        << ";enabled=" << report.enabled_processor_count
        << ";skipped=" << report.skipped_processor_count
        << ";disabled_dependencies=" << report.disabled_dependency_count
        << ";diagnostics=" << report.diagnostics.size()
        << ";causes=" << report.causes.size();
    return out.str();
}

std::string deep_simulation_core_readiness_digest(const DeepSimulationCoreReadinessReport& report) {
    std::ostringstream out;
    out << "deep_core_readiness"
        << ";processors=" << report.processor_count
        << ";processor_architecture=" << (report.processor_architecture_ready ? "ready" : "missing")
        << ";features=" << (report.feature_toggles_ready ? "ready" : "missing")
        << ";dependencies=" << (report.dependency_toggles_ready ? "ready" : "missing")
        << ";save_load=" << (report.save_load_ready ? "ready" : "missing")
        << ";replay=" << (report.replay_ready ? "ready" : "missing")
        << ";diagnostics=" << (report.diagnostics_ready ? "ready" : "missing")
        << ";engine_agnostic=" << (report.engine_agnostic_ready ? "ready" : "missing")
        << ";issues=" << report.issue_count;
    return out.str();
}

} // namespace clc::sim
