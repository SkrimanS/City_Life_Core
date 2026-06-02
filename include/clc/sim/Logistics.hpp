#pragma once

#include "clc/economy/Market.hpp"
#include "clc/sim/Caravans.hpp"
#include "clc/sim/Contracts.hpp"
#include "clc/sim/Routes.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct LogisticsCargoSignal final {
    std::string caravan_id{};
    std::string resource_id{};
    std::uint64_t amount{0};
    std::uint64_t market_demand{0};
    std::uint64_t market_supply{0};
    std::uint64_t market_pressure_basis_points{0};
    economy::MarketPressureLevel market_pressure{economy::MarketPressureLevel::balanced};
    bool supports_market_shortage{false};
    bool may_fulfill_open_contract{false};
};

struct CaravanLogisticsSignal final {
    std::string caravan_id{};
    std::string route_id{};
    std::string origin_settlement_id{};
    std::string destination_settlement_id{};
    clc::GameTime::Tick total_travel_ticks{0};
    clc::GameTime::Tick ticks_remaining{0};
    std::uint64_t total_cargo{0};
    bool arrived{false};
    bool empty{true};
    bool route_known{false};
    std::vector<LogisticsCargoSignal> cargo{};
};

struct LogisticsNetworkSnapshot final {
    std::uint64_t route_count{0};
    std::uint64_t caravan_count{0};
    std::uint64_t active_caravan_count{0};
    std::uint64_t arrived_caravan_count{0};
    std::uint64_t empty_caravan_count{0};
    std::uint64_t unknown_route_caravan_count{0};
    std::uint64_t total_cargo{0};
    std::uint64_t cargo_supporting_shortage_count{0};
    std::uint64_t cargo_supporting_contract_count{0};
    std::vector<CaravanLogisticsSignal> caravans{};
};

[[nodiscard]] LogisticsNetworkSnapshot make_logistics_network_snapshot(
    const SettlementRouteCatalog& routes,
    const CaravanFleet& fleet
);

[[nodiscard]] LogisticsNetworkSnapshot make_logistics_network_snapshot_with_market(
    const SettlementRouteCatalog& routes,
    const CaravanFleet& fleet,
    const economy::MarketSnapshot& market_snapshot
);

[[nodiscard]] LogisticsNetworkSnapshot make_logistics_network_snapshot_with_market_and_contracts(
    const SettlementRouteCatalog& routes,
    const CaravanFleet& fleet,
    const economy::MarketSnapshot& market_snapshot,
    const ContractCatalog& contracts
);

[[nodiscard]] const CaravanLogisticsSignal* logistics_signal_by_caravan(
    const LogisticsNetworkSnapshot& snapshot,
    std::string_view caravan_id
) noexcept;

[[nodiscard]] const LogisticsCargoSignal* logistics_cargo_signal(
    const CaravanLogisticsSignal& caravan,
    std::string_view resource_id
) noexcept;

[[nodiscard]] std::string logistics_network_snapshot_digest(const LogisticsNetworkSnapshot& snapshot);

} // namespace clc::sim
