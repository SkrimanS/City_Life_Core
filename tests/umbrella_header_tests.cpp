#include "clc/CityLifeCore.hpp"

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
    require(clc::core_version().major == 1, "umbrella header should expose core version");
    require(clc::core_version().minor == 0, "umbrella header should expose core version minor");
    require(clc::core_version().patch == 0, "umbrella header should expose core version patch");
    require(clc::ticks_per_minute() == 60, "umbrella header should expose time API");

    clc::data::DataRegistry registry;
    require(registry.add(clc::data::ResourceDefinition{.id = "grain", .display_name = "Grain", .category = "food", .base_value = 10}).ok(), "umbrella header should expose data registry API");

    clc::sim::ResourceStorage storage;
    require(storage.add("grain", 2).ok(), "umbrella header should expose storage API");

    clc::economy::Wallet wallet{.coins = 100};
    clc::economy::EconomyLedger ledger;
    const auto trade = clc::economy::buy_resource_with_ledger(
        wallet,
        storage,
        clc::economy::MarketPrice{.resource_id = "grain", .base_value = 10, .supply = 0, .demand = 0, .price = 5, .reason = "test"},
        1,
        ledger,
        "umbrella smoke"
    );
    require(trade.ok, "umbrella header should expose trade ledger wrapper API");

    return 0;
}
