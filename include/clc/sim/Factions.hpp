#pragma once

#include "clc/data/Validation.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace clc::sim {

enum class FactionAccessLevel {
    blocked,
    restricted,
    neutral,
    trusted,
    allied,
};

struct FactionState final {
    std::string id{};
    std::string display_name{};
};

struct FactionReputation final {
    std::string from_faction_id{};
    std::string to_faction_id{};
    std::int64_t value{0};
};

struct FactionAccessReport final {
    std::string from_faction_id{};
    std::string to_faction_id{};
    std::int64_t reputation{0};
    FactionAccessLevel access{FactionAccessLevel::neutral};
    bool can_trade{true};
    bool can_issue_contract{true};
    bool can_receive_contract{true};
    std::string reason{};
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

[[nodiscard]] std::string_view faction_access_level_name(FactionAccessLevel access) noexcept;
[[nodiscard]] FactionAccessLevel classify_faction_access(std::int64_t reputation) noexcept;
[[nodiscard]] FactionAccessReport make_faction_access_report(
    const FactionCatalog& catalog,
    std::string_view from_faction_id,
    std::string_view to_faction_id
);
[[nodiscard]] std::string faction_access_report_digest(const FactionAccessReport& report);

} // namespace clc::sim
