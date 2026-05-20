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

    add_mismatch(report,
        expected.caravans.caravan_count() == actual.caravans.caravan_count(),
        "runtime caravan count mismatch");

    if (!expected.caravans.caravans.empty() && !actual.caravans.caravans.empty()) {
        add_mismatch(report,
            expected.caravans.caravans[0].id == actual.caravans.caravans[0].id,
            "runtime caravan id mismatch");
    }

    add_mismatch(report,
        expected.factions.factions.size() == actual.factions.factions.size(),
        "runtime faction count mismatch");

    add_mismatch(report,
        expected.contracts.contracts.size() == actual.contracts.contracts.size(),
        "runtime contract count mismatch");

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
