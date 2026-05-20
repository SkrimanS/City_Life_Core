#pragma once

#include "clc/core/EventLog.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

#include <cstdint>

namespace clc::sim {

struct RuntimeEventLogSummary final {
    std::uint64_t events_appended{0};
    std::uint64_t day_events{0};
    std::uint64_t caravan_events{0};
    std::uint64_t contract_events{0};
};

struct RuntimeEventLogAnalysis final {
    std::uint64_t total_events{0};
    std::uint64_t day_events{0};
    std::uint64_t caravan_progress_events{0};
    std::uint64_t caravan_arrival_events{0};
    std::uint64_t contract_fulfilled_events{0};
    std::uint64_t unknown_events{0};
    std::uint64_t first_tick{0};
    std::uint64_t last_tick{0};
};

struct RuntimeEventLogChecksum final {
    std::uint64_t event_count{0};
    std::uint64_t first_tick{0};
    std::uint64_t last_tick{0};
    std::uint64_t value{0};
};

struct RuntimeEventLogChecksumComparison final {
    RuntimeEventLogChecksum expected{};
    RuntimeEventLogChecksum actual{};
    bool event_count_matches{false};
    bool first_tick_matches{false};
    bool last_tick_matches{false};
    bool value_matches{false};

    [[nodiscard]] bool matches() const noexcept {
        return event_count_matches && first_tick_matches && last_tick_matches && value_matches;
    }
};

[[nodiscard]] RuntimeEventLogSummary append_runtime_day_report_events(
    clc::EventLog& log,
    const SimulationRuntimeDayReport& report
);

[[nodiscard]] RuntimeEventLogSummary append_runtime_run_events(
    clc::EventLog& log,
    const SimulationRuntimeRunResult& run
);

[[nodiscard]] RuntimeEventLogSummary append_runtime_arrival_contract_events(
    clc::EventLog& log,
    const SimulationRuntimeArrivalContractResult& result
);

[[nodiscard]] RuntimeEventLogAnalysis analyze_runtime_event_log(const clc::EventLog& log);
[[nodiscard]] data::ValidationReport validate_runtime_event_log_tick_order(const clc::EventLog& log);
[[nodiscard]] data::ValidationReport validate_runtime_event_log_known_types(const clc::EventLog& log);
[[nodiscard]] data::ValidationReport validate_runtime_event_log(const clc::EventLog& log);
[[nodiscard]] RuntimeEventLogChecksum calculate_runtime_event_log_checksum(const clc::EventLog& log);
[[nodiscard]] RuntimeEventLogChecksumComparison compare_runtime_event_log_checksums(
    RuntimeEventLogChecksum expected,
    RuntimeEventLogChecksum actual
);
[[nodiscard]] data::ValidationReport validate_runtime_event_log_checksum_match(
    RuntimeEventLogChecksum expected,
    RuntimeEventLogChecksum actual
);
[[nodiscard]] data::ValidationReport validate_runtime_event_logs_match(
    const clc::EventLog& expected,
    const clc::EventLog& actual
);

} // namespace clc::sim
