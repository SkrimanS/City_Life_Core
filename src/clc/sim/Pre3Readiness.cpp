#include "clc/sim/Pre3Readiness.hpp"

#include <sstream>

namespace clc::sim {

Pre3ReadinessReport make_pre3_readiness_report(
    bool snapshot_model_ready,
    bool authority_model_ready,
    bool replay_model_ready,
    bool economy_safety_ready,
    const MultiplayerLoadSnapshot& load_snapshot,
    bool large_world_plan_ready
) {
    Pre3ReadinessReport report{
        .snapshot_model_ready = snapshot_model_ready,
        .authority_model_ready = authority_model_ready,
        .replay_model_ready = replay_model_ready,
        .economy_safety_ready = economy_safety_ready,
        .load_diagnostics_ready = !load_snapshot.high_risk,
        .large_world_plan_ready = large_world_plan_ready,
    };
    const bool flags[] = {
        report.snapshot_model_ready,
        report.authority_model_ready,
        report.replay_model_ready,
        report.economy_safety_ready,
        report.load_diagnostics_ready,
        report.large_world_plan_ready,
    };
    for (const auto ready : flags) {
        if (ready) {
            ++report.ready_count;
        } else {
            ++report.missing_count;
        }
    }
    report.status = report.missing_count == 0 ? "ready" : (report.ready_count >= 4 ? "watch" : "blocked");
    return report;
}

data::ValidationReport validate_pre3_readiness_report(const Pre3ReadinessReport& report) {
    data::ValidationReport validation;
    if (report.ready_count + report.missing_count != 6) {
        validation.add_error("pre3_readiness.counts", "ready_count plus missing_count must equal six readiness gates");
    }
    if (report.status.empty()) {
        validation.add_error("pre3_readiness.status", "status must not be empty");
    }
    return validation;
}

std::string pre3_readiness_report_digest(const Pre3ReadinessReport& report) {
    std::ostringstream out;
    out << "pre3_readiness"
        << ";status=" << report.status
        << ";ready=" << report.ready_count
        << ";missing=" << report.missing_count
        << ";snapshot=" << (report.snapshot_model_ready ? "yes" : "no")
        << ";authority=" << (report.authority_model_ready ? "yes" : "no")
        << ";replay=" << (report.replay_model_ready ? "yes" : "no")
        << ";economy=" << (report.economy_safety_ready ? "yes" : "no")
        << ";load=" << (report.load_diagnostics_ready ? "yes" : "no")
        << ";large_world_plan=" << (report.large_world_plan_ready ? "yes" : "no");
    return out.str();
}

} // namespace clc::sim
