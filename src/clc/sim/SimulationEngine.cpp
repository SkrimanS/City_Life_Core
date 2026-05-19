#include "clc/sim/SimulationEngine.hpp"

#include <string>
#include <unordered_set>
#include <utility>

namespace clc::sim {
namespace {

ResourceStorage aggregate_storage(const std::vector<SettlementState>& settlements) {
    ResourceStorage aggregate;
    for (const auto& settlement : settlements) {
        for (const auto& [resource_id, amount] : settlement.storage.entries()) {
            const auto add_report = aggregate.add(resource_id, amount);
            (void)add_report;
        }
    }
    return aggregate;
}

void add_event(SimulationDayReport& report, std::string type, std::string message) {
    report.events.push_back(SimulationEvent{
        .day = report.day,
        .type = std::move(type),
        .message = std::move(message),
    });
}

std::vector<SettlementReport> make_settlement_reports(const std::vector<SettlementState>& settlements, const data::DataRegistry& registry) {
    std::vector<SettlementReport> reports;
    reports.reserve(settlements.size());
    for (const auto& settlement : settlements) {
        reports.push_back(make_settlement_report(settlement, registry));
    }
    return reports;
}

SettlementState* find_settlement(std::vector<SettlementState>& settlements, const std::string& settlement_id) {
    for (auto& settlement : settlements) {
        if (settlement.id == settlement_id) {
            return &settlement;
        }
    }
    return nullptr;
}

const SettlementState* find_settlement(const std::vector<SettlementState>& settlements, std::string_view settlement_id) {
    for (const auto& settlement : settlements) {
        if (settlement.id == settlement_id) {
            return &settlement;
        }
    }
    return nullptr;
}

SimulationCommandResult make_command_result(
    std::string command,
    std::string subject_id,
    std::string target_id,
    std::string resource_id,
    std::uint64_t amount,
    data::ValidationReport validation
) {
    const auto ok = validation.ok();
    return SimulationCommandResult{
        .command = std::move(command),
        .ok = ok,
        .subject_id = std::move(subject_id),
        .target_id = std::move(target_id),
        .resource_id = std::move(resource_id),
        .amount = amount,
        .validation = std::move(validation),
    };
}

std::string command_event_type(const SimulationCommandResult& result) {
    return result.ok ? "simulation.command.succeeded" : "simulation.command.failed";
}

std::string command_event_message(const SimulationCommandResult& result) {
    return result.command + (result.ok ? " succeeded" : " failed");
}

} // namespace

SimulationScenarioSummary summarize_day_reports(const std::vector<SimulationDayReport>& reports) {
    SimulationScenarioSummary summary;
    summary.days_run = reports.size();
    if (reports.empty()) {
        return summary;
    }

    summary.first_day = reports.front().day;
    summary.last_day = reports.back().day;

    for (const auto& report : reports) {
        summary.settlement_ticks += report.settlement_ticks.size();
        summary.events += report.events.size();
        summary.warnings += report.warnings.size();

        for (const auto& tick : report.settlement_ticks) {
            summary.consumed_food += tick.consumed_food;
            summary.consumed_inputs += tick.consumed_inputs;
            summary.produced_resources += tick.produced_resources;
            summary.active_building_ticks += tick.active_buildings;
            summary.skipped_building_ticks += tick.skipped_buildings;
            summary.warnings += tick.warnings.size();
        }
    }

    return summary;
}

std::uint64_t scenario_summary_start_day(const SimulationScenarioSummary& summary) noexcept {
    return summary.first_day;
}

std::uint64_t scenario_summary_end_day(const SimulationScenarioSummary& summary) noexcept {
    return summary.last_day;
}

std::uint64_t scenario_summary_duration_days(const SimulationScenarioSummary& summary) noexcept {
    return summary.days_run;
}

std::uint64_t scenario_result_start_day(const SimulationScenarioResult& result) noexcept {
    return result.initial_snapshot.day;
}

std::uint64_t scenario_result_end_day(const SimulationScenarioResult& result) noexcept {
    return result.final_snapshot.day;
}

std::uint64_t scenario_result_duration_days(const SimulationScenarioResult& result) noexcept {
    if (result.final_snapshot.day < result.initial_snapshot.day) {
        return 0;
    }
    return result.final_snapshot.day - result.initial_snapshot.day;
}

bool scenario_has_events(const SimulationScenarioResult& result) noexcept {
    return !result.events_delta.empty();
}

bool scenario_has_warnings(const SimulationScenarioResult& result) noexcept {
    return !result.warnings_delta.empty();
}

bool scenario_succeeded(const SimulationScenarioResult& result) noexcept {
    return !scenario_has_warnings(result);
}

std::string scenario_result_digest(const SimulationScenarioResult& result) {
    std::string digest = "scenario days=" + std::to_string(scenario_result_duration_days(result));
    digest += " start=" + std::to_string(scenario_result_start_day(result));
    digest += " end=" + std::to_string(scenario_result_end_day(result));
    digest += " events=" + std::to_string(result.events_delta.size());
    digest += " warnings=" + std::to_string(result.warnings_delta.size());
    digest += scenario_succeeded(result) ? " status=success" : " status=warning";
    return digest;
}

data::ValidationReport validate_scenario_preset(const SimulationScenarioPreset& preset) {
    data::ValidationReport report;
    if (preset.id.empty()) {
        report.add_error("simulation.scenario_preset", "preset id must not be empty");
    }
    if (preset.display_name.empty()) {
        report.add_error("simulation.scenario_preset." + preset.id, "preset display_name must not be empty");
    }
    if (preset.day_count == 0) {
        report.add_error("simulation.scenario_preset." + preset.id, "preset day_count must be greater than zero");
    }
    return report;
}

std::string scenario_preset_result_digest(const SimulationScenarioPreset& preset, const SimulationScenarioResult& result) {
    std::string digest = "preset id=" + preset.id;
    digest += " name=" + preset.display_name;
    digest += " days=" + std::to_string(preset.day_count);
    digest += " | " + scenario_result_digest(result);
    return digest;
}

data::ValidationReport add_scenario_preset(SimulationScenarioPresetCatalog& catalog, SimulationScenarioPreset preset) {
    auto report = validate_scenario_preset(preset);
    if (!report.ok()) {
        return report;
    }

    if (scenario_preset_by_id(catalog, preset.id) != nullptr) {
        report.add_error("simulation.scenario_preset." + preset.id, "duplicate preset id");
        return report;
    }

    catalog.presets.push_back(std::move(preset));
    return report;
}

std::uint64_t scenario_preset_count(const SimulationScenarioPresetCatalog& catalog) noexcept {
    return catalog.presets.size();
}

const SimulationScenarioPreset* scenario_preset_by_id(const SimulationScenarioPresetCatalog& catalog, std::string_view preset_id) noexcept {
    for (const auto& preset : catalog.presets) {
        if (preset.id == preset_id) {
            return &preset;
        }
    }
    return nullptr;
}

SimulationEngine::SimulationEngine(data::DataRegistry registry)
    : registry_{std::move(registry)} {
}

data::DataRegistry& SimulationEngine::registry() noexcept {
    return registry_;
}

const data::DataRegistry& SimulationEngine::registry() const noexcept {
    return registry_;
}

economy::MarketState& SimulationEngine::market() noexcept {
    return market_;
}

const economy::MarketState& SimulationEngine::market() const noexcept {
    return market_;
}

data::ValidationReport SimulationEngine::add_settlement(SettlementState settlement) {
    data::ValidationReport report;
    if (settlement.id.empty()) {
        report.add_error("simulation.settlement", "settlement id must not be empty");
        return report;
    }

    for (const auto& existing : settlements_) {
        if (existing.id == settlement.id) {
            report.add_error("simulation.settlement." + settlement.id, "duplicate settlement id");
            return report;
        }
    }

    settlements_.push_back(std::move(settlement));
    return report;
}

data::ValidationReport SimulationEngine::create_settlement(std::string settlement_definition_id) {
    data::ValidationReport report;
    if (settlement_definition_id.empty()) {
        report.add_error("simulation.create_settlement", "settlement_definition_id must not be empty");
        return report;
    }

    const auto* definition = registry_.settlement(settlement_definition_id);
    if (definition == nullptr) {
        report.add_error("simulation.create_settlement." + settlement_definition_id, "unknown settlement definition");
        return report;
    }

    return add_settlement(create_settlement_from_definition(*definition));
}

data::ValidationReport SimulationEngine::add_building_to_settlement(std::string settlement_id, BuildingInstance building) {
    data::ValidationReport report;
    if (settlement_id.empty()) {
        report.add_error("simulation.add_building", "settlement_id must not be empty");
        return report;
    }

    for (auto& settlement : settlements_) {
        if (settlement.id == settlement_id) {
            return add_building(settlement, registry_, std::move(building));
        }
    }

    report.add_error("simulation.settlement." + settlement_id, "unknown settlement");
    return report;
}

data::ValidationReport SimulationEngine::add_resource_to_settlement(std::string settlement_id, std::string resource_id, std::uint64_t amount) {
    data::ValidationReport report;
    if (settlement_id.empty()) {
        report.add_error("simulation.add_resource", "settlement_id must not be empty");
        return report;
    }

    for (auto& settlement : settlements_) {
        if (settlement.id == settlement_id) {
            return settlement.storage.add(std::move(resource_id), amount);
        }
    }

    report.add_error("simulation.settlement." + settlement_id, "unknown settlement");
    return report;
}

data::ValidationReport SimulationEngine::remove_resource_from_settlement(std::string settlement_id, std::string resource_id, std::uint64_t amount) {
    data::ValidationReport report;
    if (settlement_id.empty()) {
        report.add_error("simulation.remove_resource", "settlement_id must not be empty");
        return report;
    }
    if (resource_id.empty()) {
        report.add_error("simulation.remove_resource", "resource_id must not be empty");
        return report;
    }
    if (amount == 0) {
        report.add_error("simulation.remove_resource", "amount must be greater than zero");
        return report;
    }

    auto* settlement = find_settlement(settlements_, settlement_id);
    if (settlement == nullptr) {
        report.add_error("simulation.settlement." + settlement_id, "unknown settlement");
        return report;
    }

    if (!settlement->storage.try_remove(resource_id, amount)) {
        report.add_error("simulation.settlement." + settlement_id + ".storage." + resource_id, "not enough resource in settlement storage");
    }
    return report;
}

data::ValidationReport SimulationEngine::transfer_resource_between_settlements(
    std::string from_settlement_id,
    std::string to_settlement_id,
    std::string resource_id,
    std::uint64_t amount
) {
    data::ValidationReport report;
    if (from_settlement_id.empty()) {
        report.add_error("simulation.transfer_resource", "from_settlement_id must not be empty");
        return report;
    }
    if (to_settlement_id.empty()) {
        report.add_error("simulation.transfer_resource", "to_settlement_id must not be empty");
        return report;
    }
    if (from_settlement_id == to_settlement_id) {
        report.add_error("simulation.transfer_resource", "source and target settlements must be different");
        return report;
    }
    if (resource_id.empty()) {
        report.add_error("simulation.transfer_resource", "resource_id must not be empty");
        return report;
    }
    if (amount == 0) {
        report.add_error("simulation.transfer_resource", "amount must be greater than zero");
        return report;
    }

    auto* source = find_settlement(settlements_, from_settlement_id);
    if (source == nullptr) {
        report.add_error("simulation.settlement." + from_settlement_id, "unknown source settlement");
        return report;
    }

    auto* target = find_settlement(settlements_, to_settlement_id);
    if (target == nullptr) {
        report.add_error("simulation.settlement." + to_settlement_id, "unknown target settlement");
        return report;
    }

    return transfer(source->storage, target->storage, resource_id, amount);
}

SimulationCommandResult SimulationEngine::create_settlement_command(std::string settlement_definition_id) {
    auto subject_id = settlement_definition_id;
    auto validation = create_settlement(std::move(settlement_definition_id));
    auto result = make_command_result("create_settlement", std::move(subject_id), {}, {}, 0, std::move(validation));
    events_.push_back(SimulationEvent{.day = current_day_, .type = command_event_type(result), .message = command_event_message(result)});
    return result;
}

SimulationCommandResult SimulationEngine::add_building_to_settlement_command(std::string settlement_id, BuildingInstance building) {
    auto subject_id = settlement_id;
    auto target_id = building.definition_id;
    auto validation = add_building_to_settlement(std::move(settlement_id), std::move(building));
    auto result = make_command_result("add_building_to_settlement", std::move(subject_id), std::move(target_id), {}, 0, std::move(validation));
    events_.push_back(SimulationEvent{.day = current_day_, .type = command_event_type(result), .message = command_event_message(result)});
    return result;
}

SimulationCommandResult SimulationEngine::add_resource_to_settlement_command(std::string settlement_id, std::string resource_id, std::uint64_t amount) {
    auto subject_id = settlement_id;
    auto resource = resource_id;
    auto validation = add_resource_to_settlement(std::move(settlement_id), std::move(resource_id), amount);
    auto result = make_command_result("add_resource_to_settlement", std::move(subject_id), {}, std::move(resource), amount, std::move(validation));
    events_.push_back(SimulationEvent{.day = current_day_, .type = command_event_type(result), .message = command_event_message(result)});
    return result;
}

SimulationCommandResult SimulationEngine::remove_resource_from_settlement_command(std::string settlement_id, std::string resource_id, std::uint64_t amount) {
    auto subject_id = settlement_id;
    auto resource = resource_id;
    auto validation = remove_resource_from_settlement(std::move(settlement_id), std::move(resource_id), amount);
    auto result = make_command_result("remove_resource_from_settlement", std::move(subject_id), {}, std::move(resource), amount, std::move(validation));
    events_.push_back(SimulationEvent{.day = current_day_, .type = command_event_type(result), .message = command_event_message(result)});
    return result;
}

SimulationCommandResult SimulationEngine::transfer_resource_between_settlements_command(
    std::string from_settlement_id,
    std::string to_settlement_id,
    std::string resource_id,
    std::uint64_t amount
) {
    auto subject_id = from_settlement_id;
    auto target_id = to_settlement_id;
    auto resource = resource_id;
    auto validation = transfer_resource_between_settlements(
        std::move(from_settlement_id),
        std::move(to_settlement_id),
        std::move(resource_id),
        amount
    );
    auto result = make_command_result("transfer_resource_between_settlements", std::move(subject_id), std::move(target_id), std::move(resource), amount, std::move(validation));
    events_.push_back(SimulationEvent{.day = current_day_, .type = command_event_type(result), .message = command_event_message(result)});
    return result;
}

const std::vector<SettlementState>& SimulationEngine::settlements() const noexcept {
    return settlements_;
}

bool SimulationEngine::has_settlement(std::string_view settlement_id) const {
    return find_settlement(settlements_, settlement_id) != nullptr;
}

const SettlementState* SimulationEngine::settlement(std::string_view settlement_id) const {
    return find_settlement(settlements_, settlement_id);
}

std::uint64_t SimulationEngine::settlement_resource_amount(std::string_view settlement_id, std::string_view resource_id) const {
    const auto* found = find_settlement(settlements_, settlement_id);
    if (found == nullptr) {
        return 0;
    }
    return found->storage.amount(std::string{resource_id});
}

const std::vector<SimulationEvent>& SimulationEngine::events() const noexcept {
    return events_;
}

std::vector<SimulationEvent> SimulationEngine::recent_events(std::uint64_t max_count) const {
    if (max_count == 0 || events_.empty()) {
        return {};
    }

    const auto count = max_count < events_.size() ? static_cast<std::size_t>(max_count) : events_.size();
    return std::vector<SimulationEvent>{events_.end() - static_cast<std::ptrdiff_t>(count), events_.end()};
}

std::vector<SimulationEvent> SimulationEngine::events_by_type(std::string_view event_type) const {
    std::vector<SimulationEvent> filtered;
    for (const auto& event : events_) {
        if (event.type == event_type) {
            filtered.push_back(event);
        }
    }
    return filtered;
}

void SimulationEngine::clear_events() noexcept {
    events_.clear();
}

std::uint64_t SimulationEngine::current_day() const noexcept {
    return current_day_;
}

SimulationEngineState SimulationEngine::export_state() const {
    SimulationEngineState state;
    state.current_day = current_day_;
    state.settlements = settlements_;
    state.events = events_;
    state.market_demands.reserve(market_.demands().size());
    for (const auto& [resource_id, demand] : market_.demands()) {
        state.market_demands.push_back(SimulationMarketDemand{
            .resource_id = resource_id,
            .demand = demand,
        });
    }
    return state;
}

data::ValidationReport SimulationEngine::restore_state(SimulationEngineState state) {
    data::ValidationReport report;

    std::unordered_set<std::string> settlement_ids;
    for (const auto& settlement : state.settlements) {
        if (settlement.id.empty()) {
            report.add_error("simulation.state.settlements", "settlement id must not be empty");
            continue;
        }
        if (!settlement_ids.insert(settlement.id).second) {
            report.add_error("simulation.state.settlement." + settlement.id, "duplicate settlement id");
        }
    }

    std::unordered_set<std::string> demand_resource_ids;
    for (const auto& demand : state.market_demands) {
        if (demand.resource_id.empty()) {
            report.add_error("simulation.state.market", "market demand resource_id must not be empty");
            continue;
        }
        if (!demand_resource_ids.insert(demand.resource_id).second) {
            report.add_error("simulation.state.market." + demand.resource_id, "duplicate market demand resource id");
        }
    }

    if (!report.ok()) {
        return report;
    }

    current_day_ = state.current_day;
    settlements_ = std::move(state.settlements);
    events_ = std::move(state.events);
    market_ = economy::MarketState{};
    for (auto& demand : state.market_demands) {
        const auto demand_report = market_.set_demand(std::move(demand.resource_id), demand.demand);
        for (const auto& message : demand_report.messages()) {
            if (message.severity == data::ValidationSeverity::error) {
                report.add_error(message.path, message.message);
            } else {
                report.add_warning(message.path, message.message);
            }
        }
    }
    return report;
}

SimulationSnapshot SimulationEngine::snapshot() const {
    const auto aggregate = aggregate_storage(settlements_);
    return SimulationSnapshot{
        .day = current_day_,
        .settlements = make_settlement_reports(settlements_, registry_),
        .market = economy::make_market_report(registry_, aggregate, market_),
        .events = events_,
    };
}

SimulationDayReport SimulationEngine::advance_day() {
    ++current_day_;

    SimulationDayReport report{.day = current_day_};
    report.settlement_ticks.reserve(settlements_.size());
    report.settlements.reserve(settlements_.size());
    report.events.reserve(settlements_.size() + 2);

    add_event(report, "simulation.day.started", "simulation day started");

    for (auto& settlement : settlements_) {
        auto tick_report = advance_settlement_day(settlement, registry_);
        for (const auto& warning : tick_report.warnings) {
            report.warnings.push_back(settlement.id + ": " + warning);
        }
        add_event(report, "simulation.settlement.advanced", "advanced settlement: " + settlement.id);
        report.settlement_ticks.push_back(std::move(tick_report));
        report.settlements.push_back(make_settlement_report(settlement, registry_));
    }

    const auto aggregate = aggregate_storage(settlements_);
    report.market = economy::make_market_report(registry_, aggregate, market_);
    add_event(report, "simulation.day.completed", "simulation day completed");
    for (const auto& event : report.events) {
        events_.push_back(event);
    }
    return report;
}

std::vector<SimulationDayReport> SimulationEngine::run_days(std::uint64_t day_count) {
    std::vector<SimulationDayReport> reports;
    reports.reserve(static_cast<std::size_t>(day_count));
    for (std::uint64_t day_index = 0; day_index < day_count; ++day_index) {
        reports.push_back(advance_day());
    }
    return reports;
}

SimulationScenarioResult SimulationEngine::run_scenario(std::uint64_t day_count) {
    auto initial_snapshot = snapshot();
    const auto initial_event_count = events_.size();
    auto reports = run_days(day_count);
    auto summary = summarize_day_reports(reports);
    auto final_snapshot = snapshot();
    std::vector<SimulationEvent> events_delta;
    if (initial_event_count < final_snapshot.events.size()) {
        events_delta = std::vector<SimulationEvent>{
            final_snapshot.events.begin() + static_cast<std::ptrdiff_t>(initial_event_count),
            final_snapshot.events.end()
        };
    }
    std::vector<std::string> warnings_delta;
    for (const auto& report : reports) {
        for (const auto& warning : report.warnings) {
            warnings_delta.push_back(warning);
        }
    }
    return SimulationScenarioResult{
        .initial_snapshot = std::move(initial_snapshot),
        .reports = std::move(reports),
        .summary = std::move(summary),
        .events_delta = std::move(events_delta),
        .warnings_delta = std::move(warnings_delta),
        .final_snapshot = std::move(final_snapshot),
    };
}

SimulationScenarioResult SimulationEngine::run_scenario_preset(const SimulationScenarioPreset& preset) {
    const auto validation = validate_scenario_preset(preset);
    if (!validation.ok()) {
        return SimulationScenarioResult{
            .initial_snapshot = snapshot(),
            .final_snapshot = snapshot(),
        };
    }
    return run_scenario(preset.day_count);
}

} // namespace clc::sim
