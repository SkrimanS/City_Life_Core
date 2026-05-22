#include "clc/sim/Settlement.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

clc::data::DataRegistry make_registry() {
    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "grain should register");
    require(registry.add(clc::data::ResourceDefinition{.id = "wood", .display_name = "Wood", .category = "construction", .base_value = 6}).ok(), "wood should register");
    return registry;
}

} // namespace

int main() {
    const auto max_u64 = std::numeric_limits<std::uint64_t>::max();
    auto registry = make_registry();

    {
        clc::sim::SettlementState settlement{
            .id = "huge_population",
            .display_name = "Huge Population",
            .population = max_u64,
        };

        const auto report = clc::sim::advance_settlement_ticks(settlement, registry, clc::ticks_per_day());
        require(report.elapsed_ticks == clc::ticks_per_day(), "tick report should preserve elapsed ticks");
        require(!report.warnings.empty(), "huge population without food should warn instead of overflowing");
        require(settlement.tick_remainders.size() == 1, "food remainder should be tracked");
        require(settlement.tick_remainders[0].numerator < clc::ticks_per_day(), "food remainder numerator should stay below denominator");
    }

    {
        clc::sim::SettlementState settlement{
            .id = "huge_ticks",
            .display_name = "Huge Ticks",
            .population = max_u64,
        };

        const auto report = clc::sim::advance_settlement_ticks(settlement, registry, max_u64);
        require(report.elapsed_ticks == max_u64, "tick report should preserve huge elapsed ticks");
        require(!report.warnings.empty(), "huge tick scaling without food should warn instead of overflowing");
        require(settlement.tick_remainders.size() == 1, "huge tick scaling should still create one food remainder");
        require(settlement.tick_remainders[0].numerator < clc::ticks_per_day(), "huge tick scaling remainder should stay normalized");
    }

    {
        clc::sim::SettlementState settlement{
            .id = "storage_total",
            .display_name = "Storage Total",
            .population = 1,
        };
        require(settlement.storage.add("grain", max_u64).ok(), "storage should accept max grain");
        require(settlement.storage.add("wood", 1).ok(), "storage should accept wood");

        const auto report = clc::sim::make_settlement_report(settlement, registry);
        require(report.storage.size() == 2, "settlement report should include both storage entries");
        require(report.total_stored_resources == max_u64, "settlement report total should saturate instead of wrapping");
    }

    return 0;
}
