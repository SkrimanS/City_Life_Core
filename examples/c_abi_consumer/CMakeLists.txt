cmake_minimum_required(VERSION 3.22)

project(CityLifeCoreCAbiConsumer LANGUAGES C CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(city_life_core_c_abi_consumer main.c)
set_target_properties(city_life_core_c_abi_consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(city_life_core_c_abi_consumer PRIVATE CityLifeCore::core)
