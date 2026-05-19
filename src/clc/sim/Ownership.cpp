#include "clc/sim/Ownership.hpp"

#include <utility>

namespace clc::sim {
namespace {

bool settlement_exists(const std::vector<SettlementState>& settlements, std::string_view settlement_id) {
    for (const auto& settlement : settlements) {
        if (settlement.id == settlement_id) {
            return true;
        }
    }
    return false;
}

} // namespace

data::ValidationReport validate_settlement_ownership(const SettlementOwnership& ownership) {
    data::ValidationReport report;
    if (ownership.settlement_id.empty()) {
        report.add_error("simulation.ownership.settlement", "settlement_id must not be empty");
    }
    if (ownership.faction_id.empty()) {
        report.add_error("simulation.ownership.settlement." + ownership.settlement_id, "faction_id must not be empty");
    }
    return report;
}

data::ValidationReport validate_caravan_ownership(const CaravanOwnership& ownership) {
    data::ValidationReport report;
    if (ownership.caravan_id.empty()) {
        report.add_error("simulation.ownership.caravan", "caravan_id must not be empty");
    }
    if (ownership.faction_id.empty()) {
        report.add_error("simulation.ownership.caravan." + ownership.caravan_id, "faction_id must not be empty");
    }
    return report;
}

data::ValidationReport validate_settlement_ownership_references(
    const SettlementOwnership& ownership,
    const std::vector<SettlementState>& settlements,
    const FactionCatalog& factions
) {
    auto report = validate_settlement_ownership(ownership);
    if (!report.ok()) {
        return report;
    }
    if (!settlement_exists(settlements, ownership.settlement_id)) {
        report.add_error("simulation.ownership.settlement." + ownership.settlement_id, "unknown settlement");
    }
    if (!faction_exists(factions, ownership.faction_id)) {
        report.add_error("simulation.ownership.faction." + ownership.faction_id, "unknown faction");
    }
    return report;
}

data::ValidationReport validate_caravan_ownership_references(
    const CaravanOwnership& ownership,
    const CaravanFleet& caravans,
    const FactionCatalog& factions
) {
    auto report = validate_caravan_ownership(ownership);
    if (!report.ok()) {
        return report;
    }
    if (caravan_by_id(caravans, ownership.caravan_id) == nullptr) {
        report.add_error("simulation.ownership.caravan." + ownership.caravan_id, "unknown caravan");
    }
    if (!faction_exists(factions, ownership.faction_id)) {
        report.add_error("simulation.ownership.faction." + ownership.faction_id, "unknown faction");
    }
    return report;
}

data::ValidationReport set_settlement_owner(
    OwnershipCatalog& catalog,
    std::string settlement_id,
    std::string faction_id
) {
    SettlementOwnership ownership{.settlement_id = std::move(settlement_id), .faction_id = std::move(faction_id)};
    auto report = validate_settlement_ownership(ownership);
    if (!report.ok()) {
        return report;
    }

    for (auto& existing : catalog.settlements) {
        if (existing.settlement_id == ownership.settlement_id) {
            existing.faction_id = std::move(ownership.faction_id);
            return report;
        }
    }

    catalog.settlements.push_back(std::move(ownership));
    return report;
}

data::ValidationReport set_caravan_owner(
    OwnershipCatalog& catalog,
    std::string caravan_id,
    std::string faction_id
) {
    CaravanOwnership ownership{.caravan_id = std::move(caravan_id), .faction_id = std::move(faction_id)};
    auto report = validate_caravan_ownership(ownership);
    if (!report.ok()) {
        return report;
    }

    for (auto& existing : catalog.caravans) {
        if (existing.caravan_id == ownership.caravan_id) {
            existing.faction_id = std::move(ownership.faction_id);
            return report;
        }
    }

    catalog.caravans.push_back(std::move(ownership));
    return report;
}

std::string_view settlement_owner(const OwnershipCatalog& catalog, std::string_view settlement_id) noexcept {
    for (const auto& ownership : catalog.settlements) {
        if (ownership.settlement_id == settlement_id) {
            return ownership.faction_id;
        }
    }
    return {};
}

std::string_view caravan_owner(const OwnershipCatalog& catalog, std::string_view caravan_id) noexcept {
    for (const auto& ownership : catalog.caravans) {
        if (ownership.caravan_id == caravan_id) {
            return ownership.faction_id;
        }
    }
    return {};
}

std::uint64_t settlement_ownership_count(const OwnershipCatalog& catalog) noexcept {
    return catalog.settlements.size();
}

std::uint64_t caravan_ownership_count(const OwnershipCatalog& catalog) noexcept {
    return catalog.caravans.size();
}

std::vector<SettlementOwnership> settlements_owned_by(const OwnershipCatalog& catalog, std::string_view faction_id) {
    std::vector<SettlementOwnership> ownerships;
    for (const auto& ownership : catalog.settlements) {
        if (ownership.faction_id == faction_id) {
            ownerships.push_back(ownership);
        }
    }
    return ownerships;
}

std::vector<CaravanOwnership> caravans_owned_by(const OwnershipCatalog& catalog, std::string_view faction_id) {
    std::vector<CaravanOwnership> ownerships;
    for (const auto& ownership : catalog.caravans) {
        if (ownership.faction_id == faction_id) {
            ownerships.push_back(ownership);
        }
    }
    return ownerships;
}

} // namespace clc::sim
