#include "clc/sim/SimulationRuntimePersistenceValidation.hpp"

namespace clc::sim {

namespace {

void add_mismatch(data::ValidationReport& report, bool condition, const char* message) {
    if (!condition) {
        report.add_error(message);
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
        expected.routes.routes.size() == actual.routes.routes.size(),
        "runtime route count mismatch");

    const auto route_count = expected.routes.routes.size() < actual.routes.routes.size()
        ? expected.routes.routes.size()
        : actual.routes.routes.size();
    for (std::size_t index = 0; index < route_count; ++index) {
        add_mismatch(report,
            expected.routes.routes[index].id == actual.routes.routes[index].id,
            "runtime route id mismatch");
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
    }

    add_mismatch(report,
        expected.wallet.coins == actual.wallet.coins,
        "runtime wallet mismatch");

    add_mismatch(report,
        expected.ledger.entries().size() == actual.ledger.entries().size(),
        "runtime ledger size mismatch");

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
