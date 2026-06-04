#include "clc/CityLifeCore.hpp"

#include <iostream>
#include <string_view>

namespace {

void print_validation(const clc::data::ValidationReport& report) {
    for (const auto& message : report.messages()) {
        std::cerr << message.path << ": " << message.message << '\n';
    }
}

bool require_ok(std::string_view step, const clc::data::ValidationReport& report) {
    if (report.ok()) {
        return true;
    }

    std::cerr << "Failed step: " << step << '\n';
    print_validation(report);
    return false;
}

const clc::sim::SettlementState* require_settlement(
    const clc::sim::SimulationRuntime& runtime,
    std::string_view settlement_id
) {
    const auto* settlement = runtime.engine.settlement(settlement_id);
    if (settlement == nullptr) {
        std::cerr << "Missing settlement: " << settlement_id << '\n';
    }
    return settlement;
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    if (!bootstrap.ok()) {
        std::cerr << "failed to create basic runtime scenario\n";
        print_validation(bootstrap.validation);
        return 1;
    }

    auto& runtime = bootstrap.runtime;

    if (!require_ok("add farm building", runtime.engine.add_building_to_settlement("riverwatch", clc::sim::BuildingInstance{
        .definition_id = "farm",
        .assigned_workers = 2,
    }))) {
        return 1;
    }

    if (!require_ok("set grain market demand", runtime.engine.market().set_demand("grain", 120))) {
        return 1;
    }

    auto caravan = clc::sim::create_runtime_caravan_for_route(
        runtime,
        "riverwatch_to_hillford",
        "deep_systems_caravan",
        "Deep Systems Caravan"
    );
    if (!caravan.ok()) {
        std::cerr << "failed to create caravan\n";
        print_validation(caravan.validation);
        return 1;
    }

    if (!require_ok("load grain cargo", clc::sim::load_runtime_caravan_at_origin(runtime, "deep_systems_caravan", "grain", 10))) {
        return 1;
    }

    const auto snapshot = runtime.engine.snapshot();
    const auto market = clc::economy::make_market_snapshot(snapshot.market);
    const auto ledger = clc::economy::make_ledger_summary(runtime.ledger);
    const auto contracts = clc::sim::make_contract_lifecycle_summary_for_factions(
        runtime.contracts,
        runtime.factions,
        runtime.time.current_tick()
    );

    const auto* riverwatch = require_settlement(runtime, "riverwatch");
    if (riverwatch == nullptr) {
        return 1;
    }

    const auto production = clc::sim::make_settlement_production_snapshot_with_market(
        *riverwatch,
        runtime.engine.registry(),
        market
    );

    const auto logistics = clc::sim::make_logistics_network_snapshot_with_market_and_contracts(
        runtime.routes,
        runtime.caravans,
        market,
        runtime.contracts
    );

    const auto diagnostics = clc::sim::make_deep_systems_diagnostics(
        market,
        ledger,
        contracts,
        production,
        logistics
    );

    const auto access = clc::sim::make_faction_access_report(runtime.factions, "riverwatch", "traders_guild");

    std::cout << "City Life Core deep systems foundation example\n";
    std::cout << clc::economy::market_snapshot_digest(market) << '\n';
    std::cout << clc::economy::ledger_summary_digest(ledger) << '\n';
    std::cout << clc::sim::contract_lifecycle_summary_digest(contracts) << '\n';
    std::cout << clc::sim::settlement_production_snapshot_digest(production) << '\n';
    std::cout << clc::sim::logistics_network_snapshot_digest(logistics) << '\n';
    std::cout << clc::sim::faction_access_report_digest(access) << '\n';
    std::cout << clc::sim::deep_systems_diagnostics_digest(diagnostics) << '\n';
    std::cout << '\n' << clc::sim::deep_systems_diagnostics_markdown(diagnostics);

    return diagnostics.critical_count == 0 ? 0 : 0;
}
