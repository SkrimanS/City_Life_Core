#include "clc/sim/SimulationEngine.hpp"

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

} // namespace

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

const std::vector<SettlementState>& SimulationEngine::settlements() const noexcept {
    return settlements_;
}

std::uint64_t SimulationEngine::current_day() const noexcept {
    return current_day_;
}

SimulationSnapshot SimulationEngine::snapshot() const {
    const auto aggregate = aggregate_storage(settlements_);
    return SimulationSnapshot{
        .day = current_day_,
        .settlements = make_settlement_reports(settlements_, registry_),
        .market = economy::make_market_report(registry_, aggregate, market_),
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
    return report;
}

} // namespace clc::sim
