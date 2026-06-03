#pragma once

#include "clc/core/EventLog.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace clc::sim {

enum class ScaleRiskLevel {
    nominal,
    watch,
    high,
};

struct ScaleProfileThresholds final {
    std::uint64_t settlement_watch{250};
    std::uint64_t settlement_high{1000};
    std::uint64_t caravan_watch{1000};
    std::uint64_t caravan_high{5000};
    std::uint64_t contract_watch{1000};
    std::uint64_t contract_high{5000};
    std::uint64_t event_watch{10000};
    std::uint64_t event_high{50000};
    std::uint64_t serialized_line_watch{10000};
    std::uint64_t serialized_line_high{50000};
};

struct RuntimeScaleSnapshot final {
    std::uint64_t settlements{0};
    std::uint64_t buildings{0};
    std::uint64_t stored_resource_entries{0};
    std::uint64_t routes{0};
    std::uint64_t caravans{0};
    std::uint64_t caravan_cargo_entries{0};
    std::uint64_t factions{0};
    std::uint64_t reputations{0};
    std::uint64_t settlement_owners{0};
    std::uint64_t caravan_owners{0};
    std::uint64_t contracts{0};
    std::uint64_t ledger_entries{0};
    std::uint64_t engine_events{0};
    std::uint64_t runtime_event_log_entries{0};
    std::uint64_t serialized_world_state_lines{0};
    std::uint64_t estimated_entity_count{0};
    ScaleRiskLevel risk{ScaleRiskLevel::nominal};
    std::string highest_pressure_area{};
};

[[nodiscard]] std::string_view scale_risk_level_name(ScaleRiskLevel risk) noexcept;
[[nodiscard]] ScaleProfileThresholds default_scale_profile_thresholds() noexcept;

[[nodiscard]] RuntimeScaleSnapshot make_runtime_scale_snapshot(
    const SimulationRuntime& runtime,
    const clc::EventLog* event_log = nullptr,
    ScaleProfileThresholds thresholds = default_scale_profile_thresholds()
);

[[nodiscard]] data::ValidationReport validate_runtime_scale_snapshot(
    const RuntimeScaleSnapshot& snapshot,
    ScaleProfileThresholds thresholds = default_scale_profile_thresholds()
);

[[nodiscard]] std::string runtime_scale_snapshot_digest(const RuntimeScaleSnapshot& snapshot);
[[nodiscard]] std::string runtime_scale_snapshot_markdown(const RuntimeScaleSnapshot& snapshot);

} // namespace clc::sim
