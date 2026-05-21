#pragma once

#include "clc/core/Time.hpp"
#include "clc/data/Validation.hpp"
#include "clc/sim/Routes.hpp"
#include "clc/sim/Settlement.hpp"
#include "clc/sim/Storage.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct CaravanState final {
    std::string id{};
    std::string display_name{};
    std::string route_id{};
    std::string origin_settlement_id{};
    std::string destination_settlement_id{};
    std::uint64_t total_travel_days{0};
    std::uint64_t days_remaining{0};
    clc::GameTime::Tick total_travel_ticks{0};
    clc::GameTime::Tick ticks_remaining{0};
    ResourceStorage cargo{};
};

struct CaravanAdvanceReport final {
    std::string caravan_id{};
    std::string route_id{};
    std::uint64_t days_remaining_before{0};
    std::uint64_t days_remaining_after{0};
    clc::GameTime::Tick ticks_remaining_before{0};
    clc::GameTime::Tick ticks_remaining_after{0};
    clc::GameTime::Tick ticks_elapsed{0};
    bool moved{false};
    bool arrived{false};
};

struct CaravanFleet final {
    std::vector<CaravanState> caravans{};
};

[[nodiscard]] CaravanState create_caravan_for_route(
    const SettlementRoute& route,
    std::string id,
    std::string display_name,
    ResourceStorage cargo = {}
);

[[nodiscard]] data::ValidationReport validate_caravan(const CaravanState& caravan);
[[nodiscard]] data::ValidationReport validate_caravan_for_route(const CaravanState& caravan, const SettlementRoute& route);
[[nodiscard]] bool caravan_arrived(const CaravanState& caravan) noexcept;
[[nodiscard]] CaravanAdvanceReport advance_caravan_ticks(CaravanState& caravan, clc::GameTime::Tick ticks);
[[nodiscard]] CaravanAdvanceReport advance_caravan_day(CaravanState& caravan);

[[nodiscard]] data::ValidationReport load_caravan_at_origin(
    CaravanState& caravan,
    SettlementState& origin,
    std::string_view resource_id,
    std::uint64_t amount
);

[[nodiscard]] data::ValidationReport unload_caravan_at_destination(
    CaravanState& caravan,
    SettlementState& destination,
    std::string_view resource_id,
    std::uint64_t amount
);

[[nodiscard]] data::ValidationReport add_caravan(CaravanFleet& fleet, CaravanState caravan);
[[nodiscard]] std::uint64_t caravan_count(const CaravanFleet& fleet) noexcept;
[[nodiscard]] const CaravanState* caravan_by_id(const CaravanFleet& fleet, std::string_view caravan_id) noexcept;
[[nodiscard]] std::vector<CaravanState> active_caravans(const CaravanFleet& fleet);
[[nodiscard]] std::vector<CaravanState> arrived_caravans(const CaravanFleet& fleet);
[[nodiscard]] clc::GameTime::Tick caravan_total_travel_ticks(const CaravanState& caravan) noexcept;
[[nodiscard]] clc::GameTime::Tick caravan_ticks_remaining(const CaravanState& caravan) noexcept;

} // namespace clc::sim
