#include "clc/sim/Factions.hpp"

#include <cstdlib>
#include <iostream>
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
    const clc::sim::FactionState empty_faction;
    const auto empty_faction_report = clc::sim::validate_faction(empty_faction);
    require(!empty_faction_report.ok(), "empty faction should fail validation");
    require(empty_faction_report.error_count() == 2, "empty faction should report id and display name errors");

    clc::sim::FactionCatalog catalog;
    require(clc::sim::faction_count(catalog) == 0, "empty faction catalog should count zero");
    require(!clc::sim::faction_exists(catalog, "traders_guild"), "empty faction catalog should not contain faction");
    require(clc::sim::faction_by_id(catalog, "traders_guild") == nullptr, "empty faction catalog lookup should miss");

    require(clc::sim::add_faction(catalog, clc::sim::FactionState{.id = "traders_guild", .display_name = "Traders Guild"}).ok(), "valid faction should add");
    require(clc::sim::add_faction(catalog, clc::sim::FactionState{.id = "riverwatch", .display_name = "Riverwatch"}).ok(), "second valid faction should add");
    require(clc::sim::add_faction(catalog, clc::sim::FactionState{.id = "hillford", .display_name = "Hillford"}).ok(), "third valid faction should add");
    require(clc::sim::faction_count(catalog) == 3, "catalog should count inserted factions");
    require(clc::sim::faction_exists(catalog, "traders_guild"), "catalog should contain inserted faction");

    const auto* traders = clc::sim::faction_by_id(catalog, "traders_guild");
    require(traders != nullptr, "catalog lookup should find inserted faction");
    require(traders->display_name == "Traders Guild", "catalog lookup should preserve display name");
    require(!clc::sim::add_faction(catalog, clc::sim::FactionState{.id = "traders_guild", .display_name = "Duplicate Traders Guild"}).ok(), "duplicate faction id should be rejected");
    require(clc::sim::faction_count(catalog) == 3, "duplicate faction should not change catalog size");
    require(!clc::sim::add_faction(catalog, empty_faction).ok(), "invalid faction should not add");
    require(clc::sim::faction_count(catalog) == 3, "invalid faction should not change catalog size");

    const clc::sim::FactionReputation empty_reputation;
    const auto empty_reputation_report = clc::sim::validate_faction_reputation(empty_reputation);
    require(!empty_reputation_report.ok(), "empty reputation should fail validation");
    require(empty_reputation_report.error_count() == 2, "empty reputation should report endpoint errors");

    require(!clc::sim::validate_faction_reputation(clc::sim::FactionReputation{.from_faction_id = "riverwatch", .to_faction_id = "riverwatch", .value = 10}).ok(), "self reputation should fail validation");
    require(!clc::sim::set_faction_reputation(catalog, "riverwatch", "riverwatch", 10).ok(), "setting self reputation should fail");
    require(!clc::sim::set_faction_reputation(catalog, "missing", "riverwatch", 10).ok(), "unknown source faction should fail");
    require(!clc::sim::set_faction_reputation(catalog, "riverwatch", "missing", 10).ok(), "unknown target faction should fail");
    require(clc::sim::faction_reputation(catalog, "riverwatch", "hillford") == 0, "missing reputation should default to zero");

    require(clc::sim::set_faction_reputation(catalog, "riverwatch", "hillford", 25).ok(), "valid reputation should set");
    require(clc::sim::set_faction_reputation(catalog, "hillford", "riverwatch", -10).ok(), "reverse reputation should set independently");
    require(clc::sim::set_faction_reputation(catalog, "traders_guild", "riverwatch", 40).ok(), "third reputation should set");
    require(clc::sim::faction_reputation(catalog, "riverwatch", "hillford") == 25, "reputation lookup should return set value");
    require(clc::sim::faction_reputation(catalog, "hillford", "riverwatch") == -10, "directed reputation should preserve reverse value");
    require(clc::sim::faction_reputation(catalog, "traders_guild", "riverwatch") == 40, "third reputation should be lookupable");

    require(clc::sim::set_faction_reputation(catalog, "riverwatch", "hillford", 30).ok(), "setting existing reputation should update");
    require(clc::sim::faction_reputation(catalog, "riverwatch", "hillford") == 30, "reputation update should replace value");
    require(catalog.reputations.size() == 3, "reputation update should not duplicate relation");

    const auto from_riverwatch = clc::sim::faction_reputations_from(catalog, "riverwatch");
    require(from_riverwatch.size() == 1, "from filter should return one riverwatch relation");
    require(from_riverwatch[0].to_faction_id == "hillford", "from filter should return expected target");
    require(from_riverwatch[0].value == 30, "from filter should return updated value");

    const auto to_riverwatch = clc::sim::faction_reputations_to(catalog, "riverwatch");
    require(to_riverwatch.size() == 2, "to filter should return two riverwatch relations");
    require(clc::sim::faction_reputations_from(catalog, "missing").empty(), "from filter should return empty for missing faction");
    require(clc::sim::faction_reputations_to(catalog, "missing").empty(), "to filter should return empty for missing faction");

    return 0;
}
