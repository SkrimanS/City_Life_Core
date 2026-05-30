#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/SimulationEngine.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

inline constexpr std::string_view runtime_action_type_add_resource = "add_resource";
inline constexpr std::string_view runtime_action_type_remove_resource = "remove_resource";
inline constexpr std::string_view runtime_action_type_transfer_resource = "transfer_resource";
inline constexpr std::string_view runtime_action_type_advance_days = "advance_days";

inline constexpr std::string_view runtime_action_status_accepted = "accepted";
inline constexpr std::string_view runtime_action_status_invalid = "invalid";
inline constexpr std::string_view runtime_action_status_rejected = "rejected";

inline constexpr std::string_view runtime_action_error_malformed_json = "malformed_json";
inline constexpr std::string_view runtime_action_error_invalid_action = "invalid_action";
inline constexpr std::string_view runtime_action_error_action_rejected = "action_rejected";
inline constexpr std::string_view runtime_action_error_unsupported_action_type = "unsupported_action_type";

struct RuntimeAction final {
    std::string action_id{};
    std::string type{};
    std::string actor_id{};
    std::string target_id{};
    std::string secondary_target_id{};
    std::string resource_id{};
    std::uint64_t amount{0};
    std::uint64_t days{0};
    std::string payload_json{};
};

struct RuntimeActionParseResult final {
    RuntimeAction action{};
    data::ValidationReport validation{};
};

struct RuntimeActionResult final {
    std::string action_id{};
    std::string type{};
    bool accepted{false};
    std::string validation_status{};
    std::string error_code{};
    std::string message{};
    data::ValidationReport validation{};
    SimulationCommandResult command{};
    std::vector<SimulationEvent> events{};
};

[[nodiscard]] RuntimeActionParseResult parse_runtime_action_json(std::string_view json);
[[nodiscard]] data::ValidationReport validate_runtime_action(const RuntimeAction& action);
[[nodiscard]] RuntimeActionResult dispatch_runtime_action(SimulationEngine& engine, const RuntimeAction& action);
[[nodiscard]] RuntimeActionResult dispatch_runtime_action_json(SimulationEngine& engine, std::string_view json);
[[nodiscard]] std::string runtime_action_result_to_json(const RuntimeActionResult& result);

} // namespace clc::sim
