#include "clc/sim/ActionBridge.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

clc::sim::SimulationEngine make_engine() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "scenario bootstrap failed");
    return bootstrap.runtime.engine;
}

} // namespace

int main() {
    auto engine = make_engine();

    const auto before_grain = engine.settlement_resource_amount("riverwatch", "grain");
    const auto valid = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a1","type":"add_resource","actor_id":"tester","payload":{"target_id":"riverwatch","resource_id":"grain","amount":5}})"
    );
    require(valid.accepted, "valid add_resource action was rejected");
    require(valid.validation_status == "accepted", "accepted action had wrong validation_status");
    require(valid.error_code.empty(), "accepted action unexpectedly had error_code");
    require(valid.validation.ok(), "accepted action unexpectedly had validation diagnostics");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_grain + 5, "valid action did not mutate runtime");
    require(!valid.events.empty(), "valid action did not report produced events");

    const auto before_invalid = engine.settlement_resource_amount("riverwatch", "grain");
    const auto invalid_type = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a2","type":"unknown_action","payload":{"target_id":"riverwatch","resource_id":"grain","amount":5}})"
    );
    require(!invalid_type.accepted, "invalid action type was accepted");
    require(invalid_type.validation_status == "invalid", "invalid action type had wrong validation_status");
    require(invalid_type.error_code == "invalid_action", "invalid action type returned wrong error_code");
    require(!invalid_type.validation.ok(), "invalid action type did not return diagnostics");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_invalid, "invalid action mutated runtime");

    const auto malformed = clc::sim::dispatch_runtime_action_json(engine, "not json");
    require(!malformed.accepted, "malformed JSON was accepted");
    require(malformed.validation_status == "invalid", "malformed JSON had wrong validation_status");
    require(malformed.error_code == "malformed_json", "malformed JSON returned wrong error_code");
    require(!malformed.validation.ok(), "malformed JSON did not return diagnostics");

    const auto malformed_payload = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a3","type":"add_resource","payload":"not an object"})"
    );
    require(!malformed_payload.accepted, "malformed payload was accepted");
    require(malformed_payload.error_code == "malformed_json", "malformed payload returned wrong error_code");

    const auto missing_fields = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a4","type":"add_resource","payload":{"target_id":"riverwatch"}})"
    );
    require(!missing_fields.accepted, "missing fields action was accepted");
    require(missing_fields.validation_status == "invalid", "missing fields action had wrong validation_status");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_invalid, "missing fields action mutated runtime");

    const auto remove = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a5","type":"remove_resource","payload":{"target_id":"riverwatch","resource_id":"grain","amount":1}})"
    );
    require(remove.accepted, "valid remove_resource action was rejected");

    auto transfer_engine = make_engine();
    const auto riverwatch_before = transfer_engine.settlement_resource_amount("riverwatch", "grain");
    const auto hillford_before = transfer_engine.settlement_resource_amount("hillford", "grain");
    const auto transfer = clc::sim::dispatch_runtime_action_json(
        transfer_engine,
        R"({"action_id":"a6","type":"transfer_resource","payload":{"target_id":"riverwatch","secondary_target_id":"hillford","resource_id":"grain","amount":2}})"
    );
    require(transfer.accepted, "valid transfer_resource action was rejected");
    require(transfer_engine.settlement_resource_amount("riverwatch", "grain") == riverwatch_before - 2, "transfer did not debit source");
    require(transfer_engine.settlement_resource_amount("hillford", "grain") == hillford_before + 2, "transfer did not credit target");

    const auto current_day = transfer_engine.current_day();
    const auto advance = clc::sim::dispatch_runtime_action_json(
        transfer_engine,
        R"({"action_id":"a7","type":"advance_days","payload":{"days":2}})"
    );
    require(advance.accepted, "valid advance_days action was rejected");
    require(advance.validation_status == "accepted", "advance_days had wrong validation_status");
    require(transfer_engine.current_day() == current_day + 2, "advance_days did not advance runtime");

    const auto json = clc::sim::runtime_action_result_to_json(advance);
    require(json.find("\"accepted\":true") != std::string::npos, "result JSON missing accepted status");
    require(json.find("\"validation_status\":\"accepted\"") != std::string::npos, "result JSON missing validation status");
    require(json.find("\"action_id\":\"a7\"") != std::string::npos, "result JSON missing action id");
    require(json.find("\"diagnostics\":0") != std::string::npos, "result JSON missing diagnostics count");

    return 0;
}
