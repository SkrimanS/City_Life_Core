#include "clc/sim/MultiplayerLoadDiagnostics.hpp"

#include <sstream>

namespace clc::sim {

data::ValidationReport validate_multiplayer_load_thresholds(const MultiplayerLoadThresholds& thresholds) {
    data::ValidationReport report;
    if (thresholds.watch_actions > thresholds.high_actions) {
        report.add_error("multiplayer_load.actions", "watch_actions must not exceed high_actions");
    }
    if (thresholds.watch_snapshots > thresholds.high_snapshots) {
        report.add_error("multiplayer_load.snapshots", "watch_snapshots must not exceed high_snapshots");
    }
    if (thresholds.watch_events > thresholds.high_events) {
        report.add_error("multiplayer_load.events", "watch_events must not exceed high_events");
    }
    return report;
}

MultiplayerLoadSnapshot make_multiplayer_load_snapshot(
    const SimulationRuntime& runtime,
    const std::vector<ServerRuntimeActionAuditRecord>& audits,
    std::uint64_t snapshot_count,
    const MultiplayerLoadThresholds& thresholds
) {
    MultiplayerLoadSnapshot snapshot{
        .settlement_count = runtime.engine.settlements().size(),
        .caravan_count = runtime.caravans.caravans.size(),
        .route_count = runtime.routes.routes.size(),
        .contract_count = runtime.contracts.contracts.size(),
        .event_count = runtime.engine.events().size(),
        .action_count = audits.size(),
        .snapshot_count = snapshot_count,
    };
    for (const auto& audit : audits) {
        if (!audit.accepted) {
            ++snapshot.rejected_action_count;
        }
    }
    snapshot.high_risk = snapshot.action_count >= thresholds.high_actions
        || snapshot.snapshot_count >= thresholds.high_snapshots
        || snapshot.event_count >= thresholds.high_events;
    snapshot.watch_risk = snapshot.high_risk
        || snapshot.action_count >= thresholds.watch_actions
        || snapshot.snapshot_count >= thresholds.watch_snapshots
        || snapshot.event_count >= thresholds.watch_events;
    return snapshot;
}

data::ValidationReport validate_multiplayer_load_snapshot(const MultiplayerLoadSnapshot& snapshot) {
    data::ValidationReport report;
    if (snapshot.rejected_action_count > snapshot.action_count) {
        report.add_error("multiplayer_load.rejected_action_count", "rejected_action_count exceeds action_count");
    }
    return report;
}

std::string multiplayer_load_snapshot_digest(const MultiplayerLoadSnapshot& snapshot) {
    std::ostringstream out;
    out << "multiplayer_load"
        << ";settlements=" << snapshot.settlement_count
        << ";caravans=" << snapshot.caravan_count
        << ";routes=" << snapshot.route_count
        << ";contracts=" << snapshot.contract_count
        << ";events=" << snapshot.event_count
        << ";actions=" << snapshot.action_count
        << ";rejected_actions=" << snapshot.rejected_action_count
        << ";snapshots=" << snapshot.snapshot_count
        << ";watch=" << (snapshot.watch_risk ? "yes" : "no")
        << ";high=" << (snapshot.high_risk ? "yes" : "no");
    return out.str();
}

} // namespace clc::sim
