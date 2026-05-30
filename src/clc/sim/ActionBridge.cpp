#include "clc/sim/ActionBridge.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

std::string trim_copy(std::string_view value) {
    auto begin = value.begin();
    auto end = value.end();
    while (begin != end && std::isspace(static_cast<unsigned char>(*begin)) != 0) {
        ++begin;
    }
    while (begin != end && std::isspace(static_cast<unsigned char>(*(end - 1))) != 0) {
        --end;
    }
    return std::string{begin, end};
}

std::string unescape_json_string(std::string_view value) {
    std::string output;
    output.reserve(value.size());
    bool escaping = false;
    for (const auto ch : value) {
        if (escaping) {
            switch (ch) {
            case 'n': output.push_back('\n'); break;
            case 'r': output.push_back('\r'); break;
            case 't': output.push_back('\t'); break;
            case '"': output.push_back('"'); break;
            case '\\': output.push_back('\\'); break;
            default: output.push_back(ch); break;
            }
            escaping = false;
            continue;
        }
        if (ch == '\\') {
            escaping = true;
            continue;
        }
        output.push_back(ch);
    }
    return output;
}

std::size_t skip_ws(std::string_view json, std::size_t index) {
    while (index < json.size() && std::isspace(static_cast<unsigned char>(json[index])) != 0) {
        ++index;
    }
    return index;
}

std::size_t find_string_end(std::string_view json, std::size_t quote_index) {
    bool escaping = false;
    for (auto index = quote_index + 1; index < json.size(); ++index) {
        const auto ch = json[index];
        if (escaping) {
            escaping = false;
            continue;
        }
        if (ch == '\\') {
            escaping = true;
            continue;
        }
        if (ch == '"') {
            return index;
        }
    }
    return std::string_view::npos;
}

std::size_t find_field_value_start(std::string_view json, std::string_view field) {
    std::size_t depth = 0;

    for (std::size_t index = 0; index < json.size(); ++index) {
        const auto ch = json[index];
        if (ch == '"') {
            const auto key_end = find_string_end(json, index);
            if (key_end == std::string_view::npos) {
                return std::string_view::npos;
            }

            if (depth != 1) {
                index = key_end;
                continue;
            }

            const auto colon = skip_ws(json, key_end + 1);
            if (colon >= json.size() || json[colon] != ':') {
                index = key_end;
                continue;
            }

            const auto raw_key = json.substr(index + 1, key_end - index - 1);
            if (unescape_json_string(raw_key) == field) {
                return colon + 1;
            }

            index = key_end;
            continue;
        }

        if (ch == '{') {
            ++depth;
            continue;
        }
        if (ch == '}') {
            if (depth > 0) {
                --depth;
            }
            continue;
        }
    }

    return std::string_view::npos;
}

bool has_field(std::string_view json, std::string_view field) {
    return find_field_value_start(json, field) != std::string_view::npos;
}

bool find_string_field(std::string_view json, std::string_view field, std::string& out) {
    auto quote = skip_ws(json, find_field_value_start(json, field));
    if (quote >= json.size() || json[quote] != '"') {
        return false;
    }

    std::string raw;
    bool escaping = false;
    for (auto index = quote + 1; index < json.size(); ++index) {
        const auto ch = json[index];
        if (escaping) {
            raw.push_back('\\');
            raw.push_back(ch);
            escaping = false;
            continue;
        }
        if (ch == '\\') {
            escaping = true;
            continue;
        }
        if (ch == '"') {
            out = unescape_json_string(raw);
            return true;
        }
        raw.push_back(ch);
    }
    return false;
}

bool find_uint_field(std::string_view json, std::string_view field, std::uint64_t& out) {
    auto start = skip_ws(json, find_field_value_start(json, field));
    auto end = start;
    while (end < json.size() && std::isdigit(static_cast<unsigned char>(json[end])) != 0) {
        ++end;
    }
    if (end == start) {
        return false;
    }

    std::uint64_t value = 0;
    for (auto index = start; index < end; ++index) {
        value = value * 10 + static_cast<std::uint64_t>(json[index] - '0');
    }
    out = value;
    return true;
}

bool find_object_field(std::string_view json, std::string_view field, std::string& out) {
    auto start = skip_ws(json, find_field_value_start(json, field));
    if (start >= json.size() || json[start] != '{') {
        return false;
    }

    std::size_t depth = 0;
    bool in_string = false;
    bool escaping = false;
    for (auto index = start; index < json.size(); ++index) {
        const auto ch = json[index];
        if (in_string) {
            if (escaping) {
                escaping = false;
                continue;
            }
            if (ch == '\\') {
                escaping = true;
                continue;
            }
            if (ch == '"') {
                in_string = false;
            }
            continue;
        }

        if (ch == '"') {
            in_string = true;
            continue;
        }
        if (ch == '{') {
            ++depth;
        } else if (ch == '}') {
            --depth;
            if (depth == 0) {
                out = std::string{json.substr(start, index - start + 1)};
                return true;
            }
        }
    }
    return false;
}

std::string escape_json_string(std::string_view value) {
    std::string output;
    output.reserve(value.size());
    for (const auto ch : value) {
        switch (ch) {
        case '"': output += "\\\""; break;
        case '\\': output += "\\\\"; break;
        case '\n': output += "\\n"; break;
        case '\r': output += "\\r"; break;
        case '\t': output += "\\t"; break;
        default: output.push_back(ch); break;
        }
    }
    return output;
}

std::string copy_string(std::string_view value) {
    return std::string{value};
}

std::string validation_status_for(const data::ValidationReport& validation, bool accepted) {
    if (!validation.ok()) {
        return copy_string(runtime_action_status_invalid);
    }
    return copy_string(accepted ? runtime_action_status_accepted : runtime_action_status_rejected);
}

std::string validation_severity_to_string(data::ValidationSeverity severity) {
    switch (severity) {
    case data::ValidationSeverity::warning:
        return "warning";
    case data::ValidationSeverity::error:
        return "error";
    }
    return "error";
}

RuntimeActionResult rejected_result(const RuntimeAction& action, data::ValidationReport validation, std::string error_code, std::string message) {
    return RuntimeActionResult{
        .action_id = action.action_id,
        .type = action.type,
        .accepted = false,
        .validation_status = validation_status_for(validation, false),
        .error_code = std::move(error_code),
        .message = std::move(message),
        .validation = std::move(validation),
    };
}

RuntimeActionResult rejected_parse_result(data::ValidationReport validation, std::string error_code, std::string message) {
    return RuntimeActionResult{
        .accepted = false,
        .validation_status = validation_status_for(validation, false),
        .error_code = std::move(error_code),
        .message = std::move(message),
        .validation = std::move(validation),
    };
}

std::string first_validation_message(const data::ValidationReport& report) {
    if (report.messages().empty()) {
        return {};
    }
    return report.messages().front().message;
}

RuntimeActionResult command_result_to_action_result(
    const RuntimeAction& action,
    SimulationEngine& engine,
    SimulationCommandResult command,
    std::size_t event_start
) {
    std::vector<SimulationEvent> events;
    const auto& engine_events = engine.events();
    if (event_start < engine_events.size()) {
        events.assign(engine_events.begin() + static_cast<std::ptrdiff_t>(event_start), engine_events.end());
    }

    auto validation = command.validation;
    return RuntimeActionResult{
        .action_id = action.action_id,
        .type = action.type,
        .accepted = command.ok,
        .validation_status = copy_string(command.ok ? runtime_action_status_accepted : runtime_action_status_rejected),
        .error_code = command.ok ? std::string{} : copy_string(runtime_action_error_action_rejected),
        .message = command.ok ? std::string{"accepted"} : first_validation_message(validation),
        .validation = std::move(validation),
        .command = std::move(command),
        .events = std::move(events),
    };
}

} // namespace

RuntimeActionParseResult parse_runtime_action_json(std::string_view json) {
    RuntimeActionParseResult result;
    const auto trimmed = trim_copy(json);
    if (trimmed.empty()) {
        result.validation.add_error("action.json", "JSON action must not be empty");
        return result;
    }
    if (trimmed.front() != '{' || trimmed.back() != '}') {
        result.validation.add_error("action.json", "JSON action must be an object");
        return result;
    }

    find_string_field(trimmed, "action_id", result.action.action_id);
    find_string_field(trimmed, "type", result.action.type);
    find_string_field(trimmed, "actor_id", result.action.actor_id);
    find_object_field(trimmed, "payload", result.action.payload_json);

    const auto& payload = result.action.payload_json.empty() ? trimmed : result.action.payload_json;
    find_string_field(payload, "target_id", result.action.target_id);
    find_string_field(payload, "secondary_target_id", result.action.secondary_target_id);
    find_string_field(payload, "resource_id", result.action.resource_id);
    find_uint_field(payload, "amount", result.action.amount);
    find_uint_field(payload, "days", result.action.days);

    if (result.action.action_id.empty()) {
        result.validation.add_error("action.action_id", "action_id must not be empty");
    }
    if (result.action.type.empty()) {
        result.validation.add_error("action.type", "type must not be empty");
    }
    if (has_field(trimmed, "payload") && result.action.payload_json.empty()) {
        result.validation.add_error("action.payload", "payload must be a JSON object when present");
    }

    return result;
}

data::ValidationReport validate_runtime_action(const RuntimeAction& action) {
    data::ValidationReport report;
    if (action.action_id.empty()) {
        report.add_error("action.action_id", "action_id must not be empty");
    }
    if (action.type.empty()) {
        report.add_error("action.type", "type must not be empty");
        return report;
    }

    if (action.type == runtime_action_type_add_resource) {
        if (action.target_id.empty()) {
            report.add_error("action.payload.target_id", "target_id must not be empty for add_resource");
        }
        if (action.resource_id.empty()) {
            report.add_error("action.payload.resource_id", "resource_id must not be empty for add_resource");
        }
        if (action.amount == 0) {
            report.add_error("action.payload.amount", "amount must be greater than zero for add_resource");
        }
    } else if (action.type == runtime_action_type_remove_resource) {
        if (action.target_id.empty()) {
            report.add_error("action.payload.target_id", "target_id must not be empty for remove_resource");
        }
        if (action.resource_id.empty()) {
            report.add_error("action.payload.resource_id", "resource_id must not be empty for remove_resource");
        }
        if (action.amount == 0) {
            report.add_error("action.payload.amount", "amount must be greater than zero for remove_resource");
        }
    } else if (action.type == runtime_action_type_transfer_resource) {
        if (action.target_id.empty()) {
            report.add_error("action.payload.target_id", "target_id must not be empty for transfer_resource");
        }
        if (action.secondary_target_id.empty()) {
            report.add_error("action.payload.secondary_target_id", "secondary_target_id must not be empty for transfer_resource");
        }
        if (action.resource_id.empty()) {
            report.add_error("action.payload.resource_id", "resource_id must not be empty for transfer_resource");
        }
        if (action.amount == 0) {
            report.add_error("action.payload.amount", "amount must be greater than zero for transfer_resource");
        }
    } else if (action.type == runtime_action_type_advance_days) {
        if (action.days == 0) {
            report.add_error("action.payload.days", "days must be greater than zero for advance_days");
        }
    } else {
        report.add_error("action.type", "unsupported action type");
    }

    return report;
}

RuntimeActionResult dispatch_runtime_action(SimulationEngine& engine, const RuntimeAction& action) {
    auto validation = validate_runtime_action(action);
    if (!validation.ok()) {
        const auto message = first_validation_message(validation);
        return rejected_result(action, std::move(validation), copy_string(runtime_action_error_invalid_action), message);
    }

    const auto event_start = engine.events().size();

    if (action.type == runtime_action_type_add_resource) {
        return command_result_to_action_result(
            action,
            engine,
            engine.add_resource_to_settlement_command(action.target_id, action.resource_id, action.amount),
            event_start
        );
    }
    if (action.type == runtime_action_type_remove_resource) {
        return command_result_to_action_result(
            action,
            engine,
            engine.remove_resource_from_settlement_command(action.target_id, action.resource_id, action.amount),
            event_start
        );
    }
    if (action.type == runtime_action_type_transfer_resource) {
        return command_result_to_action_result(
            action,
            engine,
            engine.transfer_resource_between_settlements_command(action.target_id, action.secondary_target_id, action.resource_id, action.amount),
            event_start
        );
    }
    if (action.type == runtime_action_type_advance_days) {
        const auto reports = engine.run_days(action.days);
        std::vector<SimulationEvent> events;
        for (const auto& report : reports) {
            events.insert(events.end(), report.events.begin(), report.events.end());
        }
        return RuntimeActionResult{
            .action_id = action.action_id,
            .type = action.type,
            .accepted = true,
            .validation_status = copy_string(runtime_action_status_accepted),
            .message = "accepted",
            .validation = std::move(validation),
            .events = std::move(events),
        };
    }

    return rejected_result(action, std::move(validation), copy_string(runtime_action_error_unsupported_action_type), "unsupported action type");
}

RuntimeActionResult dispatch_runtime_action_json(SimulationEngine& engine, std::string_view json) {
    auto parsed = parse_runtime_action_json(json);
    if (!parsed.validation.ok()) {
        const auto message = first_validation_message(parsed.validation);
        return rejected_parse_result(std::move(parsed.validation), copy_string(runtime_action_error_malformed_json), message);
    }
    return dispatch_runtime_action(engine, parsed.action);
}

std::string runtime_action_result_to_json(const RuntimeActionResult& result) {
    std::ostringstream output;
    output << '{';
    output << "\"action_id\":\"" << escape_json_string(result.action_id) << "\",";
    output << "\"type\":\"" << escape_json_string(result.type) << "\",";
    output << "\"accepted\":" << (result.accepted ? "true" : "false") << ',';
    output << "\"validation_status\":\"" << escape_json_string(result.validation_status) << "\",";
    output << "\"error_code\":\"" << escape_json_string(result.error_code) << "\",";
    output << "\"message\":\"" << escape_json_string(result.message) << "\",";
    output << "\"events\":" << result.events.size() << ',';
    output << "\"diagnostics\":" << result.validation.messages().size() << ',';

    output << "\"command_detail\":";
    if (result.command.command.empty()) {
        output << "null";
    } else {
        output << '{';
        output << "\"command\":\"" << escape_json_string(result.command.command) << "\",";
        output << "\"ok\":" << (result.command.ok ? "true" : "false") << ',';
        output << "\"subject_id\":\"" << escape_json_string(result.command.subject_id) << "\",";
        output << "\"target_id\":\"" << escape_json_string(result.command.target_id) << "\",";
        output << "\"resource_id\":\"" << escape_json_string(result.command.resource_id) << "\",";
        output << "\"amount\":" << result.command.amount;
        output << '}';
    }
    output << ',';

    output << "\"events_detail\":[";
    for (std::size_t index = 0; index < result.events.size(); ++index) {
        const auto& event = result.events[index];
        if (index != 0) {
            output << ',';
        }
        output << '{';
        output << "\"day\":" << event.day << ',';
        output << "\"type\":\"" << escape_json_string(event.type) << "\",";
        output << "\"message\":\"" << escape_json_string(event.message) << "\"";
        output << '}';
    }
    output << "],";

    const auto& diagnostics = result.validation.messages();
    output << "\"diagnostics_detail\":[";
    for (std::size_t index = 0; index < diagnostics.size(); ++index) {
        const auto& diagnostic = diagnostics[index];
        if (index != 0) {
            output << ',';
        }
        output << '{';
        output << "\"severity\":\"" << validation_severity_to_string(diagnostic.severity) << "\",";
        output << "\"path\":\"" << escape_json_string(diagnostic.path) << "\",";
        output << "\"message\":\"" << escape_json_string(diagnostic.message) << "\"";
        output << '}';
    }
    output << ']';
    output << '}';
    return output.str();
}

} // namespace clc::sim
