#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/ActionAuthority.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct MultiplayerCommandLogEntry final {
    ServerRuntimeActionEnvelope envelope{};
    ActorAuthority actor{};
};

struct MultiplayerCommandLogSummary final {
    std::uint64_t entry_count{0};
    std::uint64_t accepted_count{0};
    std::uint64_t rejected_count{0};
    std::uint64_t first_sequence{0};
    std::uint64_t last_sequence{0};
    bool contiguous{true};
    bool strictly_increasing{true};
};

struct MultiplayerReplayResult final {
    MultiplayerCommandLogSummary summary{};
    std::vector<ServerRuntimeActionAuditRecord> audits{};
    data::ValidationReport validation{};
};

[[nodiscard]] MultiplayerCommandLogSummary summarize_multiplayer_command_log(
    const std::vector<MultiplayerCommandLogEntry>& entries
);
[[nodiscard]] data::ValidationReport validate_multiplayer_command_log(
    const std::vector<MultiplayerCommandLogEntry>& entries,
    std::uint64_t expected_first_sequence = 1
);
[[nodiscard]] MultiplayerReplayResult replay_multiplayer_command_log(
    SimulationRuntime& runtime,
    const std::vector<MultiplayerCommandLogEntry>& entries,
    const ActionAuthorityPolicy& policy = {}
);
[[nodiscard]] std::string multiplayer_command_log_summary_digest(const MultiplayerCommandLogSummary& summary);

} // namespace clc::sim
