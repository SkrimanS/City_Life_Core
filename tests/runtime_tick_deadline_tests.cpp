#include "clc/sim/SimulationRuntimeScenario.hpp"
#include "clc/sim/SimulationRuntimeTick.hpp"

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
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "runtime tick deadline bootstrap should succeed");
    auto& runtime = bootstrap.runtime;

    require(clc::sim::add_runtime_resource_delivery_contract(runtime, clc::sim::ResourceDeliveryContract{
        .id = "runtime_tick_deadline",
        .display_name = "Runtime Tick Deadline",
        .issuer_faction_id = "riverwatch",
        .receiver_faction_id = "traders_guild",
        .resource_id = "grain",
        .quantity = 1,
        .reward_coins = 1,
        .due_ticks = clc::hours_to_ticks(3),
    }).ok(), "runtime tick deadline contract should add");

    const auto boundary = clc::sim::advance_runtime_ticks(runtime, clc::hours_to_ticks(3));
    require(boundary.ok(), "runtime tick boundary advance should succeed");
    require(boundary.tick_before == 0, "runtime tick boundary should start at zero");
    require(boundary.tick_after == clc::hours_to_ticks(3), "runtime tick boundary should advance runtime clock");
    require(boundary.contracts.failed_count == 0, "runtime tick deadline should not fail on exact deadline");
    require(clc::sim::contract_by_id(runtime.contracts, "runtime_tick_deadline")->status == clc::sim::ContractStatus::open, "runtime tick deadline contract should stay open on boundary");
    require(runtime.engine.current_day() == 0, "runtime tick boundary should not advance daily engine");

    const auto after_deadline = clc::sim::advance_runtime_ticks(runtime, 1);
    require(after_deadline.ok(), "runtime tick after-deadline advance should succeed");
    require(after_deadline.tick_before == clc::hours_to_ticks(3), "runtime tick after-deadline should start after boundary");
    require(after_deadline.tick_after == clc::hours_to_ticks(3) + 1, "runtime tick after-deadline should advance one tick");
    require(after_deadline.contracts.current_tick == clc::hours_to_ticks(3) + 1, "runtime tick deadline report should expose current tick");
    require(after_deadline.contracts.failed_count == 1, "runtime tick deadline should fail after deadline tick");
    require(after_deadline.contracts.failed_contract_ids.size() == 1, "runtime tick deadline should report failed id");
    require(after_deadline.contracts.failed_contract_ids[0] == "runtime_tick_deadline", "runtime tick deadline should report expected id");
    require(clc::sim::contract_by_id(runtime.contracts, "runtime_tick_deadline")->status == clc::sim::ContractStatus::failed, "runtime tick deadline should mark contract failed");
    require(runtime.engine.current_day() == 0, "runtime tick deadline should not require daily engine advancement");

    return 0;
}
