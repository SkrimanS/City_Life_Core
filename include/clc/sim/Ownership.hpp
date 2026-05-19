#pragma once

#include "clc/data/Validation.hpp"
#include "clc/sim/Caravans.hpp"
#include "clc/sim/Factions.hpp"
#include "clc/sim/Settlement.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct SettlementOwnership final {
    std::string settlement_id{};
    std::string faction_id{};
};

struct CaravanOwnership final {
    std::string caravan_id{};
    std::string faction_id{};
};

struct OwnershipCatalog final {
    std::vector<SettlementOwnership> settlements{};
    std::vector<CaravanOwnership> caravans{};
};

[[nodiscard]] data::ValidationReport validate_settlement_ownership(const SettlementOwnership& ownership);
[[nodiscard]] data::ValidationReport validate_caravan_ownership(const CaravanOwnership& ownership);

[[nodiscard]] data::ValidationReport validate_settlement_ownership_references(
    const SettlementOwnership& ownership,
    const std::vector<SettlementState>& settlements,
    const FactionCatalog& factions
);

[[nodiscard]] data::ValidationReport validate_caravan_ownership_references(
    const CaravanOwnership& ownership,
    const CaravanFleet& caravans,
    const FactionCatalog& factions
);

[[nodiscard]] data::ValidationReport set_settlement_owner(
    OwnershipCatalog& catalog,
    std::string settlement_id,
    std::string faction_id
);

[[nodiscard]] data::ValidationReport set_caravan_owner(
    OwnershipCatalog& catalog,
    std::string caravan_id,
    std::string faction_id
);

[[nodiscard]] std::string_view settlement_owner(const OwnershipCatalog& catalog, std::string_view settlement_id) noexcept;
[[nodiscard]] std::string_view caravan_owner(const OwnershipCatalog& catalog, std::string_view caravan_id) noexcept;
[[nodiscard]] std::uint64_t settlement_ownership_count(const OwnershipCatalog& catalog) noexcept;
[[nodiscard]] std::uint64_t caravan_ownership_count(const OwnershipCatalog& catalog) noexcept;

[[nodiscard]] std::vector<SettlementOwnership> settlements_owned_by(const OwnershipCatalog& catalog, std::string_view faction_id);
[[nodiscard]] std::vector<CaravanOwnership> caravans_owned_by(const OwnershipCatalog& catalog, std::string_view faction_id);

} // namespace clc::sim
