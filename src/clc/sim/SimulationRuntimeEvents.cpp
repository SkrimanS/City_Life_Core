#include "clc/sim/SimulationRuntimeEvents.hpp"

#include <string>
#include <utility>

namespace clc::sim {
namespace {

void append_event(
    clc::EventLog& log,
    RuntimeEventLogSummary& summary,
    std::uint64_t tick,
    std::string type,
    std::string payload
) {
    log.append(tick, std::move(type), std::move(payload));
    ++summary.events_appended;
}

bool is_known_runtime_event_type(const std::string& type) {
    return type == "runtime.day.completed"
        || type == "runtime.caravan.progress"
        || type == "runtime.caravan.arrived"
        || type == "runtime.contract.fulfilled";
}

} // namespace

RuntimeEventLogSummary append_runtime_day_report_events(
    clc::EventLog& log,
    const SimulationRuntimeDayReport& report
) {
    RuntimeEventLogSummary summary{};

    append_event(
        log,
        summary,
        report.engine.day,
        "runtime.day.completed",
        "day=" + std::to_string(report.engine.day)
    );
    ++summary.day_events;

    for (const auto& caravan : report.caravans) {
        append_event(
            log,
            summary,
            report.engine.day,
            caravan.advance.arrived ? "runtime.caravan.arrived" : "runtime.caravan.progress",
            caravan.caravan_id
        );
        ++summary.caravan_events;
    }

    return summary;
}

RuntimeEventLogSummary append_runtime_run_events(
    clc::EventLog& log,
    const SimulationRuntimeRunResult& run
) {
    RuntimeEventLogSummary summary{};

    for (const auto& report : run.reports) {
        const auto partial = append_runtime_day_report_events(log, report);
        summary.events_appended += partial.events_appended;
        summary.day_events += partial.day_events;
        summary.caravan_events += partial.caravan_events;
    }

    return summary;
}

RuntimeEventLogSummary append_runtime_arrival_contract_events(
    clc::EventLog& log,
    const SimulationRuntimeArrivalContractResult& result
) {
    auto summary = append_runtime_run_events(log, result.arrival.run);

    if (result.fulfillment.ok()) {
        append_event(
            log,
            summary,
            result.arrival.arrival_day,
            "runtime.contract.fulfilled",
            result.fulfillment.contract_id
        );
        ++summary.contract_events;
    }

    return summary;
}

RuntimeEventLogAnalysis analyze_runtime_event_log(const clc::EventLog& log) {
    RuntimeEventLogAnalysis analysis{};

    const auto& events = log.events();
    analysis.total_events = events.size();

    if (!events.empty()) {
        analysis.first_tick = events.front().tick;
        analysis.last_tick = events.back().tick;
    }

    for (const auto& event : events) {
        if (event.type == "runtime.day.completed") {
            ++analysis.day_events;
        } else if (event.type == "runtime.caravan.progress") {
            ++analysis.caravan_progress_events;
        } else if (event.type == "runtime.caravan.arrived") {
            ++analysis.caravan_arrival_events;
        } else if (event.type == "runtime.contract.fulfilled") {
            ++analysis.contract_fulfilled_events;
        } else {
            ++analysis.unknown_events;
        }
    }

    return analysis;
}

data::ValidationReport validate_runtime_event_log_tick_order(const clc::EventLog& log) {
    data::ValidationReport report{};

    const auto& events = log.events();

    for (std::size_t index = 1; index < events.size(); ++index) {
        if (events[index].tick < events[index - 1].tick) {
            report.add_error("runtime event log tick order regression detected");
            return report;
        }
    }

    return report;
}

data::ValidationReport validate_runtime_event_log_known_types(const clc::EventLog& log) {
    data::ValidationReport report{};

    for (const auto& event : log.events()) {
        if (!is_known_runtime_event_type(event.type)) {
            report.add_error("runtime event log contains unknown event type");
            return report;
        }
    }

    return report;
}

data::ValidationReport validate_runtime_event_log(const clc::EventLog& log) {
    auto report = validate_runtime_event_log_tick_order(log);

    if (!report.ok()) {
        return report;
    }

    return validate_runtime_event_log_known_types(log);
}

} // namespace clc::sim
