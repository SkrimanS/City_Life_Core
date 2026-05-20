#include "clc/core/EventLog.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    clc::EventLog log{};

    log.append(1, "runtime.day.completed", "day=1");
    log.append(1, "runtime.caravan.progress", "caravan_a");
    log.append(2, "runtime.day.completed", "day=2");
    log.append(2, "runtime.caravan.arrived", "caravan_a");
    log.append(2, "runtime.contract.fulfilled", "contract_a");

    const auto analysis = clc::sim::analyze_runtime_event_log(log);

    if (analysis.total_events != 5) {
        std::cerr << "unexpected total_events\n";
        return 1;
    }

    if (analysis.day_events != 2) {
        std::cerr << "unexpected day_events\n";
        return 1;
    }

    if (analysis.caravan_progress_events != 1) {
        std::cerr << "unexpected progress events\n";
        return 1;
    }

    if (analysis.caravan_arrival_events != 1) {
        std::cerr << "unexpected arrival events\n";
        return 1;
    }

    if (analysis.contract_fulfilled_events != 1) {
        std::cerr << "unexpected contract events\n";
        return 1;
    }

    if (analysis.unknown_events != 0) {
        std::cerr << "unexpected unknown events\n";
        return 1;
    }

    if (analysis.first_tick != 1 || analysis.last_tick != 2) {
        std::cerr << "unexpected tick range\n";
        return 1;
    }

    const auto checksum = clc::sim::calculate_runtime_event_log_checksum(log);

    if (checksum.event_count != 5) {
        std::cerr << "unexpected checksum event count\n";
        return 1;
    }

    if (checksum.first_tick != 1 || checksum.last_tick != 2) {
        std::cerr << "unexpected checksum tick range\n";
        return 1;
    }

    if (checksum.value == 0) {
        std::cerr << "unexpected zero checksum\n";
        return 1;
    }

    const auto checksum_copy = clc::sim::calculate_runtime_event_log_checksum(log);
    const auto comparison = clc::sim::compare_runtime_event_log_checksums(checksum, checksum_copy);

    if (!comparison.matches()) {
        std::cerr << "matching checksums unexpectedly differ\n";
        return 1;
    }

    const auto valid_checksum = clc::sim::validate_runtime_event_log_checksum_match(checksum, checksum_copy);

    if (!valid_checksum.ok()) {
        std::cerr << "matching checksums unexpectedly invalid\n";
        return 1;
    }

    const auto valid_logs = clc::sim::validate_runtime_event_logs_match(log, log);

    if (!valid_logs.ok()) {
        std::cerr << "matching logs unexpectedly invalid\n";
        return 1;
    }

    clc::EventLog modified = log;
    modified.append(3, "runtime.day.completed", "day=3");

    const auto modified_checksum = clc::sim::calculate_runtime_event_log_checksum(modified);
    const auto mismatch = clc::sim::compare_runtime_event_log_checksums(checksum, modified_checksum);

    if (mismatch.matches()) {
        std::cerr << "different checksums unexpectedly match\n";
        return 1;
    }

    const auto invalid_checksum = clc::sim::validate_runtime_event_log_checksum_match(checksum, modified_checksum);

    if (invalid_checksum.ok()) {
        std::cerr << "different checksums unexpectedly valid\n";
        return 1;
    }

    const auto invalid_logs = clc::sim::validate_runtime_event_logs_match(log, modified);

    if (invalid_logs.ok()) {
        std::cerr << "different logs unexpectedly valid\n";
        return 1;
    }

    const auto ordered = clc::sim::validate_runtime_event_log_tick_order(log);

    if (!ordered.ok()) {
        std::cerr << "ordered log unexpectedly invalid\n";
        return 1;
    }

    const auto known = clc::sim::validate_runtime_event_log_known_types(log);

    if (!known.ok()) {
        std::cerr << "known log unexpectedly invalid\n";
        return 1;
    }

    const auto combined = clc::sim::validate_runtime_event_log(log);

    if (!combined.ok()) {
        std::cerr << "combined validator unexpectedly invalid\n";
        return 1;
    }

    clc::EventLog broken{};
    broken.append(5, "runtime.day.completed", "day=5");
    broken.append(3, "runtime.caravan.progress", "caravan_a");

    const auto invalid = clc::sim::validate_runtime_event_log_tick_order(broken);

    if (invalid.ok()) {
        std::cerr << "broken log unexpectedly valid\n";
        return 1;
    }

    const auto broken_combined = clc::sim::validate_runtime_event_log(broken);

    if (broken_combined.ok()) {
        std::cerr << "combined validator unexpectedly accepted broken order\n";
        return 1;
    }

    broken.append(6, "runtime.unknown", "mystery");

    const auto unknown = clc::sim::validate_runtime_event_log_known_types(broken);

    if (unknown.ok()) {
        std::cerr << "unknown event unexpectedly accepted\n";
        return 1;
    }

    return 0;
}
