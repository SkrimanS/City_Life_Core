#pragma once

#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Settlement.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace clc::sim {

struct SimulationEvent final {
    std::uint64_t day{0};
    std::string type{};
    std::string message{};
};

struct SimulationSnapshot final {
    std::uint64_t day{0};
    std::vector<SettlementReport> settlements{};
    economy::MarketReport market{};
};

struct SimulationDayReport final {
    std::uint64_t day{0};
    std::vector<SettlementTickReport> settlement_ticks{};
    std::vector<SettlementReport> settlements{};
    economy::MarketReport market{};
    std::vector<SimulationEvent> events{};
    std::vector<std::string> warnings{};
};

class SimulationEngine final {
public:
    explicit SimulationEngine(data::DataRegistry registry);

    [[nodiscard]] data::DataRegistry& registry() noexcept;
    [[nodiscard]] const data::DataRegistry& registry() const noexcept;

    [[nodiscard]] economy::MarketState& market() noexcept;
    [[nodiscard]] const economy::MarketState& market() const noexcept;

    [[nodiscard]] data::ValidationReport add_settlement(SettlementState settlement);
    [[nodiscard]] data::ValidationReport create_settlement(std::string settlement_definition_id);
    [[nodiscard]] data::ValidationReport add_building_to_settlement(std::string settlement_id, BuildingInstance building);

    [[nodiscard]] const std::vector<SettlementState>& settlements() const noexcept;
    [[nodiscard]] std::uint64_t current_day() const noexcept;

    [[nodiscard]] SimulationSnapshot snapshot() const;
    [[nodiscard]] SimulationDayReport advance_day();

private:
    data::DataRegistry registry_{};
    economy::MarketState market_{};
    std::vector<SettlementState> settlements_{};
    std::uint64_t current_day_{0};
};

} // namespace clc::sim
