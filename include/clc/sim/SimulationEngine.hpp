#pragma once

#include "clc/data/DataRegistry.hpp"
#include "clc/data/Validation.hpp"
#include "clc/economy/Market.hpp"
#include "clc/sim/Settlement.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct SimulationEvent final {
    std::uint64_t day{0};
    std::string type{};
    std::string message{};
};

struct SimulationCommandResult final {
    std::string command{};
    bool ok{false};
    std::string subject_id{};
    std::string target_id{};
    std::string resource_id{};
    std::uint64_t amount{0};
    data::ValidationReport validation{};
};

struct SimulationSnapshot final {
    std::uint64_t day{0};
    std::vector<SettlementReport> settlements{};
    economy::MarketReport market{};
    std::vector<SimulationEvent> events{};
};

struct SimulationDayReport final {
    std::uint64_t day{0};
    std::vector<SettlementTickReport> settlement_ticks{};
    std::vector<SettlementReport> settlements{};
    economy::MarketReport market{};
    std::vector<SimulationEvent> events{};
    std::vector<std::string> warnings{};
};

struct SimulationScenarioSummary final {
    std::uint64_t days_run{0};
    std::uint64_t first_day{0};
    std::uint64_t last_day{0};
    std::uint64_t settlement_ticks{0};
    std::uint64_t consumed_food{0};
    std::uint64_t consumed_inputs{0};
    std::uint64_t produced_resources{0};
    std::uint64_t active_building_ticks{0};
    std::uint64_t skipped_building_ticks{0};
    std::uint64_t events{0};
    std::uint64_t warnings{0};
};

struct SimulationScenarioResult final {
    SimulationSnapshot initial_snapshot{};
    std::vector<SimulationDayReport> reports{};
    SimulationScenarioSummary summary{};
    std::vector<SimulationEvent> events_delta{};
    std::vector<std::string> warnings_delta{};
    SimulationSnapshot final_snapshot{};
};

[[nodiscard]] SimulationScenarioSummary summarize_day_reports(const std::vector<SimulationDayReport>& reports);
[[nodiscard]] std::uint64_t scenario_summary_start_day(const SimulationScenarioSummary& summary) noexcept;
[[nodiscard]] std::uint64_t scenario_summary_end_day(const SimulationScenarioSummary& summary) noexcept;
[[nodiscard]] std::uint64_t scenario_summary_duration_days(const SimulationScenarioSummary& summary) noexcept;
[[nodiscard]] std::uint64_t scenario_result_start_day(const SimulationScenarioResult& result) noexcept;
[[nodiscard]] std::uint64_t scenario_result_end_day(const SimulationScenarioResult& result) noexcept;
[[nodiscard]] std::uint64_t scenario_result_duration_days(const SimulationScenarioResult& result) noexcept;
[[nodiscard]] bool scenario_has_events(const SimulationScenarioResult& result) noexcept;
[[nodiscard]] bool scenario_has_warnings(const SimulationScenarioResult& result) noexcept;
[[nodiscard]] bool scenario_succeeded(const SimulationScenarioResult& result) noexcept;
[[nodiscard]] std::string scenario_result_digest(const SimulationScenarioResult& result);

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
    [[nodiscard]] data::ValidationReport add_resource_to_settlement(std::string settlement_id, std::string resource_id, std::uint64_t amount);
    [[nodiscard]] data::ValidationReport remove_resource_from_settlement(std::string settlement_id, std::string resource_id, std::uint64_t amount);
    [[nodiscard]] data::ValidationReport transfer_resource_between_settlements(
        std::string from_settlement_id,
        std::string to_settlement_id,
        std::string resource_id,
        std::uint64_t amount
    );

    [[nodiscard]] SimulationCommandResult create_settlement_command(std::string settlement_definition_id);
    [[nodiscard]] SimulationCommandResult add_building_to_settlement_command(std::string settlement_id, BuildingInstance building);
    [[nodiscard]] SimulationCommandResult add_resource_to_settlement_command(std::string settlement_id, std::string resource_id, std::uint64_t amount);
    [[nodiscard]] SimulationCommandResult remove_resource_from_settlement_command(std::string settlement_id, std::string resource_id, std::uint64_t amount);
    [[nodiscard]] SimulationCommandResult transfer_resource_between_settlements_command(
        std::string from_settlement_id,
        std::string to_settlement_id,
        std::string resource_id,
        std::uint64_t amount
    );

    [[nodiscard]] const std::vector<SettlementState>& settlements() const noexcept;
    [[nodiscard]] bool has_settlement(std::string_view settlement_id) const;
    [[nodiscard]] const SettlementState* settlement(std::string_view settlement_id) const;
    [[nodiscard]] std::uint64_t settlement_resource_amount(std::string_view settlement_id, std::string_view resource_id) const;
    [[nodiscard]] const std::vector<SimulationEvent>& events() const noexcept;
    [[nodiscard]] std::vector<SimulationEvent> recent_events(std::uint64_t max_count) const;
    [[nodiscard]] std::vector<SimulationEvent> events_by_type(std::string_view event_type) const;
    void clear_events() noexcept;
    [[nodiscard]] std::uint64_t current_day() const noexcept;

    [[nodiscard]] SimulationSnapshot snapshot() const;
    [[nodiscard]] SimulationDayReport advance_day();
    [[nodiscard]] std::vector<SimulationDayReport> run_days(std::uint64_t day_count);
    [[nodiscard]] SimulationScenarioResult run_scenario(std::uint64_t day_count);

private:
    data::DataRegistry registry_{};
    economy::MarketState market_{};
    std::vector<SettlementState> settlements_{};
    std::vector<SimulationEvent> events_{};
    std::uint64_t current_day_{0};
};

} // namespace clc::sim
