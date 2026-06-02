#include "clc/sim/DeepSystemsDiagnostics.hpp"

#include <sstream>
#include <string>
#include <utility>

namespace clc::sim {
namespace {

void add_warning(DeepSystemsDiagnostics& diagnostics, std::string warning, bool critical = false) {
    diagnostics.warnings.push_back(std::move(warning));
    ++diagnostics.warning_count;
    if (critical) {
        ++diagnostics.critical_count;
    }
}

} // namespace

DeepSystemsDiagnostics make_deep_systems_diagnostics(
    economy::MarketSnapshot market,
    economy::LedgerSummary ledger,
    ContractLifecycleSummary contracts,
    SettlementProductionSnapshot production,
    LogisticsNetworkSnapshot logistics
) {
    DeepSystemsDiagnostics diagnostics{
        .market = std::move(market),
        .ledger = std::move(ledger),
        .contracts = std::move(contracts),
        .production = std::move(production),
        .logistics = std::move(logistics),
    };

    if (diagnostics.market.depleted_count > 0) {
        add_warning(diagnostics, "market: depleted resources detected", true);
    }
    if (diagnostics.market.shortage_count > 0) {
        add_warning(diagnostics, "market: resource shortages detected");
    }
    if (diagnostics.contracts.overdue_open_count > 0) {
        add_warning(diagnostics, "contracts: overdue open contracts detected", true);
    }
    if (!diagnostics.contracts.blocked_by_reputation_contract_ids.empty()) {
        add_warning(diagnostics, "contracts: reputation-blocked contracts detected");
    }
    if (diagnostics.production.missing_input_count > 0) {
        add_warning(diagnostics, "production: missing input resources detected", true);
    }
    if (diagnostics.production.pressured_output_count > 0) {
        add_warning(diagnostics, "production: outputs match market pressure");
    }
    if (diagnostics.logistics.unknown_route_caravan_count > 0) {
        add_warning(diagnostics, "logistics: caravans with unknown routes detected", true);
    }
    if (diagnostics.logistics.cargo_supporting_shortage_count == 0 && diagnostics.market.shortage_count + diagnostics.market.depleted_count > 0) {
        add_warning(diagnostics, "logistics: no cargo currently supports market shortages");
    }
    if (diagnostics.ledger.entry_count == 0 && (diagnostics.market.total_supply > 0 || diagnostics.market.total_demand > 0)) {
        add_warning(diagnostics, "ledger: no economy ledger entries for active market state");
    }

    return diagnostics;
}

std::string deep_systems_diagnostics_digest(const DeepSystemsDiagnostics& diagnostics) {
    std::ostringstream out;
    out << "deep_systems"
        << ";warnings=" << diagnostics.warning_count
        << ";critical=" << diagnostics.critical_count
        << ";market_shortage=" << diagnostics.market.shortage_count
        << ";market_depleted=" << diagnostics.market.depleted_count
        << ";ledger_entries=" << diagnostics.ledger.entry_count
        << ";contracts_open=" << diagnostics.contracts.open_count
        << ";contracts_overdue=" << diagnostics.contracts.overdue_open_count
        << ";production_active=" << diagnostics.production.active_building_count
        << ";production_blocked=" << diagnostics.production.blocked_building_count
        << ";logistics_active=" << diagnostics.logistics.active_caravan_count
        << ";logistics_cargo=" << diagnostics.logistics.total_cargo;
    return out.str();
}

std::string deep_systems_diagnostics_markdown(const DeepSystemsDiagnostics& diagnostics) {
    std::ostringstream out;
    out << "# Deep systems diagnostics\n\n";
    out << "- Warnings: " << diagnostics.warning_count << "\n";
    out << "- Critical warnings: " << diagnostics.critical_count << "\n";
    out << "- Market shortages: " << diagnostics.market.shortage_count << "\n";
    out << "- Market depleted resources: " << diagnostics.market.depleted_count << "\n";
    out << "- Ledger entries: " << diagnostics.ledger.entry_count << "\n";
    out << "- Open contracts: " << diagnostics.contracts.open_count << "\n";
    out << "- Overdue open contracts: " << diagnostics.contracts.overdue_open_count << "\n";
    out << "- Active production buildings: " << diagnostics.production.active_building_count << "\n";
    out << "- Blocked production buildings: " << diagnostics.production.blocked_building_count << "\n";
    out << "- Active caravans: " << diagnostics.logistics.active_caravan_count << "\n";
    out << "- Total cargo: " << diagnostics.logistics.total_cargo << "\n";

    if (!diagnostics.warnings.empty()) {
        out << "\n## Warnings\n\n";
        for (const auto& warning : diagnostics.warnings) {
            out << "- " << warning << "\n";
        }
    }

    return out.str();
}

bool deep_systems_diagnostics_has_warnings(const DeepSystemsDiagnostics& diagnostics) noexcept {
    return diagnostics.warning_count > 0;
}

bool deep_systems_diagnostics_has_critical_warnings(const DeepSystemsDiagnostics& diagnostics) noexcept {
    return diagnostics.critical_count > 0;
}

const std::string* deep_systems_warning_by_prefix(
    const DeepSystemsDiagnostics& diagnostics,
    std::string_view prefix
) noexcept {
    for (const auto& warning : diagnostics.warnings) {
        if (warning.rfind(prefix, 0) == 0) {
            return &warning;
        }
    }
    return nullptr;
}

} // namespace clc::sim
