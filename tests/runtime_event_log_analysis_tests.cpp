#include "clc/core/EventLog.hpp"
#include "clc/core/Time.hpp"
#include "clc/sim/SimulationRuntimeEvents.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    clc::EventLog log{};
    log.append(clc::ticks_per_day(), "runtime.day.completed", "day=1");
    log.append(clc::ticks_per_day(), "runtime.caravan.progress", "caravan_a");
    log.append(clc::days_to_ticks(2), "runtime.day.completed", "day=2");
    log.append(clc::days_to_ticks(2), "runtime.caravan.arrived", "caravan_a");
    log.append(clc::days_to_ticks(2), "runtime.caravan.cargo_delivered", "caravan_a->hillford:total=10");
    log.append(clc::days_to_ticks(2), "runtime.contract.fulfilled", "contract_a");
    log.append(clc::days_to_ticks(3), "runtime.contract.failed", "contract_b");

    const auto analysis = clc::sim::analyze_runtime_event_log(log);
    require(analysis.total_events == 7, "analysis should count total events");
    require(analysis.day_events == 2, "analysis should count day events");
    require(analysis.caravan_progress_events == 1, "analysis should count progress events");
    require(analysis.caravan_arrival_events == 1, "analysis should count arrival events");
    require(analysis.caravan_cargo_delivered_events == 1, "analysis should count cargo events");
    require(analysis.contract_fulfilled_events == 1, "analysis should count fulfilled contracts");
    require(analysis.contract_failed_events == 1, "analysis should count failed contracts");
    require(analysis.unknown_events == 0, "analysis should not count unknown events");
    require(analysis.first_tick == clc::ticks_per_day(), "analysis should expose first runtime tick");
    require(analysis.last_tick == clc::days_to_ticks(3), "analysis should expose last runtime tick");

    const auto checksum = clc::sim::calculate_runtime_event_log_checksum(log);
    require(checksum.event_count == 7, "checksum should count events");
    require(checksum.first_tick == clc::ticks_per_day(), "checksum should expose first runtime tick");
    require(checksum.last_tick == clc::days_to_ticks(3), "checksum should expose last runtime tick");
    require(checksum.value != 0, "checksum should be non-zero");

    const auto checksum_copy = clc::sim::calculate_runtime_event_log_checksum(log);
    require(clc::sim::compare_runtime_event_log_checksums(checksum, checksum_copy).matches(), "matching checksums should match");
    require(clc::sim::validate_runtime_event_log_checksum_match(checksum, checksum_copy).ok(), "matching checksums should validate");
    require(clc::sim::validate_runtime_event_logs_match(log, log).ok(), "matching logs should validate");
    require(clc::sim::validate_runtime_event_log(log).ok(), "combined event log validation should pass");

    clc::EventLog modified = log;
    modified.append(clc::days_to_ticks(4), "runtime.day.completed", "day=4");
    const auto modified_checksum = clc::sim::calculate_runtime_event_log_checksum(modified);
    require(!clc::sim::compare_runtime_event_log_checksums(checksum, modified_checksum).matches(), "different checksums should not match");
    require(!clc::sim::validate_runtime_event_log_checksum_match(checksum, modified_checksum).ok(), "different checksums should not validate");
    require(!clc::sim::validate_runtime_event_logs_match(log, modified).ok(), "different logs should not validate");

    clc::EventLog broken_order{};
    broken_order.append(clc::days_to_ticks(5), "runtime.day.completed", "day=5");
    broken_order.append(clc::days_to_ticks(3), "runtime.caravan.progress", "caravan_a");
    require(!clc::sim::validate_runtime_event_log_tick_order(broken_order).ok(), "tick order validator should reject regressions");
    require(!clc::sim::validate_runtime_event_log(broken_order).ok(), "combined validator should reject tick order regressions");

    clc::EventLog unknown{};
    unknown.append(clc::ticks_per_day(), "runtime.unknown", "mystery");
    require(!clc::sim::validate_runtime_event_log_known_types(unknown).ok(), "known type validator should reject unknown events");

    clc::EventLog bad_day_payload{};
    bad_day_payload.append(clc::ticks_per_day(), "runtime.day.completed", "1");
    require(!clc::sim::validate_runtime_event_log_payloads(bad_day_payload).ok(), "payload validator should reject bad day payload");
    require(!clc::sim::validate_runtime_event_log(bad_day_payload).ok(), "combined validator should reject bad day payload");

    clc::EventLog empty_caravan_payload{};
    empty_caravan_payload.append(clc::ticks_per_day(), "runtime.caravan.progress", "");
    require(!clc::sim::validate_runtime_event_log_payloads(empty_caravan_payload).ok(), "payload validator should reject empty caravan payload");

    clc::EventLog bad_cargo_payload{};
    bad_cargo_payload.append(clc::ticks_per_day(), "runtime.caravan.cargo_delivered", "caravan_a->hillford:total=0");
    require(!clc::sim::validate_runtime_event_log_payloads(bad_cargo_payload).ok(), "payload validator should reject zero cargo payload");

    clc::EventLog malformed_cargo_payload{};
    malformed_cargo_payload.append(clc::ticks_per_day(), "runtime.caravan.cargo_delivered", "caravan_a:hillford:total=10");
    require(!clc::sim::validate_runtime_event_log_payloads(malformed_cargo_payload).ok(), "payload validator should reject malformed cargo payload");

    clc::EventLog bad_contract_payload{};
    bad_contract_payload.append(clc::ticks_per_day(), "runtime.contract.failed", "");
    require(!clc::sim::validate_runtime_event_log_payloads(bad_contract_payload).ok(), "payload validator should reject empty contract payload");

    return 0;
}
