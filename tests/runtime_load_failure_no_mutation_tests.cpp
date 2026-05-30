#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

void require_runtime_preserved(const clc::sim::SimulationRuntime& runtime) {
    require(runtime.engine.has_settlement("riverwatch"), "failed load should preserve riverwatch settlement");
    require(runtime.engine.has_settlement("hillford"), "failed load should preserve hillford settlement");
    require(runtime.engine.current_day() == 0, "failed load should preserve current day");
    require(runtime.engine.settlement_resource_amount("riverwatch", "grain") > 0, "failed load should preserve riverwatch grain");
    require(runtime.wallet.coins == 100, "failed load should preserve wallet");
    require(runtime.routes.routes.size() == 1, "failed load should preserve routes");
    require(runtime.caravans.caravans.empty(), "failed load should preserve caravan fleet");
}

} // namespace

int main() {
    auto bootstrap = clc::sim::make_basic_runtime_scenario();
    require(bootstrap.ok(), "scenario bootstrap failed");

    auto& runtime = bootstrap.runtime;
    runtime.wallet.coins = 100;
    require_runtime_preserved(runtime);

    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_load_failure_no_mutation_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    const auto malformed_path = directory / "malformed.clcs";
    {
        std::ofstream malformed_file{malformed_path};
        malformed_file << "not a city life runtime save\n";
    }

    const auto malformed = clc::sim::load_simulation_runtime_from_file(malformed_path, runtime);
    require(!malformed.ok(), "malformed runtime load should fail");
    require(!malformed.validation.ok(), "malformed runtime load should return diagnostics");
    require_runtime_preserved(runtime);

    const auto incomplete_path = directory / "incomplete.clcs";
    {
        std::ofstream incomplete_file{incomplete_path};
        incomplete_file << "CLC_SIM_WORLD_STATE\t1\n";
        incomplete_file << "time\t999999\n";
        incomplete_file << "wallet\t999999\n";
    }

    const auto incomplete = clc::sim::load_simulation_runtime_from_file(incomplete_path, runtime);
    require(!incomplete.ok(), "incomplete runtime load should fail");
    require(!incomplete.validation.ok(), "incomplete runtime load should return diagnostics");
    require_runtime_preserved(runtime);

    std::filesystem::remove_all(directory);
    return 0;
}
