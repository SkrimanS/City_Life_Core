cmake_minimum_required(VERSION 3.22)

project(CityLifeCoreCAbiConsumer LANGUAGES C CXX)

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

add_executable(city_life_core_c_abi_consumer main.c)
set_target_properties(city_life_core_c_abi_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(city_life_core_c_abi_consumer PRIVATE CityLifeCore::core)
