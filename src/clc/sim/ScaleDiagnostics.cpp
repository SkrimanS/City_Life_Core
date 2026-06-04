#include "clc/sim/ScaleDiagnostics.hpp"

#include "clc/sim/SimulationPersistence.hpp"

#include <algorithm>
#include <sstream>
#include <string>

namespace clc::sim {
namespace {

std::uint64_t count_serialized_lines(std::string_view content) {
    if (content.empty()) {
        return 0;
    }
    std::uint64_t lines = 0;
    for (const auto character : content) {
        if (character == '\n') {
            ++lines;
        }
    }
    return content.back() == '\n' ? lines : lines + 1;
}

ScaleRiskLevel risk_for(std::uint64_t value, std::uint64_t watch, std::uint64_t high) noexcept {
    if (value >= high) {
        return ScaleRiskLevel::high;
    }
    if (value >= watch) {
        return ScaleRiskLevel::watch;
    }
    return ScaleRiskLevel::nominal;
}

void raise_risk(RuntimeScaleSnapshot& snapshot, ScaleRiskLevel risk, std::string area) {
    if (static_cast<int>(risk) > static_cast<int>(snapshot.risk)) {
        snapshot.risk = risk;
        snapshot.highest_pressure_area = std::move(area);
    }
}

std::uint64_t saturating_add(std::uint64_t lhs, std::uint64_t rhs) noexcept {
    if (rhs > UINT64_MAX - lhs) {
        return UINT64_MAX;
    }
    return lhs + rhs;
}

} // namespace

std::string_view scale_risk_level_name(ScaleRiskLevel risk) noexcept {
    switch (risk) {
    case ScaleRiskLevel::nominal:
        return "nominal";
    case ScaleRiskLevel::watch:
        return "watch";
    case ScaleRiskLevel::high:
        return "high";
    }
    return "unknown";
}

ScaleProfileThresholds default_scale_profile_thresholds() noexcept {
    return ScaleProfileThresholds{};
}

RuntimeScaleSnapshot make_runtime_scale_snapshot(
    const SimulationRuntime& runtime,
    const clc::EventLog* event_log,
    ScaleProfileThresholds thresholds
) {
    RuntimeScaleSnapshot snapshot;
    const auto world = capture_simulation_world_state(runtime);

    snapshot.settlements = world.engine.settlements.size();
    for (const auto& settlement : world.engine.settlements) {
        snapshot.buildings = saturating_add(snapshot.buildings, settlement.buildings.size());
        snapshot.stored_resource_entries = saturating_add(snapshot.stored_resource_entries, settlement.storage.entries().size());
    }

    snapshot.routes = world.routes.routes.size();
    snapshot.caravans = world.caravans.caravans.size();
    for (const auto& caravan : world.caravans.caravans) {
        snapshot.caravan_cargo_entries = saturating_add(snapshot.caravan_cargo_entries, caravan.cargo.entries().size());
    }

    snapshot.factions = world.factions.factions.size();
    snapshot.reputations = world.factions.reputations.size();
    snapshot.settlement_owners = world.ownership.settlements.size();
    snapshot.caravan_owners = world.ownership.caravans.size();
    snapshot.contracts = world.contracts.contracts.size();
    snapshot.ledger_entries = world.ledger_entries.size();
    snapshot.engine_events = world.engine.events.size();
    snapshot.runtime_event_log_entries = event_log == nullptr ? 0 : event_log->size();
    snapshot.serialized_world_state_lines = count_serialized_lines(serialize_simulation_world_state(world));

    snapshot.estimated_entity_count = snapshot.settlements;
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.buildings);
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.stored_resource_entries);
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.routes);
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.caravans);
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.caravan_cargo_entries);
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.factions);
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.reputations);
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.contracts);
    snapshot.estimated_entity_count = saturating_add(snapshot.estimated_entity_count, snapshot.ledger_entries);

    raise_risk(snapshot, risk_for(snapshot.settlements, thresholds.settlement_watch, thresholds.settlement_high), "settlements");
    raise_risk(snapshot, risk_for(snapshot.caravans, thresholds.caravan_watch, thresholds.caravan_high), "caravans");
    raise_risk(snapshot, risk_for(snapshot.contracts, thresholds.contract_watch, thresholds.contract_high), "contracts");
    raise_risk(snapshot, risk_for(snapshot.runtime_event_log_entries, thresholds.event_watch, thresholds.event_high), "runtime_event_log");
    raise_risk(snapshot, risk_for(snapshot.serialized_world_state_lines, thresholds.serialized_line_watch, thresholds.serialized_line_high), "serialized_world_state");

    if (snapshot.highest_pressure_area.empty()) {
        snapshot.highest_pressure_area = "none";
    }
    return snapshot;
}

data::ValidationReport validate_runtime_scale_snapshot(
    const RuntimeScaleSnapshot& snapshot,
    ScaleProfileThresholds thresholds
) {
    data::ValidationReport report;
    if (snapshot.settlements >= thresholds.settlement_high) {
        report.add_error("runtime.scale.settlements", "settlement count is above high scale threshold");
    } else if (snapshot.settlements >= thresholds.settlement_watch) {
        report.add_warning("runtime.scale.settlements", "settlement count is above watch scale threshold");
    }
    if (snapshot.caravans >= thresholds.caravan_high) {
        report.add_error("runtime.scale.caravans", "caravan count is above high scale threshold");
    } else if (snapshot.caravans >= thresholds.caravan_watch) {
        report.add_warning("runtime.scale.caravans", "caravan count is above watch scale threshold");
    }
    if (snapshot.contracts >= thresholds.contract_high) {
        report.add_error("runtime.scale.contracts", "contract count is above high scale threshold");
    } else if (snapshot.contracts >= thresholds.contract_watch) {
        report.add_warning("runtime.scale.contracts", "contract count is above watch scale threshold");
    }
    if (snapshot.runtime_event_log_entries >= thresholds.event_high) {
        report.add_error("runtime.scale.events", "runtime event-log count is above high scale threshold");
    } else if (snapshot.runtime_event_log_entries >= thresholds.event_watch) {
        report.add_warning("runtime.scale.events", "runtime event-log count is above watch scale threshold");
    }
    if (snapshot.serialized_world_state_lines >= thresholds.serialized_line_high) {
        report.add_error("runtime.scale.serialization", "serialized world-state line count is above high scale threshold");
    } else if (snapshot.serialized_world_state_lines >= thresholds.serialized_line_watch) {
        report.add_warning("runtime.scale.serialization", "serialized world-state line count is above watch scale threshold");
    }
    return report;
}

std::string runtime_scale_snapshot_digest(const RuntimeScaleSnapshot& snapshot) {
    std::ostringstream out;
    out << "runtime_scale"
        << ";risk=" << scale_risk_level_name(snapshot.risk)
        << ";area=" << snapshot.highest_pressure_area
        << ";settlements=" << snapshot.settlements
        << ";buildings=" << snapshot.buildings
        << ";storage_entries=" << snapshot.stored_resource_entries
        << ";routes=" << snapshot.routes
        << ";caravans=" << snapshot.caravans
        << ";contracts=" << snapshot.contracts
        << ";ledger=" << snapshot.ledger_entries
        << ";engine_events=" << snapshot.engine_events
        << ";runtime_events=" << snapshot.runtime_event_log_entries
        << ";serialized_lines=" << snapshot.serialized_world_state_lines
        << ";entities=" << snapshot.estimated_entity_count;
    return out.str();
}

std::string runtime_scale_snapshot_markdown(const RuntimeScaleSnapshot& snapshot) {
    std::ostringstream out;
    out << "# Runtime Scale Snapshot\n\n"
        << "- Risk: " << scale_risk_level_name(snapshot.risk) << '\n'
        << "- Highest pressure area: " << snapshot.highest_pressure_area << '\n'
        << "- Settlements: " << snapshot.settlements << '\n'
        << "- Buildings: " << snapshot.buildings << '\n'
        << "- Storage entries: " << snapshot.stored_resource_entries << '\n'
        << "- Routes: " << snapshot.routes << '\n'
        << "- Caravans: " << snapshot.caravans << '\n'
        << "- Contracts: " << snapshot.contracts << '\n'
        << "- Ledger entries: " << snapshot.ledger_entries << '\n'
        << "- Engine events: " << snapshot.engine_events << '\n'
        << "- Runtime event-log entries: " << snapshot.runtime_event_log_entries << '\n'
        << "- Serialized world-state lines: " << snapshot.serialized_world_state_lines << '\n'
        << "- Estimated entity count: " << snapshot.estimated_entity_count << '\n';
    return out.str();
}

} // namespace clc::sim
