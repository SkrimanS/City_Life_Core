#include "clc/sim/Factions.hpp"
#include "clc/sim/Ownership.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    clc::sim::FactionCatalog factions;
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "riverwatch faction should add");
    require(clc::sim::add_faction(factions, clc::sim::FactionState{.id = "hillford", .display_name = "Hillford"}).ok(), "hillford faction should add");

    constexpr auto min_reputation = std::numeric_limits<std::int64_t>::min();
    constexpr auto max_reputation = std::numeric_limits<std::int64_t>::max();

    require(clc::sim::set_faction_reputation(factions, "riverwatch", "hillford", max_reputation).ok(), "max reputation should be accepted");
    require(factions.reputations.size() == 1, "initial reputation should create one relation");
    require(clc::sim::faction_reputation(factions, "riverwatch", "hillford") == max_reputation, "max reputation should be preserved");

    require(clc::sim::set_faction_reputation(factions, "riverwatch", "hillford", min_reputation).ok(), "min reputation should update existing relation");
    require(factions.reputations.size() == 1, "reputation update should not duplicate relation");
    require(clc::sim::faction_reputation(factions, "riverwatch", "hillford") == min_reputation, "min reputation should be preserved");

    const auto reputation_count_before_invalid = factions.reputations.size();
    require(!clc::sim::set_faction_reputation(factions, "riverwatch", "riverwatch", 100).ok(), "self reputation should be rejected");
    require(!clc::sim::set_faction_reputation(factions, "missing", "hillford", 100).ok(), "unknown source reputation should be rejected");
    require(!clc::sim::set_faction_reputation(factions, "riverwatch", "missing", 100).ok(), "unknown target reputation should be rejected");
    require(!clc::sim::set_faction_reputation(factions, "", "hillford", 100).ok(), "empty source reputation should be rejected");
    require(!clc::sim::set_faction_reputation(factions, "riverwatch", "", 100).ok(), "empty target reputation should be rejected");
    require(factions.reputations.size() == reputation_count_before_invalid, "invalid reputation writes should not mutate relation count");
    require(clc::sim::faction_reputation(factions, "riverwatch", "hillford") == min_reputation, "invalid reputation writes should not mutate existing value");

    clc::sim::OwnershipCatalog ownership;
    require(clc::sim::set_settlement_owner(ownership, "riverwatch", "riverwatch").ok(), "initial settlement owner should set");
    require(clc::sim::set_caravan_owner(ownership, "caravan_001", "riverwatch").ok(), "initial caravan owner should set");
    require(clc::sim::settlement_ownership_count(ownership) == 1, "initial settlement owner should create one entry");
    require(clc::sim::caravan_ownership_count(ownership) == 1, "initial caravan owner should create one entry");

    require(clc::sim::set_settlement_owner(ownership, "riverwatch", "hillford").ok(), "settlement owner should update existing entry");
    require(clc::sim::set_caravan_owner(ownership, "caravan_001", "hillford").ok(), "caravan owner should update existing entry");
    require(clc::sim::settlement_ownership_count(ownership) == 1, "settlement owner update should not duplicate entry");
    require(clc::sim::caravan_ownership_count(ownership) == 1, "caravan owner update should not duplicate entry");
    require(clc::sim::settlement_owner(ownership, "riverwatch") == std::string_view{"hillford"}, "settlement owner update should preserve replacement");
    require(clc::sim::caravan_owner(ownership, "caravan_001") == std::string_view{"hillford"}, "caravan owner update should preserve replacement");

    const auto settlement_count_before_invalid = clc::sim::settlement_ownership_count(ownership);
    const auto caravan_count_before_invalid = clc::sim::caravan_ownership_count(ownership);
    require(!clc::sim::set_settlement_owner(ownership, "", "riverwatch").ok(), "empty settlement id should be rejected");
    require(!clc::sim::set_settlement_owner(ownership, "riverwatch", "").ok(), "empty settlement faction id should be rejected");
    require(!clc::sim::set_caravan_owner(ownership, "", "riverwatch").ok(), "empty caravan id should be rejected");
    require(!clc::sim::set_caravan_owner(ownership, "caravan_001", "").ok(), "empty caravan faction id should be rejected");
    require(clc::sim::settlement_ownership_count(ownership) == settlement_count_before_invalid, "invalid settlement owner writes should not mutate count");
    require(clc::sim::caravan_ownership_count(ownership) == caravan_count_before_invalid, "invalid caravan owner writes should not mutate count");
    require(clc::sim::settlement_owner(ownership, "riverwatch") == std::string_view{"hillford"}, "invalid settlement owner writes should not mutate existing owner");
    require(clc::sim::caravan_owner(ownership, "caravan_001") == std::string_view{"hillford"}, "invalid caravan owner writes should not mutate existing owner");

    const auto empty_settlement_reference_report = clc::sim::validate_settlement_ownership_references(
        clc::sim::SettlementOwnership{},
        {},
        factions
    );
    require(!empty_settlement_reference_report.ok(), "empty settlement reference validation should fail before reference lookup");
    require(empty_settlement_reference_report.error_count() == 2, "empty settlement reference validation should only report structural errors");

    const auto empty_caravan_reference_report = clc::sim::validate_caravan_ownership_references(
        clc::sim::CaravanOwnership{},
        clc::sim::CaravanFleet{},
        factions
    );
    require(!empty_caravan_reference_report.ok(), "empty caravan reference validation should fail before reference lookup");
    require(empty_caravan_reference_report.error_count() == 2, "empty caravan reference validation should only report structural errors");

    return 0;
}
