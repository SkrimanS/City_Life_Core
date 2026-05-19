#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

struct FactionState final {
    std::string id{};
    std::string display_name{};
};

struct FactionReputation final {
    std::string from_faction_id{};
    std::string to_faction_id{};
    std::int64_t value{0};
};

struct FactionCatalog final {
    std::vector<FactionState> factions{};
    std::vector<FactionReputation> reputations{};
};

[[nodiscard]] data::ValidationReport validate_faction(const FactionState& faction);
[[nodiscard]] data::ValidationReport validate_faction_reputation(const FactionReputation& reputation);

[[nodiscard]] data::ValidationReport add_faction(FactionCatalog& catalog, FactionState faction);
[[nodiscard]] std::uint64_t faction_count(const FactionCatalog& catalog) noexcept;
[[nodiscard]] const FactionState* faction_by_id(const FactionCatalog& catalog, std::string_view faction_id) noexcept;
[[nodiscard]] bool faction_exists(const FactionCatalog& catalog, std::string_view faction_id) noexcept;

[[nodiscard]] data::ValidationReport set_faction_reputation(
    FactionCatalog& catalog,
    std::string from_faction_id,
    std::string to_faction_id,
    std::int64_t value
);

[[nodiscard]] std::int64_t faction_reputation(
    const FactionCatalog& catalog,
    std::string_view from_faction_id,
    std::string_view to_faction_id
) noexcept;

[[nodiscard]] std::vector<FactionReputation> faction_reputations_from(
    const FactionCatalog& catalog,
    std::string_view faction_id
);

[[nodiscard]] std::vector<FactionReputation> faction_reputations_to(
    const FactionCatalog& catalog,
    std::string_view faction_id
);

} // namespace clc::sim
