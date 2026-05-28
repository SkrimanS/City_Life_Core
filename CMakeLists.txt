cmake_minimum_required(VERSION 3.20)

project(
    CityLifeCore
    VERSION 1.0.0
    DESCRIPTION "Headless simulation SDK for living settlements"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_SHARED_LIBS "Build City Life Core libraries as shared libraries" OFF)
option(CLC_BUILD_TESTS "Build City Life Core tests" ON)
option(CLC_BUILD_EXAMPLES "Build City Life Core examples" ON)
option(CLC_BUILD_BENCHMARKS "Build City Life Core benchmarks" OFF)
option(CLC_BUILD_TOOLS "Build City Life Core tools" ON)

add_library(city_life_core
    src/clc/Version.cpp
    src/clc/Time.cpp
    src/clc/data/DataRegistry.cpp
    src/clc/data/DataRegistryValidation.cpp
    src/clc/data/DataPackLoader.cpp
    src/clc/sim/ResourceStorage.cpp
    src/clc/sim/SettlementSimulation.cpp
    src/clc/sim/SettlementReport.cpp
    src/clc/sim/SettlementRoute.cpp
    src/clc/sim/CaravanSimulation.cpp
    src/clc/sim/FactionSystem.cpp
    src/clc/sim/Contracts.cpp
    src/clc/sim/SimulationRuntime.cpp
    src/clc/sim/SimulationRuntimeValidation.cpp
    src/clc/sim/SimulationRuntimePersistence.cpp
    src/clc/sim/SimulationRuntimePersistenceValidation.cpp
    src/clc/sim/WorldEvent.cpp
    src/clc/sim/WorldEventLog.cpp
    src/clc/economy/Market.cpp
    src/clc/economy/Wallet.cpp
    src/clc/economy/Trade.cpp
    src/clc/economy/Ledger.cpp
    src/clc/c/CityLifeCoreC.cpp
)

add_library(CityLifeCore::core ALIAS city_life_core)

target_compile_features(city_life_core PUBLIC cxx_std_20)

target_include_directories(city_life_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(city_life_core PROPERTIES
    OUTPUT_NAME city_life_core
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

if(CLC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CLC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CLC_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(CLC_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

set(CLC_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/CityLifeCore)
set(CLC_INSTALL_DATA_DIR ${CMAKE_INSTALL_DATADIR}/city-life-core)
set(CLC_INSTALL_EXAMPLES_DIR ${CMAKE_INSTALL_DOCDIR}/examples)

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
    FILES_MATCHING
        PATTERN "*.md"
        PATTERN "release-notes-0*.md" EXCLUDE
        PATTERN "release-manifest-draft-*.md" EXCLUDE
        PATTERN "release-blockers.md" EXCLUDE
        PATTERN "release-checklist.md" EXCLUDE
)

install(
    DIRECTORY examples/
    DESTINATION ${CLC_INSTALL_EXAMPLES_DIR}
    FILES_MATCHING
        PATTERN "*.cpp"
        PATTERN "*.c"
        PATTERN "*.cs"
        PATTERN "*.txt"
        PATTERN "*.md"
)

install(
    DIRECTORY data/
    DESTINATION ${CLC_INSTALL_DATA_DIR}
    FILES_MATCHING
        PATTERN "*.clcd"
)

install(
    EXPORT CityLifeCoreTargets
    NAMESPACE CityLifeCore::
    DESTINATION ${CLC_INSTALL_CMAKEDIR}
)

configure_package_config_file(
    cmake/CityLifeCoreConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CityLifeCoreConfig.cmake
    INSTALL_DESTINATION ${CLC_INSTALL_CMAKEDIR}
    PATH_VARS
        CLC_INSTALL_DATA_DIR
        CLC_INSTALL_EXAMPLES_DIR
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CityLifeCoreConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/CityLifeCoreConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/CityLifeCoreConfigVersion.cmake
    DESTINATION ${CLC_INSTALL_CMAKEDIR}
)

include(CPack)
