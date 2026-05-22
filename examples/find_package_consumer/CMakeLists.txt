cmake_minimum_required(VERSION 3.22)

project(CityLifeCoreFindPackageConsumer LANGUAGES CXX)

find_package(CityLifeCore CONFIG REQUIRED)

add_executable(city_life_core_consumer main.cpp)
target_link_libraries(city_life_core_consumer PRIVATE CityLifeCore::core)
target_compile_features(city_life_core_consumer PRIVATE cxx_std_20)
