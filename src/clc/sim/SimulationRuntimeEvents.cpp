#include "clc/sim/SimulationRuntimeEvents.hpp"

#include "clc/sim/Caravans.hpp"
#include "clc/sim/Contracts.hpp"
#include "clc/sim/SimulationRuntime.hpp"

#include <algorithm>
#include <charconv>
#include <limits>
#include <string>
#include <string_view>

namespace clc::sim {
namespace {

constexpr std::string_view runtime_day_completed = "runtime.day.completed";
constexpr std::string_view runtime_tick_completed = "runtime.tick.completed";
constexpr std::string_view runtime_caravan_progress = "runtime.caravan.progress";
constexpr std::string_view runtime_caravan_arrived = "runtime.caravan.arrived";
constexpr std::string_view runtime_caravan_cargo_delivered = "runtime.caravan.cargo_delivered";
constexpr std::string_view runtime_contract_fulfilled = "runtime.contract.fulfilled";
constexpr std::string_view runtime_contract_failed = "runtime.contract.failed";

std::string to_decimal(std::uint64_t value) {
    return std::to_string(value);
}

bool decimal_digits_only(std::string_view value) noexcept {
    return !value.empty() && std::all_of(value.begin(), value.end(), [](char character) {
        return character >= '0' && character <= '9';
    });
}

bool parse_u64(std::string_view value, std::uint64_t& out) noexcept {
    if (!decimal_digits_only(value)) {
        return false;
    }

    const auto* begin = value.data();
    const auto* end = value.data() + value.size();
    const auto [ptr, error] = std::from_chars(begin, end, out);
    return error == std::errc{} && ptr == end;
}

bool valid_tick_completed_payload(std::string_view payload) noexcept {
    constexpr std::string_view prefix = "elapsed=";
    if (payload.rfind(prefix, 0) != 0) {
        return false;
    }

    std::uint64_t elapsed{};
    return parse_u64(payload.substr(prefix.size()), elapsed) && elapsed > 0;
}

bool valid_cargo_delivery_payload(std::string_view payload) noexcept {
    const auto colon = payload.find(':');
    if (colon == std::string_view::npos || colon == 0 || colon + 1 >= payload.size()) {
        return false;
    }

    const auto arrow = payload.substr(0, colon).find("->");
    if (arrow == std::string_view::npos || arrow == 0 || arrow + 2 >= colon) {
        return false;
    }

    constexpr std::string_view total_prefix = "total=";
    const auto total = payload.substr(colon + 1);
    if (total.rfind(total_prefix, 0) != 0) {
        return false;
    }

    std::uint64_t amount{};
    return parse_u64(total.substr(total_prefix.size()), amount) && amount > 0;
}

bool is_known_runtime_event_type(std::string_view type) noexcept {
    return type == runtime_day_completed
        || type == runtime_tick_completed
        || type == runtime_caravan_progress
        || type == runtime_caravan_arrived
        || type == runtime_caravan_cargo_delivered
        || type == runtime_contract_fulfilled
        || type == runtime_contract_failed;
}

void append_event(EventLog& log, GameTime time, std::string type, std::string payload) {
    log.append(time.current_tick(), std::move(type), std::move(payload));
}

std::string caravan_payload(const CaravanState& caravan) {
    return caravan.id;
}

std::string contract_payload(const ResourceDeliveryContract& contract) {
    return contract.id;
}

std::string cargo_payload(const RuntimeCaravanCargoDeliveryResult& result) {
    return result.caravan_id + "->" + result.destination_settlement_id + ":total=" + to_decimal(result.total_amount);
}

} // namespace

void append_runtime_day_completed_event(EventLog& log, std::uint64_t day) {
    log.append(day, std::string{runtime_day_completed}, "day=" + to_decimal(day));
}

void append_runtime_day_completed_event(EventLog& log, GameTime time, std::uint64_t day) {
    append_event(log, time, std::string{runtime_day_completed}, "day=" + to_decimal(day));
}

void append_runtime_tick_completed_event(EventLog& log, GameTime time, std::uint64_t elapsed_ticks) {
    append_event(log, time, std::string{runtime_tick_completed}, "elapsed=" + to_decimal(elapsed_ticks));
}

void append_runtime_caravan_progress_event(EventLog& log, GameTime time, const CaravanState& caravan) {
    append_event(log, time, std::string{runtime_caravan_progress}, caravan_payload(caravan));
}

void append_runtime_caravan_arrived_event(EventLog& log, GameTime time, const CaravanState& caravan) {
    append_event(log, time, std::string{runtime_caravan_arrived}, caravan_payload(caravan));
}

void append_runtime_caravan_cargo_delivered_event(EventLog& log, GameTime time, const RuntimeCaravanCargoDeliveryResult& result) {
    append_event(log, time, std::string{runtime_caravan_cargo_delivered}, cargo_payload(result));
}

void append_runtime_contract_fulfilled_event(EventLog& log, GameTime time, const ResourceDeliveryContract& contract) {
    append_event(log, time, std::string{runtime_contract_fulfilled}, contract_payload(contract));
}

void append_runtime_contract_failed_event(EventLog& log, GameTime time, const ResourceDeliveryContract& contract) {
    append_event(log, time, std::string{runtime_contract_failed}, contract_payload(contract));
}

void append_runtime_caravan_arrival_events(EventLog& log, GameTime time, const RuntimeAdvanceReport& report, const CaravanFleet& fleet) {
    for (const auto& caravan_report : report.caravans) {
        if (!caravan_report.arrived || caravan_report.ticks_elapsed == 0) {
            continue;
        }

        const auto* caravan = caravan_by_id(fleet, caravan_report.caravan_id);
        if (caravan == nullptr) {
            continue;
        }

        append_runtime_caravan_arrived_event(log, time, *caravan);
    }
}

void append_runtime_caravan_progress_events(EventLog& log, GameTime time, const RuntimeAdvanceReport& report, const CaravanFleet& fleet) {
    for (const auto& caravan_report : report.caravans) {
        if (caravan_report.ticks_elapsed == 0) {
            continue;
        }

        const auto* caravan = caravan_by_id(fleet, caravan_report.caravan_id);
        if (caravan == nullptr) {
            continue;
        }

        append_runtime_caravan_progress_event(log, time, *caravan);
    }
}

void append_runtime_contract_events(EventLog& log, GameTime time, const ContractCatalog& contracts) {
    for (const auto& contract : contracts.contracts) {
        if (contract.status == ContractStatus::fulfilled) {
            append_runtime_contract_fulfilled_event(log, time, contract);
        } else if (contract.status == ContractStatus::failed) {
            append_runtime_contract_failed_event(log, time, contract);
        }
    }
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
        if (event.type == runtime_day_completed) {
            ++analysis.day_events;
        } else if (event.type == runtime_tick_completed) {
            ++analysis.tick_events;
        } else if (event.type == runtime_caravan_progress) {
            ++analysis.caravan_progress_events;
        } else if (event.type == runtime_caravan_arrived) {
            ++analysis.caravan_arrival_events;
        } else if (event.type == runtime_caravan_cargo_delivered) {
            ++analysis.caravan_cargo_delivered_events;
        } else if (event.type == runtime_contract_fulfilled) {
            ++analysis.contract_fulfilled_events;
        } else if (event.type == runtime_contract_failed) {
            ++analysis.contract_failed_events;
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
            report.add_error("runtime.event_log.tick_order", "runtime event log tick order regression detected");
            return report;
        }
    }

    return report;
}

data::ValidationReport validate_runtime_event_log_known_types(const clc::EventLog& log) {
    data::ValidationReport report{};

    for (const auto& event : log.events()) {
        if (!is_known_runtime_event_type(event.type)) {
            report.add_error("runtime.event_log.type", "runtime event log contains unknown event type");
            return report;
        }
    }

    return report;
}

data::ValidationReport validate_runtime_event_log_payloads(const clc::EventLog& log) {
    data::ValidationReport report{};

    for (const auto& event : log.events()) {
        if (event.type == runtime_day_completed) {
            constexpr std::string_view prefix = "day=";
            if (event.payload.rfind(prefix, 0) != 0 || !decimal_digits_only(std::string_view{event.payload}.substr(prefix.size()))) {
                report.add_error("runtime.event_log.payload", "day event payload must be day=N");
                return report;
            }
        } else if (event.type == runtime_tick_completed) {
            if (!valid_tick_completed_payload(event.payload)) {
                report.add_error("runtime.event_log.payload", "tick event payload must be elapsed=N with N greater than zero");
                return report;
            }
        } else if (event.type == runtime_caravan_progress
            || event.type == runtime_caravan_arrived
            || event.type == runtime_contract_fulfilled
            || event.type == runtime_contract_failed) {
            if (event.payload.empty()) {
                report.add_error("runtime.event_log.payload", "runtime event payload must not be empty");
                return report;
            }
        } else if (event.type == runtime_caravan_cargo_delivered) {
            if (!valid_cargo_delivery_payload(event.payload)) {
                report.add_error("runtime.event_log.payload", "cargo delivery payload must be caravan->settlement:total=N with N greater than zero");
                return report;
            }
        }
    }

    return report;
}

data::ValidationReport validate_runtime_event_log(const clc::EventLog& log) {
    auto report = validate_runtime_event_log_tick_order(log);

    if (!report.ok()) {
        return report;
    }

    report = validate_runtime_event_log_known_types(log);
    if (!report.ok()) {
        return report;
    }

    return validate_runtime_event_log_payloads(log);
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
        report.add_error("runtime.event_log.checksum", "runtime event log checksum mismatch detected");
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
