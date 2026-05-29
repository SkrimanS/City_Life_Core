#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/SimulationEngine.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct RuntimeAction final {
    std::string action_id{};
    std::string type{};
    std::string actor_id{};
    std::string target_id{};
    std::string secondary_target_id{};
    std::string resource_id{};
    std::uint64_t amount{0};
    std::uint64_t days{0};
};

struct RuntimeActionParseResult final {
    RuntimeAction action{};
    data::ValidationReport validation{};
};

struct RuntimeActionResult final {
    std::string action_id{};
    std::string type{};
    bool accepted{false};
    std::string error_code{};
    std::string message{};
    SimulationCommandResult command{};
    std::vector<SimulationEvent> events{};
};

[[nodiscard]] RuntimeActionParseResult parse_runtime_action_json(std::string_view json);
[[nodiscard]] data::ValidationReport validate_runtime_action(const RuntimeAction& action);
[[nodiscard]] RuntimeActionResult dispatch_runtime_action(SimulationEngine& engine, const RuntimeAction& action);
[[nodiscard]] RuntimeActionResult dispatch_runtime_action_json(SimulationEngine& engine, std::string_view json);
[[nodiscard]] std::string runtime_action_result_to_json(const RuntimeActionResult& result);

} // namespace clc::sim
