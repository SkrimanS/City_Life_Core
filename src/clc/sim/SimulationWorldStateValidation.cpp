#include "clc/sim/SimulationPersistence.hpp"

#include <string>
#include <unordered_set>

namespace clc::sim {
namespace {

void append_report(data::ValidationReport& target, const data::ValidationReport& source) {
    for (const auto& message : source.messages()) {
        if (message.severity == data::ValidationSeverity::error) {
            target.add_error(message.path, message.message);
        } else {
            target.add_warning(message.path, message.message);
        }
    }
}

bool settlement_exists(const std::vector<SettlementState>& settlements, std::string_view settlement_id) {
    for (const auto& settlement : settlements) {
        if (settlement.id == settlement_id) return true;
    }
    return false;
}

bool reputation_endpoint_exists(const FactionCatalog& factions, const FactionReputation& reputation) {
    return faction_exists(factions, reputation.from_faction_id) && faction_exists(factions, reputation.to_faction_id);
}

void validate_unique_id(data::ValidationReport& report, std::unordered_set<std::string>& ids, std::string id, std::string path, std::string message) {
    if (id.empty()) return;
    if (!ids.insert(std::move(id)).second) {
        report.add_error(std::move(path), std::move(message));
    }
}

} // namespace

data::ValidationReport validate_simulation_world_state(const SimulationWorldState& state) {
    data::ValidationReport report;

    std::unordered_set<std::string> settlement_ids;
    for (const auto& settlement : state.engine.settlements) {
        validate_unique_id(report, settlement_ids, settlement.id, "simulation.world_state.settlement." + settlement.id, "duplicate settlement id");
    }

    std::unordered_set<std::string> route_ids;
    for (const auto& route : state.routes.routes) {
        append_report(report, validate_settlement_route_for_settlements(route, state.engine.settlements));
        validate_unique_id(report, route_ids, route.id, "simulation.world_state.route." + route.id, "duplicate route id");
    }

    std::unordered_set<std::string> caravan_ids;
    for (const auto& caravan : state.caravans.caravans) {
        validate_unique_id(report, caravan_ids, caravan.id, "simulation.world_state.caravan." + caravan.id, "duplicate caravan id");
        const auto* route = settlement_route_by_id(state.routes, caravan.route_id);
        if (route == nullptr) {
            append_report(report, validate_caravan(caravan));
            report.add_error("simulation.world_state.caravan." + caravan.id + ".route", "unknown caravan route");
        } else {
            append_report(report, validate_caravan_for_route(caravan, *route));
        }
    }

    std::unordered_set<std::string> faction_ids;
    for (const auto& faction : state.factions.factions) {
        append_report(report, validate_faction(faction));
        validate_unique_id(report, faction_ids, faction.id, "simulation.world_state.faction." + faction.id, "duplicate faction id");
    }
    for (const auto& reputation : state.factions.reputations) {
        append_report(report, validate_faction_reputation(reputation));
        if (!reputation_endpoint_exists(state.factions, reputation)) {
            report.add_error("simulation.world_state.reputation." + reputation.from_faction_id + "." + reputation.to_faction_id, "unknown reputation faction endpoint");
        }
    }

    std::unordered_set<std::string> settlement_owners;
    for (const auto& ownership : state.ownership.settlements) {
        append_report(report, validate_settlement_ownership_references(ownership, state.engine.settlements, state.factions));
        validate_unique_id(report, settlement_owners, ownership.settlement_id, "simulation.world_state.ownership.settlement." + ownership.settlement_id, "duplicate settlement ownership");
    }

    std::unordered_set<std::string> caravan_owners;
    for (const auto& ownership : state.ownership.caravans) {
        append_report(report, validate_caravan_ownership_references(ownership, state.caravans, state.factions));
        validate_unique_id(report, caravan_owners, ownership.caravan_id, "simulation.world_state.ownership.caravan." + ownership.caravan_id, "duplicate caravan ownership");
    }

    std::unordered_set<std::string> contract_ids;
    for (const auto& contract : state.contracts.contracts) {
        append_report(report, validate_resource_delivery_contract_for_factions(contract, state.factions));
        validate_unique_id(report, contract_ids, contract.id, "simulation.world_state.contract." + contract.id, "duplicate contract id");
    }

    economy::EconomyLedger ledger;
    append_report(report, ledger.restore_entries(state.ledger_entries));

    for (const auto& demand : state.engine.market_demands) {
        if (demand.resource_id.empty()) {
            report.add_error("simulation.world_state.market", "market demand resource_id must not be empty");
        }
    }
    for (const auto& event : state.engine.events) {
        if (event.type.empty()) {
            report.add_error("simulation.world_state.event", "event type must not be empty");
        }
    }

    if (!state.routes.routes.empty() && state.engine.settlements.empty()) {
        report.add_error("simulation.world_state.routes", "routes require settlements");
    }
    if (!state.caravans.caravans.empty() && state.routes.routes.empty()) {
        report.add_error("simulation.world_state.caravans", "caravans require routes");
    }
    if (!state.ownership.settlements.empty() && state.factions.factions.empty()) {
        report.add_error("simulation.world_state.ownership", "ownership requires factions");
    }
    if (!state.contracts.contracts.empty() && state.factions.factions.empty()) {
        report.add_error("simulation.world_state.contracts", "contracts require factions");
    }

    (void)settlement_exists;
    return report;
}

} // namespace clc::sim
