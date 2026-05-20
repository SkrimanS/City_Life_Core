#pragma once

#include "clc/core/EventLog.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

namespace clc::sim {

struct RuntimeEventLogSummary final {
    std::uint64_t events_appended{0};
    std::uint64_t day_events{0};
    std::uint64_t caravan_events{0};
    std::uint64_t contract_events{0};
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

} // namespace clc::sim
