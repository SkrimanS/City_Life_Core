#include "clc/sim/ActionBridge.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::sim::SimulationEngine make_engine() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "scenario bootstrap failed");
    return bootstrap.runtime.engine;
}

std::string run_sequence_and_digest(clc::sim::SimulationEngine& engine) {
    constexpr std::array<std::string_view, 5> actions{
        R"({"action_id":"det-1","type":"add_resource","actor_id":"test","payload":{"target_id":"riverwatch","resource_id":"grain","amount":7}})",
        R"({"action_id":"det-2","type":"remove_resource","actor_id":"test","payload":{"target_id":"riverwatch","resource_id":"grain","amount":2}})",
        R"({"action_id":"det-3","type":"transfer_resource","actor_id":"test","payload":{"target_id":"riverwatch","secondary_target_id":"hillford","resource_id":"grain","amount":3}})",
        R"({"action_id":"det-4","type":"advance_days","actor_id":"test","payload":{"days":1}})",
        R"({"action_id":"det-5","type":"remove_resource","actor_id":"test","payload":{"target_id":"riverwatch","resource_id":"grain","amount":999999}})"
    };

    std::string digest;
    for (const auto action : actions) {
        const auto result = clc::sim::dispatch_runtime_action_json(engine, action);
        digest += clc::sim::runtime_action_result_to_json(result);
        digest += '\n';
    }

    digest += "day=" + std::to_string(engine.current_day()) + '\n';
    digest += "riverwatch.grain=" + std::to_string(engine.settlement_resource_amount("riverwatch", "grain")) + '\n';
    digest += "hillford.grain=" + std::to_string(engine.settlement_resource_amount("hillford", "grain")) + '\n';
    digest += "events=" + std::to_string(engine.events().size()) + '\n';
    for (const auto& event : engine.events()) {
        digest += std::to_string(event.day);
        digest += '|';
        digest += event.type;
        digest += '|';
        digest += event.message;
        digest += '\n';
    }
    return digest;
}

} // namespace

int main() {
    auto first = make_engine();
    auto second = make_engine();

    const auto first_digest = run_sequence_and_digest(first);
    const auto second_digest = run_sequence_and_digest(second);

    require(first_digest == second_digest, "same action sequence should produce deterministic digest");
    require(first.current_day() == second.current_day(), "same action sequence should produce same day");
    require(first.settlement_resource_amount("riverwatch", "grain") == second.settlement_resource_amount("riverwatch", "grain"), "same action sequence should produce same source amount");
    require(first.settlement_resource_amount("hillford", "grain") == second.settlement_resource_amount("hillford", "grain"), "same action sequence should produce same target amount");
    require(first.events().size() == second.events().size(), "same action sequence should produce same event count");

    return 0;
}
