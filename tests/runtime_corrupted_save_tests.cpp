#include "clc/sim/SimulationPersistence.hpp"
#include "clc/sim/SimulationRuntimeScenario.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void write_text_file(const std::filesystem::path& path, std::string_view content) {
    std::ofstream file{path};
    require(file.good(), "failed to open runtime test file for writing");
    file << std::string{content};
    require(file.good(), "failed to write runtime test file");
}

void expect_runtime_load_fails(const std::filesystem::path& path, std::string_view message) {
    clc::sim::SimulationRuntime target{clc::sim::make_basic_runtime_scenario_registry()};
    const auto load = clc::sim::load_simulation_runtime_from_file(path, target);
    require(!load.ok(), message);
}

} // namespace

int main() {
    const auto directory = std::filesystem::temp_directory_path() / "clc_runtime_corrupted_save_tests";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    const auto unknown_row_path = directory / "unknown_row_runtime.clcs";
    write_text_file(unknown_row_path, "CLC_SIM_WORLD_STATE\t1\nunknown_row\tvalue\n");
    expect_runtime_load_fails(unknown_row_path, "unknown world-state row unexpectedly loaded");

    const auto invalid_contract_status_path = directory / "invalid_contract_status_runtime.clcs";
    write_text_file(
        invalid_contract_status_path,
        "CLC_SIM_WORLD_STATE\t1\ncontract\tcontract\tContract\torigin\treceiver\tgrain\t1\t2\t3\tbogus\n"
    );
    expect_runtime_load_fails(invalid_contract_status_path, "invalid contract status runtime unexpectedly loaded");

    const auto orphan_storage_path = directory / "orphan_storage_runtime.clcs";
    write_text_file(orphan_storage_path, "CLC_SIM_WORLD_STATE\t1\nsettlement_storage\tmissing_settlement\tgrain\t1\n");
    expect_runtime_load_fails(orphan_storage_path, "orphan settlement storage runtime unexpectedly loaded");

    const auto orphan_cargo_path = directory / "orphan_cargo_runtime.clcs";
    write_text_file(orphan_cargo_path, "CLC_SIM_WORLD_STATE\t1\ncaravan_cargo\tmissing_caravan\tgrain\t1\n");
    expect_runtime_load_fails(orphan_cargo_path, "orphan caravan cargo runtime unexpectedly loaded");

    const auto bad_escape_path = directory / "bad_escape_runtime.clcs";
    write_text_file(bad_escape_path, "CLC_SIM_WORLD_STATE\t1\nsettlement\triver%ZZwatch\tRiverwatch\t120\n");
    expect_runtime_load_fails(bad_escape_path, "invalid escape sequence runtime unexpectedly loaded");

    std::filesystem::remove_all(directory);
    return 0;
}
