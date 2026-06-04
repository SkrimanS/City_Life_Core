#pragma once

#include "clc/core/Time.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

enum class SnapshotVisibilityScope {
    full,
    public_view,
    faction,
    settlement,
};

struct SnapshotRequest final {
    SnapshotVisibilityScope scope{SnapshotVisibilityScope::full};
    std::string requester_actor_id{};
    std::string faction_id{};
    std::string settlement_id{};
    clc::GameTime::Tick requested_tick{0};
};

struct RuntimeSnapshotSummary final {
    std::string version{};
    SnapshotVisibilityScope scope{SnapshotVisibilityScope::full};
    std::string faction_id{};
    std::string settlement_id{};
    clc::GameTime::Tick tick{0};
    std::uint64_t day{0};
    std::uint64_t settlement_count{0};
    std::uint64_t visible_settlement_count{0};
    std::uint64_t caravan_count{0};
    std::uint64_t visible_caravan_count{0};
    std::uint64_t route_count{0};
    std::uint64_t market_price_count{0};
    std::uint64_t event_count{0};
    std::vector<std::string> visible_settlement_ids{};
    std::vector<std::string> visible_caravan_ids{};
};

[[nodiscard]] std::string_view snapshot_visibility_scope_name(SnapshotVisibilityScope scope) noexcept;
[[nodiscard]] data::ValidationReport validate_snapshot_request(const SnapshotRequest& request);
[[nodiscard]] RuntimeSnapshotSummary make_runtime_snapshot_summary(
    const SimulationRuntime& runtime,
    const SnapshotRequest& request = {}
);
[[nodiscard]] data::ValidationReport validate_runtime_snapshot_summary(const RuntimeSnapshotSummary& summary);
[[nodiscard]] std::string runtime_snapshot_summary_digest(const RuntimeSnapshotSummary& summary);

} // namespace clc::sim
