cmake_minimum_required(VERSION 3.22)

project(CityLifeCore
    VERSION 0.9.9
    DESCRIPTION "Universal headless simulation core for living settlements"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(CLC_IS_TOP_LEVEL ON)
else()
    set(CLC_IS_TOP_LEVEL OFF)
endif()

set(CLC_DEFAULT_BUILD_TESTS ${CLC_IS_TOP_LEVEL})
set(CLC_DEFAULT_BUILD_EXAMPLES ${CLC_IS_TOP_LEVEL})
set(CLC_DEFAULT_BUILD_TOOLS ${CLC_IS_TOP_LEVEL})

option(CLC_BUILD_TESTS "Build City Life Core tests" ${CLC_DEFAULT_BUILD_TESTS})
option(CLC_BUILD_EXAMPLES "Build City Life Core examples" ${CLC_DEFAULT_BUILD_EXAMPLES})
option(CLC_BUILD_TOOLS "Build City Life Core command line tools" ${CLC_DEFAULT_BUILD_TOOLS})

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

if(CLC_BUILD_TOOLS)
    add_executable(clc_runner apps/clc_runner/main.cpp)
    target_link_libraries(clc_runner PRIVATE CityLifeCore::core)
endif()

if(CLC_BUILD_EXAMPLES)
    add_executable(clc_example_basic_runtime examples/basic_runtime.cpp)
    target_link_libraries(clc_example_basic_runtime PRIVATE CityLifeCore::core)

    add_executable(clc_example_tick_runtime examples/tick_runtime.cpp)
    target_link_libraries(clc_example_tick_runtime PRIVATE CityLifeCore::core)

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

    function(clc_add_test target source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE CityLifeCore::core)
        add_test(NAME ${target} COMMAND ${target})
    endfunction()

    clc_add_test(clc_smoke_tests tests/smoke_tests.cpp)
    clc_add_test(clc_time_hardening_tests tests/time_hardening_tests.cpp)
    clc_add_test(clc_storage_hardening_tests tests/storage_hardening_tests.cpp)
    clc_add_test(clc_economy_tests tests/economy_tests.cpp)
    clc_add_test(clc_ledger_hardening_tests tests/ledger_hardening_tests.cpp)
    clc_add_test(clc_simulation_tests tests/simulation_tests.cpp)
    clc_add_test(clc_settlement_tick_tests tests/settlement_tick_tests.cpp)
    clc_add_test(clc_settlement_tick_remainder_validation_tests tests/settlement_tick_remainder_validation_tests.cpp)
    clc_add_test(clc_scenario_digest_tests tests/scenario_digest_tests.cpp)
    clc_add_test(clc_resource_command_tests tests/resource_command_tests.cpp)
    clc_add_test(clc_engine_lookup_tests tests/engine_lookup_tests.cpp)
    clc_add_test(clc_engine_command_result_tests tests/engine_command_result_tests.cpp)
    clc_add_test(clc_persistence_tests tests/persistence_tests.cpp)
    clc_add_test(clc_world_state_persistence_tests tests/world_state_persistence_tests.cpp)
    clc_add_test(clc_world_state_deserialization_hardening_tests tests/world_state_deserialization_hardening_tests.cpp)
    clc_add_test(clc_runtime_persistence_tests tests/runtime_persistence_tests.cpp)
    clc_add_test(clc_runtime_restore_transaction_tests tests/runtime_restore_transaction_tests.cpp)
    clc_add_test(clc_runtime_persistence_validation_tests tests/runtime_persistence_validation_tests.cpp)
    clc_add_test(clc_runtime_replay_persistence_tests tests/runtime_replay_persistence_tests.cpp)
    clc_add_test(clc_runtime_cargo_delivery_persistence_tests tests/runtime_cargo_delivery_persistence_tests.cpp)
    clc_add_test(clc_runtime_cargo_delivery_result_validation_tests tests/runtime_cargo_delivery_result_validation_tests.cpp)
    clc_add_test(clc_runtime_workflow_hardening_tests tests/runtime_workflow_hardening_tests.cpp)
    clc_add_test(clc_runtime_corrupted_save_tests tests/runtime_corrupted_save_tests.cpp)
    clc_add_test(clc_simulation_runtime_bundle_tests tests/simulation_runtime_bundle_tests.cpp)
    clc_add_test(clc_simulation_runtime_workflow_tests tests/simulation_runtime_workflow_tests.cpp)
    clc_add_test(clc_simulation_runtime_scenario_tests tests/simulation_runtime_scenario_tests.cpp)
    clc_add_test(clc_runtime_legacy_time_restore_tests tests/runtime_legacy_time_restore_tests.cpp)
    clc_add_test(clc_runtime_full_roundtrip_tests tests/runtime_full_roundtrip_tests.cpp)
    clc_add_test(clc_runtime_tick_tests tests/runtime_tick_tests.cpp)
    clc_add_test(clc_runtime_tick_deadline_tests tests/runtime_tick_deadline_tests.cpp)
    clc_add_test(clc_runtime_tick_deadline_persistence_tests tests/runtime_tick_deadline_persistence_tests.cpp)
    clc_add_test(clc_runtime_run_ticks_tests tests/runtime_run_ticks_tests.cpp)
    clc_add_test(clc_runtime_run_days_tests tests/runtime_run_days_tests.cpp)
    clc_add_test(clc_runtime_run_until_arrival_tests tests/runtime_run_until_arrival_tests.cpp)
    clc_add_test(clc_runtime_auto_contract_tests tests/runtime_auto_contract_tests.cpp)
    clc_add_test(clc_runtime_arrival_contract_tests tests/runtime_arrival_contract_tests.cpp)
    clc_add_test(clc_runtime_arrival_contract_roundtrip_tests tests/runtime_arrival_contract_roundtrip_tests.cpp)
    clc_add_test(clc_runtime_event_log_tests tests/runtime_event_log_tests.cpp)
    clc_add_test(clc_runtime_tick_event_log_tests tests/runtime_tick_event_log_tests.cpp)
    clc_add_test(clc_runtime_event_log_absolute_tick_tests tests/runtime_event_log_absolute_tick_tests.cpp)
    clc_add_test(clc_runtime_event_log_roundtrip_tests tests/runtime_event_log_roundtrip_tests.cpp)
    clc_add_test(clc_runtime_event_log_analysis_tests tests/runtime_event_log_analysis_tests.cpp)
    clc_add_test(clc_runtime_event_log_runtime_time_overload_tests tests/runtime_event_log_runtime_time_overload_tests.cpp)
    clc_add_test(clc_routes_tests tests/routes_tests.cpp)
    clc_add_test(clc_caravans_tests tests/caravans_tests.cpp)
    clc_add_test(clc_factions_tests tests/factions_tests.cpp)
    clc_add_test(clc_contracts_tests tests/contracts_tests.cpp)
    clc_add_test(clc_contract_tick_deadline_tests tests/contract_tick_deadline_tests.cpp)
    clc_add_test(clc_contract_reward_ledger_tests tests/contract_reward_ledger_tests.cpp)
    clc_add_test(clc_contract_reward_transaction_tests tests/contract_reward_transaction_tests.cpp)
    clc_add_test(clc_ownership_tests tests/ownership_tests.cpp)
endif()
