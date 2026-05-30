#include "clc/CityLifeCore.hpp"
#include "clc/data/DataPackLoader.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace {

bool contains_message(const clc::data::ValidationReport& report, std::string_view needle) {
    for (const auto& message : report.messages()) {
        if (message.message.find(needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string_view view_string(const std::string& value) {
    return std::string_view{value.data(), value.size()};
}

} // namespace

int main() {
    clc::data::DataRegistry registry;
    clc::data::DataPackLoader loader;

    const auto invalid_pack = loader.load_string("validation-hardening-example", R"CLC(
schema_version=0.2.2

[resource]
id=worthless
display_name=Worthless Resource
category=test
base_value=0
)CLC", registry);

    if (invalid_pack.ok() || !contains_message(invalid_pack, "base_value")) {
        std::cerr << "expected data pack validation failure\n";
        return 1;
    }

    std::cout << "data pack rejected with " << invalid_pack.error_count() << " error(s)\n";

    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        std::cerr << "failed to create runtime scenario\n";
        return 1;
    }

    auto& engine = bootstrap.runtime.engine;
    const auto before_grain = engine.settlement_resource_amount("riverwatch", "grain");

    const auto rejected = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"hardening-unknown-resource","type":"add_resource","actor_id":"example","payload":{"target_id":"riverwatch","resource_id":"unknown_resource","amount":5}})"
    );

    if (rejected.accepted || view_string(rejected.validation_status) != clc::sim::runtime_action_status_rejected) {
        std::cerr << "expected runtime action rejection\n";
        return 1;
    }

    if (engine.settlement_resource_amount("riverwatch", "grain") != before_grain
        || engine.settlement_resource_amount("riverwatch", "unknown_resource") != 0) {
        std::cerr << "rejected action mutated runtime\n";
        return 1;
    }

    std::cout << clc::sim::runtime_action_result_to_json(rejected) << '\n';
    std::cout << "rejected action preserved runtime state\n";
    return 0;
}
