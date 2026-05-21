#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"

#include <cstddef>

namespace clc::sim {

namespace {

void add_mismatch(data::ValidationReport& report, bool condition, const char* message) {
    if (!condition) {
        report.add_error(message);
    }
}

void add_storage_mismatch(
    data::ValidationReport& report,
    const ResourceStorage& expected,
    const ResourceStorage& actual,
    const char* count_message,
    const char* missing_resource_message,
    const char* quantity_message
) {
    const auto& expected_entries = expected.entries();
    const auto& actual_entries = actual.entries();

    add_mismatch(report,
        expected_entries.size() == actual_entries.size(),
        count_message);

    for (const auto& [resource_id, expected_quantity] : expected_entries) {
        const auto actual_entry = actual_entries.find(resource_id);
        add_mismatch(report,
            actual_entry != actual_entries.end(),
            missing_resource_message);

        if (actual_entry == actual_entries.end()) {
            continue;
        }

        add_mismatch(report,
            expected_quantity == actual_entry->second,
            quantity_message);
    }
}

} // namespace

data::ValidationReport validate_simulation_runtimes_match(
    const SimulationRuntime& expected,
    const SimulationRuntime& actual
) {
    data::ValidationReport report{};

    add_mismatch(report,
        expected.engine.current_day() == actual.engine.current_day(),
        "runtime current day mismatch");

    add_mismatch(report,
        expected.engine.settlements().size() == actual.engine.settlements().size(),
        "runtime engine settlement count mismatch");

    const auto engine_settlement_count = expected.engine.settlements().size() < actual.engine.settlements().size()
        ? expected.engine.settlements().size()
        : actual.engine.settlements().size();
    for (std::size_t index = 0; index < engine_settlement_count; ++index) {
        const auto& expected_settlement = expected.engine.settlements()[index];
        const auto& actual_settlement = actual.engine.settlements()[index];

        add_mismatch(report,
            expected_settlement.id == actual_settlement.id,
            "runtime engine settlement id mismatch");
        add_mismatch(report,
            expected_settlement.display_name == actual_settlement.display_name,
            "runtime engine settlement display name mismatch");
        add_mismatch(report,
            expected_settlement.population == actual_settlement.population,
            "runtime engine settlement population mismatch");
        add_storage_mismatch(report,
            expected_settlement.storage,
            actual_settlement.storage,
            "runtime engine settlement storage resource count mismatch",
            "runtime engine settlement storage resource id mismatch",
            "runtime engine settlement storage resource quantity mismatch");

        add_mismatch(report,
            expected_settlement.buildings.size() == actual_settlement.buildings.size(),
            "runtime engine settlement building count mismatch");

        const auto building_count = expected_settlement.buildings.size() < actual_settlement.buildings.size()
            ? expected_settlement.buildings.size()
            : actual_settlement.buildings.size();
        for (std::size_t building_index = 0; building_index < building_count; ++building_index) {
            add_mismatch(report,
                expected_settlement.buildings[building_index].definition_id == actual_settlement.buildings[building_index].definition_id,
                "runtime engine settlement building definition id mismatch");
            add_mismatch(report,
                expected_settlement.buildings[building_index].assigned_workers == actual_settlement.buildings[building_index].assigned_workers,
                "runtime engine settlement building assigned workers mismatch");
        }
    }

    add_mismatch(report,
        expected.engine.events().size() == actual.engine.events().size(),
        "runtime engine event count mismatch");

    const auto engine_event_count = expected.engine.events().size() < actual.engine.events().size()
        ? expected.engine.events().size()
        : actual.engine.events().size();
    for (std::size_t index = 0; index < engine_event_count; ++index) {
        add_mismatch(report,
            expected.engine.events()[index].day == actual.engine.events()[index].day,
            "runtime engine event day mismatch");
        add_mismatch(report,
            expected.engine.events()[index].type == actual.engine.events()[index].type,
            "runtime engine event type mismatch");
        add_mismatch(report,
            expected.engine.events()[index].message == actual.engine.events()[index].message,
            "runtime engine event message mismatch");
    }

    const auto& expected_market_demands = expected.engine.market().demands();
    const auto& actual_market_demands = actual.engine.market().demands();
    add_mismatch(report,
        expected_market_demands.size() == actual_market_demands.size(),
        "runtime engine market demand count mismatch");

    for (const auto& [resource_id, expected_demand] : expected_market_demands) {
        const auto actual_demand = actual_market_demands.find(resource_id);
        add_mismatch(report,
            actual_demand != actual_market_demands.end(),
            "runtime engine market demand resource id mismatch");

        if (actual_demand == actual_market_demands.end()) {
            continue;
        }

        add_mismatch(report,
            expected_demand == actual_demand->second,
            "runtime engine market demand value mismatch");
    }

    add_mismatch(report,
        expected.routes.routes.size() == actual.routes.routes.size(),
        "runtime route count mismatch");

    const auto route_count = expected.routes.routes.size() < actual.routes.routes.size()
        ? expected.routes.routes.size()
        : actual.routes.routes.size();
    for (std::size_t index = 0; index < route_count; ++index) {
        add_mismatch(report,
            expected.routes.routes[index].id == actual.routes.routes[index].id,
            "runtime route id mismatch");
        add_mismatch(report,
            expected.routes.routes[index].display_name == actual.routes.routes[index].display_name,
            "runtime route display name mismatch");
        add_mismatch(report,
            expected.routes.routes[index].origin_settlement_id == actual.routes.routes[index].origin_settlement_id,
            "runtime route origin settlement id mismatch");
        add_mismatch(report,
            expected.routes.routes[index].destination_settlement_id == actual.routes.routes[index].destination_settlement_id,
            "runtime route destination settlement id mismatch");
        add_mismatch(report,
            expected.routes.routes[index].travel_days == actual.routes.routes[index].travel_days,
            "runtime route travel days mismatch");
    }

    add_mismatch(report,
        expected.caravans.caravan_count() == actual.caravans.caravan_count(),
        "runtime caravan count mismatch");

    const auto caravan_count = expected.caravans.caravans.size() < actual.caravans.caravans.size()
        ? expected.caravans.caravans.size()
        : actual.caravans.caravans.size();
    for (std::size_t index = 0; index < caravan_count; ++index) {
        add_mismatch(report,
            expected.caravans.caravans[index].id == actual.caravans.caravans[index].id,
            "runtime caravan id mismatch");
        add_mismatch(report,
            expected.caravans.caravans[index].display_name == actual.caravans.caravans[index].display_name,
            "runtime caravan display name mismatch");
        add_mismatch(report,
            expected.caravans.caravans[index].route_id == actual.caravans.caravans[index].route_id,
            "runtime caravan route id mismatch");
        add_mismatch(report,
            expected.caravans.caravans[index].origin_settlement_id == actual.caravans.caravans[index].origin_settlement_id,
            "runtime caravan origin settlement id mismatch");
        add_mismatch(report,
            expected.caravans.caravans[index].destination_settlement_id == actual.caravans.caravans[index].destination_settlement_id,
            "runtime caravan destination settlement id mismatch");
        add_mismatch(report,
            expected.caravans.caravans[index].total_travel_days == actual.caravans.caravans[index].total_travel_days,
            "runtime caravan total travel days mismatch");
        add_mismatch(report,
            expected.caravans.caravans[index].days_remaining == actual.caravans.caravans[index].days_remaining,
            "runtime caravan days remaining mismatch");
        add_storage_mismatch(report,
            expected.caravans.caravans[index].cargo,
            actual.caravans.caravans[index].cargo,
            "runtime caravan cargo resource count mismatch",
            "runtime caravan cargo resource id mismatch",
            "runtime caravan cargo resource quantity mismatch");
    }

    add_mismatch(report,
        expected.factions.factions.size() == actual.factions.factions.size(),
        "runtime faction count mismatch");

    const auto faction_count = expected.factions.factions.size() < actual.factions.factions.size()
        ? expected.factions.factions.size()
        : actual.factions.factions.size();
    for (std::size_t index = 0; index < faction_count; ++index) {
        add_mismatch(report,
            expected.factions.factions[index].id == actual.factions.factions[index].id,
            "runtime faction id mismatch");
        add_mismatch(report,
            expected.factions.factions[index].display_name == actual.factions.factions[index].display_name,
            "runtime faction display name mismatch");
    }

    add_mismatch(report,
        expected.factions.reputations.size() == actual.factions.reputations.size(),
        "runtime faction reputation count mismatch");

    const auto faction_reputation_count = expected.factions.reputations.size() < actual.factions.reputations.size()
        ? expected.factions.reputations.size()
        : actual.factions.reputations.size();
    for (std::size_t index = 0; index < faction_reputation_count; ++index) {
        add_mismatch(report,
            expected.factions.reputations[index].from_faction_id == actual.factions.reputations[index].from_faction_id,
            "runtime faction reputation from faction id mismatch");
        add_mismatch(report,
            expected.factions.reputations[index].to_faction_id == actual.factions.reputations[index].to_faction_id,
            "runtime faction reputation to faction id mismatch");
        add_mismatch(report,
            expected.factions.reputations[index].value == actual.factions.reputations[index].value,
            "runtime faction reputation value mismatch");
    }

    add_mismatch(report,
        expected.ownership.settlements.size() == actual.ownership.settlements.size(),
        "runtime settlement ownership count mismatch");

    const auto settlement_ownership_count = expected.ownership.settlements.size() < actual.ownership.settlements.size()
        ? expected.ownership.settlements.size()
        : actual.ownership.settlements.size();
    for (std::size_t index = 0; index < settlement_ownership_count; ++index) {
        add_mismatch(report,
            expected.ownership.settlements[index].settlement_id == actual.ownership.settlements[index].settlement_id,
            "runtime settlement ownership settlement id mismatch");
        add_mismatch(report,
            expected.ownership.settlements[index].faction_id == actual.ownership.settlements[index].faction_id,
            "runtime settlement ownership faction id mismatch");
    }

    add_mismatch(report,
        expected.ownership.caravans.size() == actual.ownership.caravans.size(),
        "runtime caravan ownership count mismatch");

    const auto caravan_ownership_count = expected.ownership.caravans.size() < actual.ownership.caravans.size()
        ? expected.ownership.caravans.size()
        : actual.ownership.caravans.size();
    for (std::size_t index = 0; index < caravan_ownership_count; ++index) {
        add_mismatch(report,
            expected.ownership.caravans[index].caravan_id == actual.ownership.caravans[index].caravan_id,
            "runtime caravan ownership caravan id mismatch");
        add_mismatch(report,
            expected.ownership.caravans[index].faction_id == actual.ownership.caravans[index].faction_id,
            "runtime caravan ownership faction id mismatch");
    }

    add_mismatch(report,
        expected.contracts.contracts.size() == actual.contracts.contracts.size(),
        "runtime contract count mismatch");

    const auto contract_count = expected.contracts.contracts.size() < actual.contracts.contracts.size()
        ? expected.contracts.contracts.size()
        : actual.contracts.contracts.size();
    for (std::size_t index = 0; index < contract_count; ++index) {
        add_mismatch(report,
            expected.contracts.contracts[index].id == actual.contracts.contracts[index].id,
            "runtime contract id mismatch");
        add_mismatch(report,
            expected.contracts.contracts[index].display_name == actual.contracts.contracts[index].display_name,
            "runtime contract display name mismatch");
        add_mismatch(report,
            expected.contracts.contracts[index].issuer_faction_id == actual.contracts.contracts[index].issuer_faction_id,
            "runtime contract issuer faction id mismatch");
        add_mismatch(report,
            expected.contracts.contracts[index].receiver_faction_id == actual.contracts.contracts[index].receiver_faction_id,
            "runtime contract receiver faction id mismatch");
        add_mismatch(report,
            expected.contracts.contracts[index].resource_id == actual.contracts.contracts[index].resource_id,
            "runtime contract resource id mismatch");
        add_mismatch(report,
            expected.contracts.contracts[index].quantity == actual.contracts.contracts[index].quantity,
            "runtime contract quantity mismatch");
        add_mismatch(report,
            expected.contracts.contracts[index].reward_coins == actual.contracts.contracts[index].reward_coins,
            "runtime contract reward coins mismatch");
        add_mismatch(report,
            expected.contracts.contracts[index].due_day == actual.contracts.contracts[index].due_day,
            "runtime contract due day mismatch");
        add_mismatch(report,
            expected.contracts.contracts[index].status == actual.contracts.contracts[index].status,
            "runtime contract status mismatch");
    }

    add_mismatch(report,
        expected.wallet.coins == actual.wallet.coins,
        "runtime wallet mismatch");

    add_mismatch(report,
        expected.ledger.next_sequence() == actual.ledger.next_sequence(),
        "runtime ledger next sequence mismatch");

    const auto& expected_ledger_entries = expected.ledger.entries();
    const auto& actual_ledger_entries = actual.ledger.entries();
    add_mismatch(report,
        expected_ledger_entries.size() == actual_ledger_entries.size(),
        "runtime ledger size mismatch");

    const auto ledger_count = expected_ledger_entries.size() < actual_ledger_entries.size()
        ? expected_ledger_entries.size()
        : actual_ledger_entries.size();
    for (std::size_t index = 0; index < ledger_count; ++index) {
        add_mismatch(report,
            expected_ledger_entries[index].sequence == actual_ledger_entries[index].sequence,
            "runtime ledger sequence mismatch");
        add_mismatch(report,
            expected_ledger_entries[index].type == actual_ledger_entries[index].type,
            "runtime ledger type mismatch");
        add_mismatch(report,
            expected_ledger_entries[index].resource_id == actual_ledger_entries[index].resource_id,
            "runtime ledger resource id mismatch");
        add_mismatch(report,
            expected_ledger_entries[index].quantity == actual_ledger_entries[index].quantity,
            "runtime ledger quantity mismatch");
        add_mismatch(report,
            expected_ledger_entries[index].unit_price == actual_ledger_entries[index].unit_price,
            "runtime ledger unit price mismatch");
        add_mismatch(report,
            expected_ledger_entries[index].total_price == actual_ledger_entries[index].total_price,
            "runtime ledger total price mismatch");
        add_mismatch(report,
            expected_ledger_entries[index].reference_id == actual_ledger_entries[index].reference_id,
            "runtime ledger reference id mismatch");
        add_mismatch(report,
            expected_ledger_entries[index].note == actual_ledger_entries[index].note,
            "runtime ledger note mismatch");
    }

    return report;
}

SimulationRuntimeSaveLoadValidationResult validate_simulation_runtime_save_load_roundtrip(
    const SimulationRuntime& source,
    SimulationRuntime& target,
    const std::filesystem::path& path
) {
    SimulationRuntimeSaveLoadValidationResult result{};

    auto save_report = save_simulation_runtime_to_file(source, path);
    if (!save_report.ok()) {
        result.validation = save_report;
        return result;
    }

    result.load = load_simulation_runtime_from_file(path, target);
    if (!result.load.ok()) {
        return result;
    }

    result.validation = validate_simulation_world_state(capture_simulation_world_state(target));

    if (!result.validation.ok()) {
        return result;
    }

    result.validation = validate_simulation_runtimes_match(source, target);
    return result;
}

} // namespace clc::sim
