cmake_minimum_required(VERSION 3.22)

project(CityLifeCore
    VERSION 0.9.7
    DESCRIPTION "Universal headless simulation core for living settlements"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(CLC_BUILD_TESTS "Build City Life Core tests" ON)
option(CLC_BUILD_EXAMPLES "Build City Life Core examples" ON)

set(CLC_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/CityLifeCore" CACHE STRING "City Life Core CMake package install directory")

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
    src/clc/sim/SimulationRuntimePersistenceValidation.cpp
    src/clc/sim/SimulationRuntimeWorkflow.cpp
    src/clc/sim/SimulationRuntimeScenario.cpp
    src/clc/sim/SimulationRuntimeTick.cpp
    src/clc/sim/SimulationRuntimeEvents.cpp
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

set_target_properties(city_life_core PROPERTIES EXPORT_NAME core)

target_include_directories(city_life_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(city_life_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(city_life_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(city_life_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(clc_runner apps/clc_runner/main.cpp)
target_link_libraries(clc_runner PRIVATE CityLifeCore::core)

if(CLC_BUILD_EXAMPLES)
    add_executable(clc_example_basic_runtime examples/basic_runtime.cpp)
    target_link_libraries(clc_example_basic_runtime PRIVATE CityLifeCore::core)

    add_executable(clc_example_save_load_roundtrip examples/save_load_roundtrip.cpp)
    target_link_libraries(clc_example_save_load_roundtrip PRIVATE CityLifeCore::core)

    add_executable(clc_example_replay_persistence examples/replay_persistence.cpp)
    target_link_libraries(clc_example_replay_persistence PRIVATE CityLifeCore::core)
endif()

install(
    TARGETS city_life_core
    EXPORT CityLifeCoreTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    FILES README.md CHANGELOG.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

install(
    DIRECTORY docs/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    FILES_MATCHING PATTERN "*.md"
)

install(
    DIRECTORY examples/
    DESTINATION ${CMAKE_INSTALL_DOCDIR}/examples
    FILES_MATCHING
        PATTERN "*.cpp"
        PATTERN "*.md"
)

install(
    DIRECTORY data/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/CityLifeCore/data
)

install(
    EXPORT CityLifeCoreTargets
    NAMESPACE CityLifeCore::
    FILE CityLifeCoreTargets.cmake
    DESTINATION ${CLC_INSTALL_CMAKEDIR}
)

configure_package_config_file(
    cmake/CityLifeCoreConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/CityLifeCoreConfig.cmake"
    INSTALL_DESTINATION ${CLC_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CityLifeCoreConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/CityLifeCoreConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/CityLifeCoreConfigVersion.cmake"
    DESTINATION ${CLC_INSTALL_CMAKEDIR}
)

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

    add_executable(clc_settlement_tick_tests tests/settlement_tick_tests.cpp)
    target_link_libraries(clc_settlement_tick_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_settlement_tick_tests COMMAND clc_settlement_tick_tests)

    add_executable(clc_settlement_tick_remainder_validation_tests tests/settlement_tick_remainder_validation_tests.cpp)
    target_link_libraries(clc_settlement_tick_remainder_validation_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_settlement_tick_remainder_validation_tests COMMAND clc_settlement_tick_remainder_validation_tests)

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

    add_executable(clc_runtime_persistence_tests tests/runtime_persistence_tests.cpp)
    target_link_libraries(clc_runtime_persistence_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_persistence_tests COMMAND clc_runtime_persistence_tests)

    add_executable(clc_runtime_persistence_validation_tests tests/runtime_persistence_validation_tests.cpp)
    target_link_libraries(clc_runtime_persistence_validation_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_persistence_validation_tests COMMAND clc_runtime_persistence_validation_tests)

    add_executable(clc_runtime_replay_persistence_tests tests/runtime_replay_persistence_tests.cpp)
    target_link_libraries(clc_runtime_replay_persistence_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_replay_persistence_tests COMMAND clc_runtime_replay_persistence_tests)

    add_executable(clc_runtime_cargo_delivery_persistence_tests tests/runtime_cargo_delivery_persistence_tests.cpp)
    target_link_libraries(clc_runtime_cargo_delivery_persistence_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_cargo_delivery_persistence_tests COMMAND clc_runtime_cargo_delivery_persistence_tests)

    add_executable(clc_runtime_cargo_delivery_result_validation_tests tests/runtime_cargo_delivery_result_validation_tests.cpp)
    target_link_libraries(clc_runtime_cargo_delivery_result_validation_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_cargo_delivery_result_validation_tests COMMAND clc_runtime_cargo_delivery_result_validation_tests)

    add_executable(clc_runtime_corrupted_save_tests tests/runtime_corrupted_save_tests.cpp)
    target_link_libraries(clc_runtime_corrupted_save_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_corrupted_save_tests COMMAND clc_runtime_corrupted_save_tests)

    add_executable(clc_simulation_runtime_bundle_tests tests/simulation_runtime_bundle_tests.cpp)
    target_link_libraries(clc_simulation_runtime_bundle_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_simulation_runtime_bundle_tests COMMAND clc_simulation_runtime_bundle_tests)

    add_executable(clc_simulation_runtime_workflow_tests tests/simulation_runtime_workflow_tests.cpp)
    target_link_libraries(clc_simulation_runtime_workflow_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_simulation_runtime_workflow_tests COMMAND clc_simulation_runtime_workflow_tests)

    add_executable(clc_simulation_runtime_scenario_tests tests/simulation_runtime_scenario_tests.cpp)
    target_link_libraries(clc_simulation_runtime_scenario_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_simulation_runtime_scenario_tests COMMAND clc_simulation_runtime_scenario_tests)

    add_executable(clc_runtime_legacy_time_restore_tests tests/runtime_legacy_time_restore_tests.cpp)
    target_link_libraries(clc_runtime_legacy_time_restore_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_legacy_time_restore_tests COMMAND clc_runtime_legacy_time_restore_tests)

    add_executable(clc_runtime_full_roundtrip_tests tests/runtime_full_roundtrip_tests.cpp)
    target_link_libraries(clc_runtime_full_roundtrip_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_full_roundtrip_tests COMMAND clc_runtime_full_roundtrip_tests)

    add_executable(clc_runtime_tick_tests tests/runtime_tick_tests.cpp)
    target_link_libraries(clc_runtime_tick_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_tick_tests COMMAND clc_runtime_tick_tests)

    add_executable(clc_runtime_tick_deadline_tests tests/runtime_tick_deadline_tests.cpp)
    target_link_libraries(clc_runtime_tick_deadline_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_tick_deadline_tests COMMAND clc_runtime_tick_deadline_tests)

    add_executable(clc_runtime_tick_deadline_persistence_tests tests/runtime_tick_deadline_persistence_tests.cpp)
    target_link_libraries(clc_runtime_tick_deadline_persistence_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_tick_deadline_persistence_tests COMMAND clc_runtime_tick_deadline_persistence_tests)

    add_executable(clc_runtime_run_days_tests tests/runtime_run_days_tests.cpp)
    target_link_libraries(clc_runtime_run_days_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_run_days_tests COMMAND clc_runtime_run_days_tests)

    add_executable(clc_runtime_run_until_arrival_tests tests/runtime_run_until_arrival_tests.cpp)
    target_link_libraries(clc_runtime_run_until_arrival_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_run_until_arrival_tests COMMAND clc_runtime_run_until_arrival_tests)

    add_executable(clc_runtime_auto_contract_tests tests/runtime_auto_contract_tests.cpp)
    target_link_libraries(clc_runtime_auto_contract_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_auto_contract_tests COMMAND clc_runtime_auto_contract_tests)

    add_executable(clc_runtime_arrival_contract_tests tests/runtime_arrival_contract_tests.cpp)
    target_link_libraries(clc_runtime_arrival_contract_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_arrival_contract_tests COMMAND clc_runtime_arrival_contract_tests)

    add_executable(clc_runtime_arrival_contract_roundtrip_tests tests/runtime_arrival_contract_roundtrip_tests.cpp)
    target_link_libraries(clc_runtime_arrival_contract_roundtrip_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_arrival_contract_roundtrip_tests COMMAND clc_runtime_arrival_contract_roundtrip_tests)

    add_executable(clc_runtime_event_log_tests tests/runtime_event_log_tests.cpp)
    target_link_libraries(clc_runtime_event_log_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_event_log_tests COMMAND clc_runtime_event_log_tests)

    add_executable(clc_runtime_event_log_absolute_tick_tests tests/runtime_event_log_absolute_tick_tests.cpp)
    target_link_libraries(clc_runtime_event_log_absolute_tick_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_event_log_absolute_tick_tests COMMAND clc_runtime_event_log_absolute_tick_tests)

    add_executable(clc_runtime_event_log_roundtrip_tests tests/runtime_event_log_roundtrip_tests.cpp)
    target_link_libraries(clc_runtime_event_log_roundtrip_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_event_log_roundtrip_tests COMMAND clc_runtime_event_log_roundtrip_tests)

    add_executable(clc_runtime_event_log_analysis_tests tests/runtime_event_log_analysis_tests.cpp)
    target_link_libraries(clc_runtime_event_log_analysis_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_runtime_event_log_analysis_tests COMMAND clc_runtime_event_log_analysis_tests)

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

    add_executable(clc_contract_tick_deadline_tests tests/contract_tick_deadline_tests.cpp)
    target_link_libraries(clc_contract_tick_deadline_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_contract_tick_deadline_tests COMMAND clc_contract_tick_deadline_tests)

    add_executable(clc_contract_reward_ledger_tests tests/contract_reward_ledger_tests.cpp)
    target_link_libraries(clc_contract_reward_ledger_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_contract_reward_ledger_tests COMMAND clc_contract_reward_ledger_tests)

    add_executable(clc_ownership_tests tests/ownership_tests.cpp)
    target_link_libraries(clc_ownership_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_ownership_tests COMMAND clc_ownership_tests)
endif()
