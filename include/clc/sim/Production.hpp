#pragma once

#include "clc/data/DataRegistry.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Settlement.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

enum class ProductionResourceRole {
    input,
    output,
};

struct ProductionResourceSignal final {
    std::string building_definition_id{};
    std::string resource_id{};
    ProductionResourceRole role{ProductionResourceRole::input};
    std::uint64_t stored_amount{0};
    std::uint64_t market_supply{0};
    std::uint64_t market_demand{0};
    std::uint64_t market_pressure_basis_points{0};
    economy::MarketPressureLevel market_pressure{economy::MarketPressureLevel::balanced};
    bool available_locally{false};
    bool market_shortage{false};
};

struct BuildingProductionSignal final {
    std::string definition_id{};
    std::string display_name{};
    std::uint32_t assigned_workers{0};
    std::uint32_t worker_slots{0};
    std::uint32_t idle_worker_slots{0};
    bool known_definition{false};
    bool has_workers{false};
    bool has_required_inputs{true};
    bool output_has_market_pressure{false};
    std::vector<ProductionResourceSignal> resources{};
    std::string reason{};
};

struct SettlementProductionSnapshot final {
    std::string settlement_id{};
    std::string display_name{};
    std::uint64_t population{0};
    std::uint64_t total_stored_resources{0};
    std::uint64_t building_count{0};
    std::uint64_t active_building_count{0};
    std::uint64_t blocked_building_count{0};
    std::uint64_t unknown_building_count{0};
    std::uint64_t total_worker_slots{0};
    std::uint64_t assigned_workers{0};
    std::uint64_t idle_worker_slots{0};
    std::uint64_t missing_input_count{0};
    std::uint64_t pressured_output_count{0};
    std::string highest_pressure_output_resource_id{};
    std::vector<BuildingProductionSignal> buildings{};
};

[[nodiscard]] std::string_view production_resource_role_name(ProductionResourceRole role) noexcept;

[[nodiscard]] SettlementProductionSnapshot make_settlement_production_snapshot(
    const SettlementState& settlement,
    const data::DataRegistry& registry
);

[[nodiscard]] SettlementProductionSnapshot make_settlement_production_snapshot_with_market(
    const SettlementState& settlement,
    const data::DataRegistry& registry,
    const economy::MarketSnapshot& market_snapshot
);

[[nodiscard]] const BuildingProductionSignal* production_signal_by_building(
    const SettlementProductionSnapshot& snapshot,
    std::string_view building_definition_id
) noexcept;

[[nodiscard]] const ProductionResourceSignal* production_resource_signal(
    const BuildingProductionSignal& building,
    std::string_view resource_id,
    ProductionResourceRole role
) noexcept;

[[nodiscard]] std::string settlement_production_snapshot_digest(const SettlementProductionSnapshot& snapshot);

} // namespace clc::sim
