#include "clc/sim/SimulationRuntimeTick.hpp"

#include <algorithm>
#include <cstddef>
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

SimulationRuntimeTickReport advance_runtime_ticks(SimulationRuntime& runtime, clc::GameTime::Tick ticks) {
    SimulationRuntimeTickReport report{};
    report.tick_before = runtime.time.current_tick();
    report.elapsed_ticks = ticks;

    runtime.time.advance(ticks);
    report.tick_after = runtime.time.current_tick();

    for (auto& caravan : runtime.caravans.caravans) {
        RuntimeCaravanTickReport caravan_report{};
        caravan_report.caravan_id = caravan.id;

        const auto advanced = advance_caravan_ticks(caravan, ticks);
        caravan_report.advance = advanced;

        if (advanced.arrived && advanced.ticks_elapsed > 0) {
            report.arrived_caravan_ids.push_back(advanced.caravan_id);
        }

        report.caravans.push_back(std::move(caravan_report));
    }

    report.contracts = fail_overdue_open_contracts_at_tick(runtime.contracts, report.tick_after);
    return report;
}

SimulationRuntimeDayReport advance_runtime_day(SimulationRuntime& runtime) {
    SimulationRuntimeDayReport report{};
    report.engine = runtime.engine.advance_day();
    report.ticks = advance_runtime_ticks(runtime, clc::ticks_per_day());
    report.caravans = report.ticks.caravans;
    report.arrived_caravan_ids = report.ticks.arrived_caravan_ids;
    report.contracts = report.ticks.contracts;
    merge_validation(report.validation, report.ticks.validation);

    return report;
}

SimulationRuntimeTickRunSummary summarize_runtime_tick_reports(const std::vector<SimulationRuntimeTickReport>& reports) {
    SimulationRuntimeTickRunSummary summary{};

    if (reports.empty()) {
        return summary;
    }

    summary.tick_steps = reports.size();
    summary.first_tick = reports.front().tick_after;
    summary.last_tick = reports.back().tick_after;

    for (const auto& report : reports) {
        summary.ticks_elapsed += report.elapsed_ticks;
        summary.warnings += report.validation.warning_count();
        summary.contract_failures += report.contracts.failed_count;

        for (const auto& caravan : report.caravans) {
            ++summary.caravan_ticks;
            if (caravan.advance.arrived && caravan.advance.ticks_elapsed > 0) {
                ++summary.caravan_arrivals;
            }
        }
    }

    return summary;
}

SimulationRuntimeTickRunResult run_runtime_ticks(
    SimulationRuntime& runtime,
    clc::GameTime::Tick total_ticks,
    clc::GameTime::Tick step_ticks
) {
    SimulationRuntimeTickRunResult result{};

    if (step_ticks == 0) {
        result.validation.add_error("simulation.runtime.tick", "step_ticks must be greater than zero");
        return result;
    }

    clc::GameTime::Tick elapsed = 0;
    while (elapsed < total_ticks) {
        const auto remaining = total_ticks - elapsed;
        const auto step = std::min(step_ticks, remaining);
        auto report = advance_runtime_ticks(runtime, step);
        elapsed += step;
        merge_validation(result.validation, report.validation);
        result.reports.push_back(std::move(report));
    }

    result.summary = summarize_runtime_tick_reports(result.reports);
    return result;
}

SimulationRuntimeTickRunUntilArrivalResult run_runtime_until_first_caravan_arrival_by_ticks(
    SimulationRuntime& runtime,
    clc::GameTime::Tick max_ticks,
    clc::GameTime::Tick step_ticks
) {
    SimulationRuntimeTickRunUntilArrivalResult result{};

    if (step_ticks == 0) {
        result.run.validation.add_error("simulation.runtime.tick", "step_ticks must be greater than zero");
        return result;
    }

    clc::GameTime::Tick elapsed = 0;
    while (elapsed < max_ticks) {
        const auto remaining = max_ticks - elapsed;
        const auto step = std::min(step_ticks, remaining);
        auto report = advance_runtime_ticks(runtime, step);
        elapsed += step;
        merge_validation(result.run.validation, report.validation);

        result.arrival_elapsed_ticks += report.elapsed_ticks;
        if (!report.arrived_caravan_ids.empty()) {
            result.arrival_reached = true;
            result.arrived_caravan_id = report.arrived_caravan_ids.front();
            result.arrival_tick = report.tick_after;
        }

        result.run.reports.push_back(std::move(report));

        if (result.arrival_reached) {
            break;
        }
    }

    result.run.summary = summarize_runtime_tick_reports(result.run.reports);
    return result;
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
        summary.ticks_elapsed += report.ticks.elapsed_ticks;
        summary.warnings += report.validation.warning_count();
        summary.contract_failures += report.contracts.failed_count;

        for (const auto& caravan : report.caravans) {
            ++summary.caravan_ticks;

            if (caravan.advance.arrived && caravan.advance.ticks_elapsed > 0) {
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

SimulationRuntimeRunUntilArrivalResult run_runtime_until_first_caravan_arrival(
    SimulationRuntime& runtime,
    std::uint64_t max_days
) {
    SimulationRuntimeRunUntilArrivalResult result{};

    result.run.reports.reserve(static_cast<std::size_t>(max_days));

    for (std::uint64_t day = 0; day < max_days; ++day) {
        auto report = advance_runtime_day(runtime);
        merge_validation(result.run.validation, report.validation);

        result.arrival_elapsed_ticks += report.ticks.elapsed_ticks;
        if (!report.arrived_caravan_ids.empty()) {
            result.arrival_reached = true;
            result.arrived_caravan_id = report.arrived_caravan_ids.front();
            result.arrival_day = report.engine.day;
        }

        result.run.reports.push_back(std::move(report));

        if (result.arrival_reached) {
            break;
        }
    }

    result.run.summary = summarize_runtime_day_reports(result.run.reports);
    return result;
}

SimulationRuntimeArrivalContractResult run_runtime_until_first_caravan_arrival_and_fulfill_contract(
    SimulationRuntime& runtime,
    std::uint64_t max_days,
    std::string_view expected_faction_id
) {
    SimulationRuntimeArrivalContractResult result{};

    result.arrival = run_runtime_until_first_caravan_arrival(runtime, max_days);

    if (!result.arrival.arrival_reached) {
        result.fulfillment.validation.add_error(
            "simulation.contract.auto_fulfill",
            "no caravan arrival reached before fulfillment"
        );
        return result;
    }

    result.fulfillment = fulfill_first_runtime_contract_for_owned_arrived_caravan_with_reward_and_ledger(
        runtime,
        result.arrival.arrived_caravan_id,
        expected_faction_id
    );

    return result;
}

} // namespace clc::sim
