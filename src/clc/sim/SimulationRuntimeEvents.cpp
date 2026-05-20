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

RuntimeEventLogChecksum calculate_runtime_event_log_checksum(const clc::EventLog& log) {
    RuntimeEventLogChecksum checksum{};

    const auto& events = log.events();
    checksum.event_count = events.size();

    if (!events.empty()) {
        checksum.first_tick = events.front().tick;
        checksum.last_tick = events.back().tick;
    }

    for (const auto& event : events) {
        checksum.value ^= event.tick + 0x9e3779b97f4a7c15ULL;

        for (const auto character : event.type) {
            checksum.value = (checksum.value * 131ULL) + static_cast<unsigned char>(character);
        }

        for (const auto character : event.payload) {
            checksum.value = (checksum.value * 131ULL) + static_cast<unsigned char>(character);
        }
    }

    return checksum;
}

RuntimeEventLogChecksumComparison compare_runtime_event_log_checksums(
    RuntimeEventLogChecksum expected,
    RuntimeEventLogChecksum actual
) {
    RuntimeEventLogChecksumComparison comparison{};
    comparison.expected = expected;
    comparison.actual = actual;
    comparison.event_count_matches = expected.event_count == actual.event_count;
    comparison.first_tick_matches = expected.first_tick == actual.first_tick;
    comparison.last_tick_matches = expected.last_tick == actual.last_tick;
    comparison.value_matches = expected.value == actual.value;
    return comparison;
}

data::ValidationReport validate_runtime_event_log_checksum_match(
    RuntimeEventLogChecksum expected,
    RuntimeEventLogChecksum actual
) {
    data::ValidationReport report{};

    const auto comparison = compare_runtime_event_log_checksums(expected, actual);

    if (!comparison.matches()) {
        report.add_error("runtime event log checksum mismatch detected");
    }

    return report;
}

data::ValidationReport validate_runtime_event_logs_match(
    const clc::EventLog& expected,
    const clc::EventLog& actual
) {
    return validate_runtime_event_log_checksum_match(
        calculate_runtime_event_log_checksum(expected),
        calculate_runtime_event_log_checksum(actual)
    );
}

} // namespace clc::sim
