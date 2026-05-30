#include "clc/sim/ActionBridge.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
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
    require(valid.validation_status == clc::sim::runtime_action_status_accepted, "accepted action had wrong validation_status");
    require(valid.error_code.empty(), "accepted action unexpectedly had error_code");
    require(valid.validation.ok(), "accepted action unexpectedly had validation diagnostics");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_grain + 5, "valid action did not mutate runtime");
    require(!valid.events.empty(), "valid action did not report produced events");

    clc::sim::RuntimeAction direct_action{
        .action_id = "direct-a1",
        .type = std::string{clc::sim::runtime_action_type_add_resource},
        .actor_id = "tester",
        .target_id = "riverwatch",
        .resource_id = "grain",
        .amount = 1,
    };
    const auto before_direct = engine.settlement_resource_amount("riverwatch", "grain");
    const auto direct = clc::sim::dispatch_runtime_action(engine, direct_action);
    require(direct.accepted, "direct RuntimeAction with public type constant was rejected");
    require(direct.validation_status == clc::sim::runtime_action_status_accepted, "direct RuntimeAction had wrong validation_status");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_direct + 1, "direct RuntimeAction did not mutate runtime");

    const auto valid_json = clc::sim::runtime_action_result_to_json(valid);
    require(valid_json.find("\"command_detail\":{") != std::string::npos, "valid result JSON missing command detail object");
    require(valid_json.find("\"command\":\"add_resource_to_settlement\"") != std::string::npos, "valid result JSON missing command name");
    require(valid_json.find("\"ok\":true") != std::string::npos, "valid result JSON missing command ok state");
    require(valid_json.find("\"subject_id\":\"riverwatch\"") != std::string::npos, "valid result JSON missing command subject id");
    require(valid_json.find("\"resource_id\":\"grain\"") != std::string::npos, "valid result JSON missing command resource id");
    require(valid_json.find("\"amount\":5") != std::string::npos, "valid result JSON missing command amount");

    const auto before_invalid = engine.settlement_resource_amount("riverwatch", "grain");
    const auto invalid_type = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a2","type":"unknown_action","payload":{"target_id":"riverwatch","resource_id":"grain","amount":5}})"
    );
    require(!invalid_type.accepted, "invalid action type was accepted");
    require(invalid_type.validation_status == clc::sim::runtime_action_status_invalid, "invalid action type had wrong validation_status");
    require(invalid_type.error_code == clc::sim::runtime_action_error_invalid_action, "invalid action type returned wrong error_code");
    require(!invalid_type.validation.ok(), "invalid action type did not return diagnostics");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_invalid, "invalid action mutated runtime");

    const auto malformed = clc::sim::dispatch_runtime_action_json(engine, "not json");
    require(!malformed.accepted, "malformed JSON was accepted");
    require(malformed.validation_status == clc::sim::runtime_action_status_invalid, "malformed JSON had wrong validation_status");
    require(malformed.error_code == clc::sim::runtime_action_error_malformed_json, "malformed JSON returned wrong error_code");
    require(!malformed.validation.ok(), "malformed JSON did not return diagnostics");

    const auto malformed_payload = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a3","type":"add_resource","payload":"not an object"})"
    );
    require(!malformed_payload.accepted, "malformed payload was accepted");
    require(malformed_payload.error_code == clc::sim::runtime_action_error_malformed_json, "malformed payload returned wrong error_code");

    const auto missing_fields = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a4","type":"add_resource","payload":{"target_id":"riverwatch"}})"
    );
    require(!missing_fields.accepted, "missing fields action was accepted");
    require(missing_fields.validation_status == clc::sim::runtime_action_status_invalid, "missing fields action had wrong validation_status");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_invalid, "missing fields action mutated runtime");

    const auto float_amount = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a4-float","type":"add_resource","payload":{"target_id":"riverwatch","resource_id":"grain","amount":5.5}})"
    );
    require(!float_amount.accepted, "float amount action was accepted");
    require(float_amount.error_code == clc::sim::runtime_action_error_invalid_action, "float amount returned wrong error_code");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_invalid, "float amount action mutated runtime");

    const auto negative_amount = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a4-negative","type":"add_resource","payload":{"target_id":"riverwatch","resource_id":"grain","amount":-5}})"
    );
    require(!negative_amount.accepted, "negative amount action was accepted");
    require(negative_amount.error_code == clc::sim::runtime_action_error_invalid_action, "negative amount returned wrong error_code");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_invalid, "negative amount action mutated runtime");

    const auto remove = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a5","type":"remove_resource","payload":{"target_id":"riverwatch","resource_id":"grain","amount":1}})"
    );
    require(remove.accepted, "valid remove_resource action was rejected");

    const auto before_rejected = engine.settlement_resource_amount("riverwatch", "grain");
    const auto rejected = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"a6","type":"remove_resource","payload":{"target_id":"riverwatch","resource_id":"grain","amount":999999}})"
    );
    require(!rejected.accepted, "runtime-rejected action was accepted");
    require(rejected.validation_status == clc::sim::runtime_action_status_rejected, "runtime-rejected action had wrong validation_status");
    require(rejected.error_code == clc::sim::runtime_action_error_action_rejected, "runtime-rejected action returned wrong error_code");
    require(!rejected.validation.ok(), "runtime-rejected action did not return diagnostics");
    require(engine.settlement_resource_amount("riverwatch", "grain") == before_rejected, "runtime-rejected action mutated runtime");

    const auto rejected_json = clc::sim::runtime_action_result_to_json(rejected);
    require(rejected_json.find("\"command_detail\":{") != std::string::npos, "rejected result JSON missing command detail object");
    require(rejected_json.find("\"ok\":false") != std::string::npos, "rejected result JSON missing failed command ok state");
    require(rejected_json.find("\"diagnostics_detail\":[") != std::string::npos, "rejected result JSON missing diagnostics detail array");
    require(rejected_json.find("\"severity\":\"error\"") != std::string::npos, "rejected result JSON missing diagnostic severity");
    require(rejected_json.find("\"path\":") != std::string::npos, "rejected result JSON missing diagnostic path");
    require(rejected_json.find("not enough resource") != std::string::npos, "rejected result JSON missing diagnostic message");

    auto transfer_engine = make_engine();
    const auto riverwatch_before = transfer_engine.settlement_resource_amount("riverwatch", "grain");
    const auto hillford_before = transfer_engine.settlement_resource_amount("hillford", "grain");
    const auto transfer = clc::sim::dispatch_runtime_action_json(
        transfer_engine,
        R"({"action_id":"a7","type":"transfer_resource","payload":{"target_id":"riverwatch","secondary_target_id":"hillford","resource_id":"grain","amount":2}})"
    );
    require(transfer.accepted, "valid transfer_resource action was rejected");
    require(transfer_engine.settlement_resource_amount("riverwatch", "grain") == riverwatch_before - 2, "transfer did not debit source");
    require(transfer_engine.settlement_resource_amount("hillford", "grain") == hillford_before + 2, "transfer did not credit target");

    const auto current_day = transfer_engine.current_day();
    const auto advance = clc::sim::dispatch_runtime_action_json(
        transfer_engine,
        R"({"action_id":"a8","type":"advance_days","payload":{"days":2,"metadata":{"note":"ignore { braces } in nested strings"}}})"
    );
    require(advance.accepted, "valid advance_days action with nested metadata was rejected");
    require(advance.validation_status == clc::sim::runtime_action_status_accepted, "advance_days had wrong validation_status");
    require(transfer_engine.current_day() == current_day + 2, "advance_days did not advance runtime");

    auto legacy_engine = make_engine();
    const auto legacy_day = legacy_engine.current_day();
    const auto payload_word = clc::sim::dispatch_runtime_action_json(
        legacy_engine,
        R"({"action_id":"a9","type":"advance_days","actor_id":"payload","days":1})"
    );
    require(payload_word.accepted, "string value named payload was treated as malformed payload field");
    require(payload_word.validation_status == clc::sim::runtime_action_status_accepted, "payload word legacy action had wrong validation_status");
    require(legacy_engine.current_day() == legacy_day + 1, "payload word legacy action did not advance runtime");

    const auto json = clc::sim::runtime_action_result_to_json(advance);
    require(json.find("\"accepted\":true") != std::string::npos, "result JSON missing accepted status");
    require(json.find("\"validation_status\":\"accepted\"") != std::string::npos, "result JSON missing validation status");
    require(json.find("\"action_id\":\"a8\"") != std::string::npos, "result JSON missing action id");
    require(json.find("\"diagnostics\":0") != std::string::npos, "result JSON missing diagnostics count");
    require(json.find("\"command_detail\":null") != std::string::npos, "advance result JSON should have null command detail");
    require(json.find("\"events_detail\":[") != std::string::npos, "result JSON missing events detail array");
    require(json.find("\"day\":") != std::string::npos, "result JSON missing event day");
    require(json.find("\"type\":") != std::string::npos, "result JSON missing event type");
    require(json.find("\"message\":") != std::string::npos, "result JSON missing event message");
    require(json.find("\"diagnostics_detail\":[]") != std::string::npos, "accepted result JSON missing empty diagnostics detail array");

    return 0;
}
