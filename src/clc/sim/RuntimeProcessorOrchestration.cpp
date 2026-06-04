#include "clc/sim/RuntimeProcessorOrchestration.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

bool is_regional_processor(const std::string& processor_id) {
    return processor_id == "regional" || processor_id == "logistics" || processor_id == "market" || processor_id == "factions";
}

void append_bounded_event(
    RuntimeProcessorOrchestrationReport& report,
    const RuntimeProcessorOrchestrationConfig& config,
    RuntimeProcessorEvent event
) {
    if (report.events.size() >= config.max_event_count) {
        ++report.dropped_event_count;
        return;
    }
    if (is_regional_processor(event.processor_id)) {
        ++report.regional_event_count;
    } else {
        ++report.deep_event_count;
    }
    report.events.push_back(std::move(event));
}

} // namespace

RuntimeProcessorOrchestrationConfig make_default_runtime_processor_orchestration_config() {
    return {};
}

RuntimeProcessorOrchestrationReport orchestrate_runtime_core_processors(
    SimulationRuntime& runtime,
    const SimulationProcessorRegistry& registry,
    const RuntimeProcessorOrchestrationConfig& config
) {
    RuntimeProcessorOrchestrationReport report;
    report.tick_before = runtime.time.current_tick();
    if (config.tick_step == 0) {
        report.validation.add_error("runtime_processor_orchestration.tick_step", "tick_step must be greater than zero");
        return report;
    }
    if (config.max_event_count == 0) {
        report.validation.add_warning("runtime_processor_orchestration.max_event_count", "max_event_count is zero; all orchestration events will be dropped");
    }

    report.processor_run = make_simulation_processor_run_report(registry);
    report.tick = advance_runtime_ticks(runtime, config.tick_step);
    report.tick_after = report.tick.tick_after;
    report.core_systems = evaluate_runtime_core_systems(runtime, config.core_systems);

    for (const auto& processor_id : report.processor_run.deterministic_order) {
        append_bounded_event(report, config, {
            .tick = report.tick_after,
            .processor_id = processor_id,
            .event_type = "processor.evaluated",
            .payload = "runtime core processor evaluated in deterministic order",
        });
    }
    if (report.core_systems.diagnostic_count > 0) {
        append_bounded_event(report, config, {
            .tick = report.tick_after,
            .processor_id = "runtime",
            .event_type = "runtime.diagnostics",
            .payload = runtime_core_systems_digest(report.core_systems),
        });
    }
    for (const auto& arrived_caravan_id : report.tick.arrived_caravan_ids) {
        append_bounded_event(report, config, {
            .tick = report.tick_after,
            .processor_id = "logistics",
            .event_type = "runtime.caravan_arrived",
            .payload = arrived_caravan_id,
        });
    }

    if (report.processor_run.skipped_processor_count > 0) {
        report.validation.add_warning("runtime_processor_orchestration.processor_run", "one or more processors were skipped");
    }
    return report;
}

RuntimeProcessorOrchestrationReport orchestrate_standard_runtime_core_processors(
    SimulationRuntime& runtime,
    const RuntimeProcessorOrchestrationConfig& config
) {
    return orchestrate_runtime_core_processors(runtime, make_standard_core_processor_registry(), config);
}

std::string runtime_processor_orchestration_digest(const RuntimeProcessorOrchestrationReport& report) {
    std::ostringstream out;
    out << "runtime_processor_orchestration"
        << ";tick_before=" << report.tick_before
        << ";tick_after=" << report.tick_after
        << ";processors=" << report.processor_run.enabled_processor_count
        << ";events=" << report.events.size()
        << ";dropped=" << report.dropped_event_count
        << ";deep_events=" << report.deep_event_count
        << ";regional_events=" << report.regional_event_count
        << ";diagnostics=" << report.core_systems.diagnostic_count
        << ";warnings=" << report.validation.warning_count()
        << ";errors=" << report.validation.error_count();
    return out.str();
}

std::string runtime_processor_orchestration_markdown(const RuntimeProcessorOrchestrationReport& report) {
    std::ostringstream out;
    out << "# Runtime Processor Orchestration\n\n";
    out << "- Digest: `" << runtime_processor_orchestration_digest(report) << "`\n";
    out << "- Processor run: `" << simulation_processor_run_report_digest(report.processor_run) << "`\n";
    out << "- Runtime core systems: `" << runtime_core_systems_digest(report.core_systems) << "`\n";
    out << "- Events: " << report.events.size() << "\n";
    out << "- Dropped events: " << report.dropped_event_count << "\n";
    return out.str();
}

} // namespace clc::sim
