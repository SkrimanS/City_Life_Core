#include "clc/sim/SimulationRuntimeTick.hpp"

#include <utility>

namespace clc::sim {

namespace {

void merge_validation(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::error) {
            target.add_error(message.path, message.message);
        } else {
            target.add_warning(message.path, message.message);
        }
    }
}

} // namespace

SimulationRuntimeDayReport advance_runtime_day(SimulationRuntime& runtime) {
    SimulationRuntimeDayReport report{};
    report.engine = runtime.engine.advance_day();

    for (const auto& caravan : runtime.caravans.caravans) {
        RuntimeCaravanTickReport caravan_report{};
        caravan_report.caravan_id = caravan.id;

        const auto advanced = advance_runtime_caravan_day(runtime, caravan.id);
        caravan_report.validation = advanced.validation;

        if (advanced.ok()) {
            caravan_report.advance = advanced.report;
        }

        merge_validation(report.validation, advanced.validation);
        report.caravans.push_back(std::move(caravan_report));
    }

    return report;
}

SimulationRuntimeRunSummary summarize_runtime_day_reports(const std::vector<SimulationRuntimeDayReport>& reports) {
    SimulationRuntimeRunSummary summary{};

    if (reports.empty()) {
        return summary;
    }

    summary.days_run = reports.size();
    summary.first_day = reports.front().engine.day;
    summary.last_day = reports.back().engine.day;

    for (const auto& report : reports) {
        summary.warnings += report.validation.warning_count();

        for (const auto& caravan : report.caravans) {
            ++summary.caravan_ticks;

            if (caravan.advance.arrived) {
                ++summary.caravan_arrivals;
            }
        }
    }

    return summary;
}

SimulationRuntimeRunResult run_runtime_days(SimulationRuntime& runtime, std::uint64_t day_count) {
    SimulationRuntimeRunResult result{};

    result.reports.reserve(static_cast<std::size_t>(day_count));

    for (std::uint64_t day = 0; day < day_count; ++day) {
        auto report = advance_runtime_day(runtime);
        merge_validation(result.validation, report.validation);
        result.reports.push_back(std::move(report));
    }

    result.summary = summarize_runtime_day_reports(result.reports);
    return result;
}

} // namespace clc::sim
