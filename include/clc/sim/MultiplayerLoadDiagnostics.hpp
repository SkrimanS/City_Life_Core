#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/ServerAuthoritative.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct MultiplayerLoadThresholds final {
    std::uint64_t watch_actions{1000};
    std::uint64_t high_actions{10000};
    std::uint64_t watch_snapshots{100};
    std::uint64_t high_snapshots{1000};
    std::uint64_t watch_events{1000};
    std::uint64_t high_events{10000};
};

struct MultiplayerLoadSnapshot final {
    std::uint64_t settlement_count{0};
    std::uint64_t caravan_count{0};
    std::uint64_t route_count{0};
    std::uint64_t contract_count{0};
    std::uint64_t event_count{0};
    std::uint64_t action_count{0};
    std::uint64_t rejected_action_count{0};
    std::uint64_t snapshot_count{0};
    bool watch_risk{false};
    bool high_risk{false};
};

[[nodiscard]] data::ValidationReport validate_multiplayer_load_thresholds(const MultiplayerLoadThresholds& thresholds);
[[nodiscard]] MultiplayerLoadSnapshot make_multiplayer_load_snapshot(
    const SimulationRuntime& runtime,
    const std::vector<ServerRuntimeActionAuditRecord>& audits,
    std::uint64_t snapshot_count,
    const MultiplayerLoadThresholds& thresholds = {}
);
[[nodiscard]] data::ValidationReport validate_multiplayer_load_snapshot(const MultiplayerLoadSnapshot& snapshot);
[[nodiscard]] std::string multiplayer_load_snapshot_digest(const MultiplayerLoadSnapshot& snapshot);

} // namespace clc::sim
