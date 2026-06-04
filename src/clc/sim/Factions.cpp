#include "clc/sim/Factions.hpp"

#include <sstream>
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

std::string_view faction_access_level_name(FactionAccessLevel access) noexcept {
    switch (access) {
    case FactionAccessLevel::blocked:
        return "blocked";
    case FactionAccessLevel::restricted:
        return "restricted";
    case FactionAccessLevel::neutral:
        return "neutral";
    case FactionAccessLevel::trusted:
        return "trusted";
    case FactionAccessLevel::allied:
        return "allied";
    }
    return "unknown";
}

FactionAccessLevel classify_faction_access(std::int64_t reputation) noexcept {
    if (reputation <= -75) {
        return FactionAccessLevel::blocked;
    }
    if (reputation < 0) {
        return FactionAccessLevel::restricted;
    }
    if (reputation >= 75) {
        return FactionAccessLevel::allied;
    }
    if (reputation >= 25) {
        return FactionAccessLevel::trusted;
    }
    return FactionAccessLevel::neutral;
}

FactionAccessReport make_faction_access_report(
    const FactionCatalog& catalog,
    std::string_view from_faction_id,
    std::string_view to_faction_id
) {
    FactionAccessReport report{
        .from_faction_id = std::string{from_faction_id},
        .to_faction_id = std::string{to_faction_id},
    };

    if (from_faction_id.empty() || to_faction_id.empty()) {
        report.access = FactionAccessLevel::blocked;
        report.can_trade = false;
        report.can_issue_contract = false;
        report.can_receive_contract = false;
        report.reason = "faction ids must not be empty";
        return report;
    }

    if (!faction_exists(catalog, from_faction_id) || !faction_exists(catalog, to_faction_id)) {
        report.access = FactionAccessLevel::blocked;
        report.can_trade = false;
        report.can_issue_contract = false;
        report.can_receive_contract = false;
        report.reason = "unknown faction";
        return report;
    }

    if (from_faction_id == to_faction_id) {
        report.access = FactionAccessLevel::allied;
        report.reputation = 100;
        report.reason = "same faction";
        return report;
    }

    report.reputation = faction_reputation(catalog, from_faction_id, to_faction_id);
    report.access = classify_faction_access(report.reputation);

    switch (report.access) {
    case FactionAccessLevel::blocked:
        report.can_trade = false;
        report.can_issue_contract = false;
        report.can_receive_contract = false;
        report.reason = "reputation blocks interaction";
        break;
    case FactionAccessLevel::restricted:
        report.can_trade = true;
        report.can_issue_contract = false;
        report.can_receive_contract = true;
        report.reason = "restricted reputation allows limited trade only";
        break;
    case FactionAccessLevel::neutral:
        report.reason = "neutral access";
        break;
    case FactionAccessLevel::trusted:
        report.reason = "trusted access";
        break;
    case FactionAccessLevel::allied:
        report.reason = "allied access";
        break;
    }

    return report;
}

std::string faction_access_report_digest(const FactionAccessReport& report) {
    std::ostringstream out;
    out << "faction_access"
        << ";from=" << report.from_faction_id
        << ";to=" << report.to_faction_id
        << ";reputation=" << report.reputation
        << ";access=" << faction_access_level_name(report.access)
        << ";trade=" << (report.can_trade ? "yes" : "no")
        << ";issue_contract=" << (report.can_issue_contract ? "yes" : "no")
        << ";receive_contract=" << (report.can_receive_contract ? "yes" : "no");
    return out.str();
}

} // namespace clc::sim
