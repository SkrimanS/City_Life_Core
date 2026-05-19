cmake_minimum_required(VERSION 3.22)

project(CityLifeCore
    VERSION 0.9.0
    DESCRIPTION "Universal headless simulation core for living settlements"
    LANGUAGES CXX
)

option(CLC_BUILD_TESTS "Build City Life Core tests" ON)
option(CLC_BUILD_EXAMPLES "Build City Life Core examples" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(city_life_core
    src/clc/core/Version.cpp
    src/clc/core/World.cpp
    src/clc/core/Time.cpp
    src/clc/core/EventLog.cpp
    src/clc/data/Validation.cpp
    src/clc/data/DataRegistry.cpp
    src/clc/data/DataPackLoader.cpp
    src/clc/sim/Storage.cpp
    src/clc/sim/Settlement.cpp
    src/clc/sim/SimulationEngine.cpp
    src/clc/sim/SimulationPersistence.cpp
    src/clc/sim/SimulationWorldStatePersistence.cpp
    src/clc/sim/SimulationWorldStateLedgerRestore.cpp
    src/clc/sim/SimulationWorldStateRuntimeBridge.cpp
    src/clc/sim/SimulationWorldStateValidation.cpp
    src/clc/sim/SimulationRuntimePersistence.cpp
    src/clc/sim/Routes.cpp
    src/clc/sim/Caravans.cpp
    src/clc/sim/Factions.cpp
    src/clc/sim/Contracts.cpp
    src/clc/sim/ContractRewards.cpp
    src/clc/sim/Ownership.cpp
    src/clc/economy/Market.cpp
    src/clc/economy/Trade.cpp
    src/clc/economy/Ledger.cpp
    src/clc/economy/Orders.cpp
)

add_library(CityLifeCore::core ALIAS city_life_core)

target_include_directories(city_life_core PUBLIC include)

target_compile_features(city_life_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(city_life_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(city_life_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(clc_runner apps/clc_runner/main.cpp)
target_link_libraries(clc_runner PRIVATE CityLifeCore::core)

if(CLC_BUILD_TESTS)
    enable_testing()

    add_executable(clc_smoke_tests tests/smoke_tests.cpp)
    target_link_libraries(clc_smoke_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_smoke_tests COMMAND clc_smoke_tests)

    add_executable(clc_economy_tests tests/economy_tests.cpp)
    target_link_libraries(clc_economy_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_economy_tests COMMAND clc_economy_tests)

    add_executable(clc_simulation_tests tests/simulation_tests.cpp)
    target_link_libraries(clc_simulation_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_simulation_tests COMMAND clc_simulation_tests)

    add_executable(clc_scenario_digest_tests tests/scenario_digest_tests.cpp)
    target_link_libraries(clc_scenario_digest_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_scenario_digest_tests COMMAND clc_scenario_digest_tests)

    add_executable(clc_resource_command_tests tests/resource_command_tests.cpp)
    target_link_libraries(clc_resource_command_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_resource_command_tests COMMAND clc_resource_command_tests)

    add_executable(clc_engine_lookup_tests tests/engine_lookup_tests.cpp)
    target_link_libraries(clc_engine_lookup_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_engine_lookup_tests COMMAND clc_engine_lookup_tests)

    add_executable(clc_engine_command_result_tests tests/engine_command_result_tests.cpp)
    target_link_libraries(clc_engine_command_result_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_engine_command_result_tests COMMAND clc_engine_command_result_tests)

    add_executable(clc_persistence_tests tests/persistence_tests.cpp)
    target_link_libraries(clc_persistence_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_persistence_tests COMMAND clc_persistence_tests)

    add_executable(clc_world_state_persistence_tests tests/world_state_persistence_tests.cpp)
    target_link_libraries(clc_world_state_persistence_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_world_state_persistence_tests COMMAND clc_world_state_persistence_tests)

    add_executable(clc_routes_tests tests/routes_tests.cpp)
    target_link_libraries(clc_routes_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_routes_tests COMMAND clc_routes_tests)

    add_executable(clc_caravans_tests tests/caravans_tests.cpp)
    target_link_libraries(clc_caravans_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_caravans_tests COMMAND clc_caravans_tests)

    add_executable(clc_factions_tests tests/factions_tests.cpp)
    target_link_libraries(clc_factions_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_factions_tests COMMAND clc_factions_tests)

    add_executable(clc_contracts_tests tests/contracts_tests.cpp)
    target_link_libraries(clc_contracts_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_contracts_tests COMMAND clc_contracts_tests)

    add_executable(clc_contract_reward_ledger_tests tests/contract_reward_ledger_tests.cpp)
    target_link_libraries(clc_contract_reward_ledger_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_contract_reward_ledger_tests COMMAND clc_contract_reward_ledger_tests)

    add_executable(clc_ownership_tests tests/ownership_tests.cpp)
    target_link_libraries(clc_ownership_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_ownership_tests COMMAND clc_ownership_tests)
endif()
