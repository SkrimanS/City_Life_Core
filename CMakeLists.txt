cmake_minimum_required(VERSION 3.22)

project(CityLifeCore
    VERSION 0.3.0
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
    src/clc/sim/Settlement.cpp
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
endif()
