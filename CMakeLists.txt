cmake_minimum_required(VERSION 3.22)

project(CityLifeCore
    VERSION 0.6.0
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

    add_executable(clc_resource_command_tests tests/resource_command_tests.cpp)
    target_link_libraries(clc_resource_command_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_resource_command_tests COMMAND clc_resource_command_tests)

    add_executable(clc_engine_lookup_tests tests/engine_lookup_tests.cpp)
    target_link_libraries(clc_engine_lookup_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_engine_lookup_tests COMMAND clc_engine_lookup_tests)

    add_executable(clc_engine_command_result_tests tests/engine_command_result_tests.cpp)
    target_link_libraries(clc_engine_command_result_tests PRIVATE CityLifeCore::core)
    add_test(NAME clc_engine_command_result_tests COMMAND clc_engine_command_result_tests)
endif()
