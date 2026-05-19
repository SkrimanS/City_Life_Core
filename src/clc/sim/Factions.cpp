#include "clc/sim/Factions.hpp"

#include <utility>

namespace clc::sim {

data::ValidationReport validate_faction(const FactionState& faction) {
    data::ValidationReport report;
    if (faction.id.empty()) {
        report.add_error("simulation.faction", "faction id must not be empty");
    }
    if (faction.display_name.empty()) {
        report.add_error("simulation.faction." + faction.id, "faction display_name must not be empty");
    }
    return report;
}

data::ValidationReport validate_faction_reputation(const FactionReputation& reputation) {
    data::ValidationReport report;
    if (reputation.from_faction_id.empty()) {
        report.add_error("simulation.faction.reputation", "from_faction_id must not be empty");
    }
    if (reputation.to_faction_id.empty()) {
        report.add_error("simulation.faction.reputation", "to_faction_id must not be empty");
    }
    if (!reputation.from_faction_id.empty()
        && !reputation.to_faction_id.empty()
        && reputation.from_faction_id == reputation.to_faction_id) {
        report.add_error("simulation.faction.reputation." + reputation.from_faction_id, "faction reputation endpoints must be different");
    }
    return report;
}

data::ValidationReport add_faction(FactionCatalog& catalog, FactionState faction) {
    auto report = validate_faction(faction);
    if (!report.ok()) {
        return report;
    }

    if (faction_exists(catalog, faction.id)) {
        report.add_error("simulation.faction." + faction.id, "duplicate faction id");
        return report;
    }

    catalog.factions.push_back(std::move(faction));
    return report;
}

std::uint64_t faction_count(const FactionCatalog& catalog) noexcept {
    return catalog.factions.size();
}

const FactionState* faction_by_id(const FactionCatalog& catalog, std::string_view faction_id) noexcept {
    for (const auto& faction : catalog.factions) {
        if (faction.id == faction_id) {
            return &faction;
        }
    }
    return nullptr;
}

bool faction_exists(const FactionCatalog& catalog, std::string_view faction_id) noexcept {
    return faction_by_id(catalog, faction_id) != nullptr;
}

data::ValidationReport set_faction_reputation(
    FactionCatalog& catalog,
    std::string from_faction_id,
    std::string to_faction_id,
    std::int64_t value
) {
    FactionReputation reputation{
        .from_faction_id = std::move(from_faction_id),
        .to_faction_id = std::move(to_faction_id),
        .value = value,
    };

    auto report = validate_faction_reputation(reputation);
    if (!report.ok()) {
        return report;
    }

    if (!faction_exists(catalog, reputation.from_faction_id)) {
        report.add_error("simulation.faction." + reputation.from_faction_id, "unknown source faction");
    }
    if (!faction_exists(catalog, reputation.to_faction_id)) {
        report.add_error("simulation.faction." + reputation.to_faction_id, "unknown target faction");
    }
    if (!report.ok()) {
        return report;
    }

    for (auto& existing : catalog.reputations) {
        if (existing.from_faction_id == reputation.from_faction_id
            && existing.to_faction_id == reputation.to_faction_id) {
            existing.value = reputation.value;
            return report;
        }
    }

    catalog.reputations.push_back(std::move(reputation));
    return report;
}

std::int64_t faction_reputation(
    const FactionCatalog& catalog,
    std::string_view from_faction_id,
    std::string_view to_faction_id
) noexcept {
    for (const auto& reputation : catalog.reputations) {
        if (reputation.from_faction_id == from_faction_id && reputation.to_faction_id == to_faction_id) {
            return reputation.value;
        }
    }
    return 0;
}

std::vector<FactionReputation> faction_reputations_from(
    const FactionCatalog& catalog,
    std::string_view faction_id
) {
    std::vector<FactionReputation> reputations;
    for (const auto& reputation : catalog.reputations) {
        if (reputation.from_faction_id == faction_id) {
            reputations.push_back(reputation);
        }
    }
    return reputations;
}

std::vector<FactionReputation> faction_reputations_to(
    const FactionCatalog& catalog,
    std::string_view faction_id
) {
    std::vector<FactionReputation> reputations;
    for (const auto& reputation : catalog.reputations) {
        if (reputation.to_faction_id == faction_id) {
            reputations.push_back(reputation);
        }
    }
    return reputations;
}

} // namespace clc::sim
