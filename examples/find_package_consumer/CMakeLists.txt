cmake_minimum_required(VERSION 3.22)

project(CityLifeCoreFindPackageConsumer LANGUAGES CXX)

find_package(CityLifeCore CONFIG REQUIRED)

foreach(required_dir IN ITEMS
    CityLifeCore_INCLUDE_DIR
    CityLifeCore_DOCS_DIR
    CityLifeCore_DATA_DIR
    CityLifeCore_EXAMPLES_DIR
)
    if(NOT DEFINED ${required_dir} OR NOT EXISTS "${${required_dir}}")
        message(FATAL_ERROR "${required_dir} is not defined or does not exist: ${${required_dir}}")
    endif()
endforeach()

add_executable(city_life_core_consumer main.cpp)
target_link_libraries(city_life_core_consumer PRIVATE CityLifeCore::core)
target_compile_features(city_life_core_consumer PRIVATE cxx_std_20)
