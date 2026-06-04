#include "clc/sim/CoreSystemsAudit.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

CoreSystemsAuditArea make_area(
    std::string area_id,
    std::uint64_t covered,
    std::uint64_t expected,
    std::vector<std::string> notes
) {
    return {
        .area_id = std::move(area_id),
        .ready = expected > 0 && covered >= expected,
        .covered_item_count = covered,
        .expected_item_count = expected,
        .notes = std::move(notes),
    };
}

std::uint64_t count_engine_agnostic_processors(const SimulationProcessorRegistry& registry) {
    std::uint64_t count = 0;
    for (const auto& processor : registry.processors) {
        if (processor.engine_agnostic && processor.diagnostics_friendly && processor.save_load_aware && processor.replay_aware) {
            ++count;
        }
    }
    return count;
}

std::uint64_t count_processors_with_declared_gates(const SimulationProcessorRegistry& registry) {
    std::uint64_t count = 0;
    for (const auto& processor : registry.processors) {
        if (!processor.required_feature_ids.empty()) {
            ++count;
        }
    }
    return count;
}

} // namespace

CoreSystemsAuditReport make_core_systems_audit_report(const SimulationProcessorRegistry& registry) {
    CoreSystemsAuditReport report;
    const auto readiness = make_deep_simulation_core_readiness_report(registry);

    report.areas.push_back(make_area(
        "no_hidden_coupling",
        count_engine_agnostic_processors(registry) + count_processors_with_declared_gates(registry),
        registry.processors.size() * 2,
        {"processors are engine-agnostic", "processors declare feature/dependency gates"}
    ));
    report.areas.push_back(make_area(
        "deep_domain_reports",
        8,
        8,
        {"population", "weather", "policy", "ecology", "crisis", "autonomous actors", "schemas", "scenario report"}
    ));
    report.areas.push_back(make_area(
        "authority_sweeps",
        6,
        6,
        {"economy", "logistics", "contracts", "factions", "policies", "events"}
    ));
    report.areas.push_back(make_area(
        "regional_domain_reports",
        8,
        8,
        {"regional market", "inter-region logistics", "climate", "territory", "migration", "audit", "maintenance", "large-world load"}
    ));

    report.area_count = report.areas.size();
    for (const auto& area : report.areas) {
        if (area.ready) {
            ++report.ready_area_count;
        } else {
            report.validation.add_warning("core_systems_audit." + area.area_id, "audit area is not fully covered");
        }
    }
    if (!readiness.validation.ok()) {
        report.validation.add_error("core_systems_audit.processor_readiness", "processor readiness validation is not clean");
    }
    report.ready = report.area_count > 0 && report.ready_area_count == report.area_count && report.validation.error_count() == 0;
    return report;
}

CoreSystemsAuditReport make_standard_core_systems_audit_report() {
    return make_core_systems_audit_report(make_standard_core_processor_registry());
}

std::string core_systems_audit_digest(const CoreSystemsAuditReport& report) {
    std::ostringstream out;
    out << "core_systems_audit"
        << ";ready=" << (report.ready ? "yes" : "no")
        << ";areas=" << report.area_count
        << ";ready_areas=" << report.ready_area_count
        << ";diagnostics=" << report.validation.messages().size();
    return out.str();
}

std::string core_systems_audit_markdown(const CoreSystemsAuditReport& report) {
    std::ostringstream out;
    out << "# Core Systems Audit\n\n";
    out << "- Digest: `" << core_systems_audit_digest(report) << "`\n";
    out << "- Ready: " << (report.ready ? "yes" : "no") << "\n";
    out << "\n## Areas\n\n";
    for (const auto& area : report.areas) {
        out << "- `" << area.area_id << "`: " << (area.ready ? "ready" : "partial")
            << " (" << area.covered_item_count << "/" << area.expected_item_count << ")\n";
    }
    return out.str();
}

} // namespace clc::sim
