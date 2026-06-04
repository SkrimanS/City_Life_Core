#include "clc/sim/SnapshotSync.hpp"

#include "clc/core/Version.hpp"

#include <algorithm>
#include <sstream>
#include <string>

namespace clc::sim {
namespace {

bool push_if_unique(std::vector<std::string>& values, std::string value) {
    if (std::find(values.begin(), values.end(), value) != values.end()) {
        return false;
    }
    values.push_back(std::move(value));
    return true;
}

} // namespace

std::string_view snapshot_visibility_scope_name(SnapshotVisibilityScope scope) noexcept {
    switch (scope) {
    case SnapshotVisibilityScope::full: return "full";
    case SnapshotVisibilityScope::public_view: return "public";
    case SnapshotVisibilityScope::faction: return "faction";
    case SnapshotVisibilityScope::settlement: return "settlement";
    }
    return "unknown";
}

data::ValidationReport validate_snapshot_request(const SnapshotRequest& request) {
    data::ValidationReport report;
    if (request.scope == SnapshotVisibilityScope::faction && request.faction_id.empty()) {
        report.add_error("snapshot.faction_id", "faction_id must not be empty for faction snapshots");
    }
    if (request.scope == SnapshotVisibilityScope::settlement && request.settlement_id.empty()) {
        report.add_error("snapshot.settlement_id", "settlement_id must not be empty for settlement snapshots");
    }
    return report;
}

RuntimeSnapshotSummary make_runtime_snapshot_summary(
    const SimulationRuntime& runtime,
    const SnapshotRequest& request
) {
    RuntimeSnapshotSummary summary{
        .version = std::string{clc::core_version_string()},
        .scope = request.scope,
        .faction_id = request.faction_id,
        .settlement_id = request.settlement_id,
        .tick = runtime.time.current_tick(),
        .day = runtime.engine.current_day(),
        .settlement_count = runtime.engine.settlements().size(),
        .caravan_count = runtime.caravans.caravans.size(),
        .route_count = runtime.routes.routes.size(),
        .market_price_count = runtime.engine.snapshot().market.prices.size(),
        .event_count = runtime.engine.events().size(),
    };

    for (const auto& settlement : runtime.engine.settlements()) {
        bool visible = request.scope == SnapshotVisibilityScope::full || request.scope == SnapshotVisibilityScope::public_view;
        if (request.scope == SnapshotVisibilityScope::settlement) {
            visible = settlement.id == request.settlement_id;
        } else if (request.scope == SnapshotVisibilityScope::faction) {
            visible = settlement_owner(runtime.ownership, settlement.id) == request.faction_id;
        }
        if (visible) {
            push_if_unique(summary.visible_settlement_ids, settlement.id);
        }
    }

    for (const auto& caravan : runtime.caravans.caravans) {
        bool visible = request.scope == SnapshotVisibilityScope::full || request.scope == SnapshotVisibilityScope::public_view;
        if (request.scope == SnapshotVisibilityScope::settlement) {
            visible = caravan.origin_settlement_id == request.settlement_id || caravan.destination_settlement_id == request.settlement_id;
        } else if (request.scope == SnapshotVisibilityScope::faction) {
            visible = caravan_owner(runtime.ownership, caravan.id) == request.faction_id;
        }
        if (visible) {
            push_if_unique(summary.visible_caravan_ids, caravan.id);
        }
    }

    std::sort(summary.visible_settlement_ids.begin(), summary.visible_settlement_ids.end());
    std::sort(summary.visible_caravan_ids.begin(), summary.visible_caravan_ids.end());
    summary.visible_settlement_count = summary.visible_settlement_ids.size();
    summary.visible_caravan_count = summary.visible_caravan_ids.size();
    return summary;
}

data::ValidationReport validate_runtime_snapshot_summary(const RuntimeSnapshotSummary& summary) {
    data::ValidationReport report;
    if (summary.version.empty()) {
        report.add_error("snapshot.version", "version must not be empty");
    }
    if (summary.visible_settlement_count > summary.settlement_count) {
        report.add_error("snapshot.visible_settlement_count", "visible settlement count exceeds settlement count");
    }
    if (summary.visible_caravan_count > summary.caravan_count) {
        report.add_error("snapshot.visible_caravan_count", "visible caravan count exceeds caravan count");
    }
    return report;
}

std::string runtime_snapshot_summary_digest(const RuntimeSnapshotSummary& summary) {
    std::ostringstream out;
    out << "runtime_snapshot"
        << ";version=" << summary.version
        << ";scope=" << snapshot_visibility_scope_name(summary.scope)
        << ";tick=" << summary.tick
        << ";day=" << summary.day
        << ";settlements=" << summary.settlement_count
        << ";visible_settlements=" << summary.visible_settlement_count
        << ";caravans=" << summary.caravan_count
        << ";visible_caravans=" << summary.visible_caravan_count
        << ";routes=" << summary.route_count
        << ";market_prices=" << summary.market_price_count
        << ";events=" << summary.event_count;
    if (!summary.faction_id.empty()) {
        out << ";faction=" << summary.faction_id;
    }
    if (!summary.settlement_id.empty()) {
        out << ";settlement=" << summary.settlement_id;
    }
    return out.str();
}

} // namespace clc::sim
