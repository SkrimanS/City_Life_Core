#pragma once

#include "clc/core/Time.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/Settlement.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct SettlementRoute final {
    std::string id{};
    std::string display_name{};
    std::string origin_settlement_id{};
    std::string destination_settlement_id{};
    std::uint64_t travel_days{0};
    clc::GameTime::Tick travel_ticks{0};
};

struct SettlementRouteCatalog final {
    std::vector<SettlementRoute> routes{};
};

[[nodiscard]] SettlementRoute make_settlement_route_days(
    std::string id,
    std::string display_name,
    std::string origin_settlement_id,
    std::string destination_settlement_id,
    std::uint64_t travel_days
);

[[nodiscard]] SettlementRoute make_settlement_route_ticks(
    std::string id,
    std::string display_name,
    std::string origin_settlement_id,
    std::string destination_settlement_id,
    clc::GameTime::Tick travel_ticks
);

[[nodiscard]] data::ValidationReport validate_settlement_route(const SettlementRoute& route);
[[nodiscard]] data::ValidationReport validate_settlement_route_for_settlements(
    const SettlementRoute& route,
    const std::vector<SettlementState>& settlements
);
[[nodiscard]] data::ValidationReport add_settlement_route(SettlementRouteCatalog& catalog, SettlementRoute route);
[[nodiscard]] std::uint64_t settlement_route_count(const SettlementRouteCatalog& catalog) noexcept;
[[nodiscard]] const SettlementRoute* settlement_route_by_id(const SettlementRouteCatalog& catalog, std::string_view route_id) noexcept;
[[nodiscard]] std::vector<SettlementRoute> settlement_routes_from(const SettlementRouteCatalog& catalog, std::string_view settlement_id);
[[nodiscard]] std::vector<SettlementRoute> settlement_routes_to(const SettlementRouteCatalog& catalog, std::string_view settlement_id);
[[nodiscard]] clc::GameTime::Tick settlement_route_travel_ticks(const SettlementRoute& route) noexcept;

} // namespace clc::sim
