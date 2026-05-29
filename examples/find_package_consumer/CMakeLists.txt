cmake_minimum_required(VERSION 3.22)

project(CityLifeCoreFindPackageConsumer LANGUAGES CXX)

find_package(CityLifeCore CONFIG REQUIRED)

if(NOT DEFINED CityLifeCore_INCLUDE_DIR OR NOT EXISTS "${CityLifeCore_INCLUDE_DIR}")
    message(FATAL_ERROR "CityLifeCore_INCLUDE_DIR is not defined or does not exist")
endif()

if(NOT DEFINED CityLifeCore_DOCS_DIR OR NOT EXISTS "${CityLifeCore_DOCS_DIR}")
    message(FATAL_ERROR "CityLifeCore_DOCS_DIR is not defined or does not exist")
endif()

if(NOT DEFINED CityLifeCore_DATA_DIR OR NOT EXISTS "${CityLifeCore_DATA_DIR}")
    message(FATAL_ERROR "CityLifeCore_DATA_DIR is not defined or does not exist")
endif()

if(NOT DEFINED CityLifeCore_EXAMPLES_DIR OR NOT EXISTS "${CityLifeCore_EXAMPLES_DIR}")
    message(FATAL_ERROR "CityLifeCore_EXAMPLES_DIR is not defined or does not exist")
endif()

if(NOT EXISTS "${CityLifeCore_EXAMPLES_DIR}/csharp_unity/CityLifeCoreNative.cs")
    message(FATAL_ERROR "CityLifeCoreNative.cs is missing from CityLifeCore_EXAMPLES_DIR")
endif()

if(NOT EXISTS "${CityLifeCore_EXAMPLES_DIR}/csharp_unity/CityLifeWorldSafeAccess.cs")
    message(FATAL_ERROR "CityLifeWorldSafeAccess.cs is missing from CityLifeCore_EXAMPLES_DIR")
endif()

if(NOT EXISTS "${CityLifeCore_EXAMPLES_DIR}/csharp_unity/CityLifeNativeDiagnostics.cs")
    message(FATAL_ERROR "CityLifeNativeDiagnostics.cs is missing from CityLifeCore_EXAMPLES_DIR")
endif()

if(NOT EXISTS "${CityLifeCore_EXAMPLES_DIR}/csharp_unity/CityLifeCoreNative.CompileCheck.csproj")
    message(FATAL_ERROR "CityLifeCoreNative.CompileCheck.csproj is missing from CityLifeCore_EXAMPLES_DIR")
endif()

if(NOT DEFINED CityLifeCore_SCRIPTS_DIR OR NOT EXISTS "${CityLifeCore_SCRIPTS_DIR}")
    message(FATAL_ERROR "CityLifeCore_SCRIPTS_DIR is not defined or does not exist")
endif()

if(NOT EXISTS "${CityLifeCore_SCRIPTS_DIR}/validate_csharp_wrapper.sh")
    message(FATAL_ERROR "validate_csharp_wrapper.sh is missing from CityLifeCore_SCRIPTS_DIR")
endif()

if(NOT EXISTS "${CityLifeCore_SCRIPTS_DIR}/validate_csharp_wrapper.ps1")
    message(FATAL_ERROR "validate_csharp_wrapper.ps1 is missing from CityLifeCore_SCRIPTS_DIR")
endif()

add_executable(city_life_core_consumer main.cpp)
target_link_libraries(city_life_core_consumer PRIVATE CityLifeCore::core)
target_compile_features(city_life_core_consumer PRIVATE cxx_std_20)
