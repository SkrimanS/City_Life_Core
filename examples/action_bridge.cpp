#include "clc/CityLifeCore.hpp"

#include <iostream>

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        std::cerr << "failed to create runtime scenario\n";
        return 1;
    }

    auto& engine = bootstrap.runtime.engine;

    const auto add_result = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"example-add","type":"add_resource","actor_id":"tool","payload":{"target_id":"riverwatch","resource_id":"grain","amount":10}})"
    );
    if (!add_result.accepted) {
        std::cerr << "add_resource action was rejected\n";
        std::cout << clc::sim::runtime_action_result_to_json(add_result) << '\n';
        return 1;
    }

    std::cout << clc::sim::runtime_action_result_to_json(add_result) << '\n';

    const auto advance_result = clc::sim::dispatch_runtime_action_json(
        engine,
        R"({"action_id":"example-advance","type":"advance_days","actor_id":"tool","payload":{"days":1}})"
    );
    if (!advance_result.accepted) {
        std::cerr << "advance_days action was rejected\n";
        std::cout << clc::sim::runtime_action_result_to_json(advance_result) << '\n';
        return 1;
    }

    std::cout << clc::sim::runtime_action_result_to_json(advance_result) << '\n';
    std::cout << "riverwatch grain=" << engine.settlement_resource_amount("riverwatch", "grain") << '\n';
    std::cout << "current day=" << engine.current_day() << '\n';

    return 0;
}
