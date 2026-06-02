#include "clc/sim/Logistics.hpp"

#include <algorithm>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

std::uint64_t saturating_add(std::uint64_t lhs, std::uint64_t rhs) noexcept {
    if (rhs > std::numeric_limits<std::uint64_t>::max() - lhs) {
        return std::numeric_limits<std::uint64_t>::max();
    }
    return lhs + rhs;
}

bool open_contract_needs_resource(const ContractCatalog* contracts, std::string_view resource_id) noexcept {
    if (contracts == nullptr) {
        return false;
    }

    for (const auto& contract : contracts->contracts) {
        if (contract_is_open(contract) && contract.resource_id == resource_id) {
            return true;
        }
    }
    return false;
}

LogisticsNetworkSnapshot make_snapshot_impl(
    const SettlementRouteCatalog& routes,
    const CaravanFleet& fleet,
    const economy::MarketSnapshot* market_snapshot,
    const ContractCatalog* contracts
) {
    LogisticsNetworkSnapshot snapshot{
        .route_count = routes.routes.size(),
        .caravan_count = fleet.caravans.size(),
    };

    snapshot.caravans.reserve(fleet.caravans.size());

    for (const auto& caravan : fleet.caravans) {
        CaravanLogisticsSignal signal{
            .caravan_id = caravan.id,
            .route_id = caravan.route_id,
            .origin_settlement_id = caravan.origin_settlement_id,
            .destination_settlement_id = caravan.destination_settlement_id,
            .total_travel_ticks = caravan_total_travel_ticks(caravan),
            .ticks_remaining = caravan_ticks_remaining(caravan),
            .arrived = caravan_arrived(caravan),
            .route_known = settlement_route_by_id(routes, caravan.route_id) != nullptr,
        };

        if (signal.arrived) {
            ++snapshot.arrived_caravan_count;
        } else {
            ++snapshot.active_caravan_count;
        }
        if (!signal.route_known) {
            ++snapshot.unknown_route_caravan_count;
        }

        signal.cargo.reserve(caravan.cargo.entries().size());
        for (const auto& [resource_id, amount] : caravan.cargo.entries()) {
            LogisticsCargoSignal cargo{
                .caravan_id = caravan.id,
                .resource_id = resource_id,
                .amount = amount,
                .may_fulfill_open_contract = open_contract_needs_resource(contracts, resource_id),
            };

            if (market_snapshot != nullptr) {
                if (const auto* market_signal = economy::market_signal_by_resource(*market_snapshot, resource_id)) {
                    cargo.market_demand = market_signal->demand;
                    cargo.market_supply = market_signal->supply;
                    cargo.market_pressure_basis_points = market_signal->pressure_basis_points;
                    cargo.market_pressure = market_signal->pressure;
                    cargo.supports_market_shortage = market_signal->pressure == economy::MarketPressureLevel::shortage
                        || market_signal->pressure == economy::MarketPressureLevel::depleted;
                }
            }

            signal.total_cargo = saturating_add(signal.total_cargo, amount);
            snapshot.total_cargo = saturating_add(snapshot.total_cargo, amount);
            if (cargo.supports_market_shortage) {
                ++snapshot.cargo_supporting_shortage_count;
            }
            if (cargo.may_fulfill_open_contract) {
                ++snapshot.cargo_supporting_contract_count;
            }

            signal.cargo.push_back(std::move(cargo));
        }

        signal.empty = signal.total_cargo == 0;
        if (signal.empty) {
            ++snapshot.empty_caravan_count;
        }

        std::sort(signal.cargo.begin(), signal.cargo.end(), [](const LogisticsCargoSignal& lhs, const LogisticsCargoSignal& rhs) {
            return lhs.resource_id < rhs.resource_id;
        });

        snapshot.caravans.push_back(std::move(signal));
    }

    std::sort(snapshot.caravans.begin(), snapshot.caravans.end(), [](const CaravanLogisticsSignal& lhs, const CaravanLogisticsSignal& rhs) {
        return lhs.caravan_id < rhs.caravan_id;
    });

    return snapshot;
}

} // namespace

LogisticsNetworkSnapshot make_logistics_network_snapshot(
    const SettlementRouteCatalog& routes,
    const CaravanFleet& fleet
) {
    return make_snapshot_impl(routes, fleet, nullptr, nullptr);
}

LogisticsNetworkSnapshot make_logistics_network_snapshot_with_market(
    const SettlementRouteCatalog& routes,
    const CaravanFleet& fleet,
    const economy::MarketSnapshot& market_snapshot
) {
    const auto* market_ptr = market_snapshot.signals.empty() ? nullptr : &market_snapshot;
    return make_snapshot_impl(routes, fleet, market_ptr, nullptr);
}

LogisticsNetworkSnapshot make_logistics_network_snapshot_with_market_and_contracts(
    const SettlementRouteCatalog& routes,
    const CaravanFleet& fleet,
    const economy::MarketSnapshot& market_snapshot,
    const ContractCatalog& contracts
) {
    const auto* market_ptr = market_snapshot.signals.empty() ? nullptr : &market_snapshot;
    return make_snapshot_impl(routes, fleet, market_ptr, &contracts);
}

const CaravanLogisticsSignal* logistics_signal_by_caravan(
    const LogisticsNetworkSnapshot& snapshot,
    std::string_view caravan_id
) noexcept {
    for (const auto& caravan : snapshot.caravans) {
        if (caravan.caravan_id == caravan_id) {
            return &caravan;
        }
    }
    return nullptr;
}

const LogisticsCargoSignal* logistics_cargo_signal(
    const CaravanLogisticsSignal& caravan,
    std::string_view resource_id
) noexcept {
    for (const auto& cargo : caravan.cargo) {
        if (cargo.resource_id == resource_id) {
            return &cargo;
        }
    }
    return nullptr;
}

std::string logistics_network_snapshot_digest(const LogisticsNetworkSnapshot& snapshot) {
    std::ostringstream out;
    out << "logistics_network"
        << ";routes=" << snapshot.route_count
        << ";caravans=" << snapshot.caravan_count
        << ";active=" << snapshot.active_caravan_count
        << ";arrived=" << snapshot.arrived_caravan_count
        << ";empty=" << snapshot.empty_caravan_count
        << ";unknown_route=" << snapshot.unknown_route_caravan_count
        << ";total_cargo=" << snapshot.total_cargo
        << ";shortage_cargo=" << snapshot.cargo_supporting_shortage_count
        << ";contract_cargo=" << snapshot.cargo_supporting_contract_count;
    return out.str();
}

} // namespace clc::sim
