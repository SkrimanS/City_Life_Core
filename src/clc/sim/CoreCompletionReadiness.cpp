#include "clc/sim/CoreCompletionReadiness.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

CoreCompletionArea make_area(
    std::string area_id,
    std::string title,
    bool ready,
    std::uint64_t completed_items,
    std::uint64_t total_items,
    std::vector<std::string> remaining_work
) {
    return CoreCompletionArea{
        .area_id = std::move(area_id),
        .title = std::move(title),
        .status = ready ? CoreCompletionStatus::ready : (completed_items == 0 ? CoreCompletionStatus::missing : CoreCompletionStatus::partial),
        .completed_items = completed_items,
        .total_items = total_items,
        .remaining_work = ready ? std::vector<std::string>{} : std::move(remaining_work),
    };
}

} // namespace

std::string_view core_completion_status_name(CoreCompletionStatus status) noexcept {
    switch (status) {
    case CoreCompletionStatus::missing:
        return "missing";
    case CoreCompletionStatus::partial:
        return "partial";
    case CoreCompletionStatus::ready:
        return "ready";
    }
    return "missing";
}

CoreCompletionReadinessInput make_core_completion_readiness_input(const CoreCompletionEvidence& evidence) {
    const auto processor_ready =
        evidence.processor.processor_architecture_ready
        && evidence.processor.feature_toggles_ready
        && evidence.processor.dependency_toggles_ready
        && evidence.processor.diagnostics_ready
        && evidence.processor.engine_agnostic_ready
        && evidence.processor.validation.ok();

    const auto runtime_core_report_available = !evidence.runtime_core_systems.population.settlement_id.empty()
        || !evidence.runtime_core_systems.regional_market.region_id.empty()
        || evidence.runtime_core_systems.diagnostic_count > 0;

    return CoreCompletionReadinessInput{
        .architecture_ready = processor_ready,
        .deep_systems_ready = processor_ready && evidence.deep_domain_reports_available,
        .runtime_integration_ready = evidence.runtime_workflow_available && runtime_core_report_available && evidence.deep_domain_reports_available && evidence.regional_reports_available,
        .save_load_replay_ready = evidence.save_replay_available && evidence.processor.save_load_ready && evidence.processor.replay_ready && evidence.replay_coverage.ready,
        .authority_ready = evidence.authority_available,
        .regional_ready = evidence.regional_reports_available && evidence.platform.ready,
        .c_abi_ready = evidence.c_abi_handoff_available,
        .developer_handoff_ready = evidence.developer_handoff_available && evidence.platform.ready,
        .packaging_ready = evidence.package_validation_available && evidence.sdk_handoff_manifest.ready,
    };
}

CoreCompletionReadinessReport make_core_completion_readiness_report(const CoreCompletionReadinessInput& input) {
    CoreCompletionReadinessReport report;
    report.areas.push_back(make_area(
        "architecture",
        "Core architecture",
        input.architecture_ready,
        input.architecture_ready ? 7 : 5,
        7,
        {"runtime presets for all systems", "final no-hidden-coupling audit"}
    ));
    report.areas.push_back(make_area(
        "deep_systems",
        "v1.x deep simulation systems",
        input.deep_systems_ready,
        input.deep_systems_ready ? 12 : 8,
        12,
        {"runtime processors for report-only systems", "scenario/forecast completeness"}
    ));
    report.areas.push_back(make_area(
        "runtime",
        "Runtime integration",
        input.runtime_integration_ready,
        input.runtime_integration_ready ? 6 : 3,
        6,
        {"deep systems in tick loop", "regional systems in tick loop", "bounded event streams"}
    ));
    report.areas.push_back(make_area(
        "save_replay",
        "Save/load/replay/migration",
        input.save_load_replay_ready,
        input.save_load_replay_ready ? 7 : 3,
        7,
        {"deep save/replay fixtures", "regional save/replay coverage", "migration fixtures"}
    ));
    report.areas.push_back(make_area(
        "authority",
        "Authority and multiplayer safety",
        input.authority_ready,
        input.authority_ready ? 8 : 5,
        8,
        {"economy consistency sweep", "logistics/contract authority sweep", "faction/policy/event authority sweep"}
    ));
    report.areas.push_back(make_area(
        "regional",
        "Regional and large-world systems",
        input.regional_ready,
        input.regional_ready ? 9 : 6,
        9,
        {"large-world persistence", "world history retention", "load benchmarks"}
    ));
    report.areas.push_back(make_area(
        "c_abi",
        "C ABI and foreign-language readiness",
        input.c_abi_ready,
        input.c_abi_ready ? 6 : 3,
        6,
        {"deep diagnostics C ABI", "regional diagnostics C ABI", "Unity/C# handoff parity"}
    ));
    report.areas.push_back(make_area(
        "handoff",
        "Developer handoff",
        input.developer_handoff_ready,
        input.developer_handoff_ready ? 7 : 5,
        7,
        {"profile-specific starter examples", "installed SDK handoff validation"}
    ));
    report.areas.push_back(make_area(
        "packaging",
        "Packaging and release readiness",
        input.packaging_ready,
        input.packaging_ready ? 5 : 2,
        5,
        {"release docs alignment", "full validation", "final ZIP package"}
    ));

    for (const auto& area : report.areas) {
        switch (area.status) {
        case CoreCompletionStatus::ready:
            ++report.ready_area_count;
            break;
        case CoreCompletionStatus::partial:
            ++report.partial_area_count;
            report.validation.add_warning("core_completion." + area.area_id, "area is partially complete");
            break;
        case CoreCompletionStatus::missing:
            ++report.missing_area_count;
            report.validation.add_error("core_completion." + area.area_id, "area is missing");
            break;
        }
    }
    report.ready = report.missing_area_count == 0 && report.partial_area_count == 0;
    return report;
}

CoreCompletionReadinessReport make_core_completion_readiness_report(const CoreCompletionEvidence& evidence) {
    auto report = make_core_completion_readiness_report(make_core_completion_readiness_input(evidence));
    if (!evidence.processor.validation.ok()) {
        report.validation.add_error("core_completion.processor_evidence", "processor readiness validation is not clean");
    }
    if (!evidence.platform.validation.ok()) {
        report.validation.add_error("core_completion.platform_evidence", "platform readiness validation is not clean");
    }
    if (!evidence.replay_coverage.validation.ok()) {
        report.validation.add_error("core_completion.replay_coverage_evidence", "deep replay coverage validation is not clean");
    }
    if (!evidence.sdk_handoff_manifest.validation.ok()) {
        report.validation.add_error("core_completion.sdk_handoff_manifest_evidence", "SDK handoff manifest validation is not clean");
    }
    return report;
}

std::string core_completion_readiness_digest(const CoreCompletionReadinessReport& report) {
    std::ostringstream out;
    out << "core_completion"
        << ";ready=" << (report.ready ? "yes" : "no")
        << ";ready_areas=" << report.ready_area_count
        << ";partial_areas=" << report.partial_area_count
        << ";missing_areas=" << report.missing_area_count
        << ";areas=" << report.areas.size()
        << ";diagnostics=" << report.validation.messages().size();
    return out.str();
}

std::string core_completion_readiness_markdown(const CoreCompletionReadinessReport& report) {
    std::ostringstream out;
    out << "# Core Completion Readiness\n\n";
    out << "- Ready: " << (report.ready ? "yes" : "no") << "\n";
    out << "- Ready areas: " << report.ready_area_count << "\n";
    out << "- Partial areas: " << report.partial_area_count << "\n";
    out << "- Missing areas: " << report.missing_area_count << "\n";
    out << "\n## Areas\n\n";
    for (const auto& area : report.areas) {
        out << "- `" << area.area_id << "`: " << core_completion_status_name(area.status)
            << " (" << area.completed_items << "/" << area.total_items << ")\n";
    }
    return out.str();
}

} // namespace clc::sim
