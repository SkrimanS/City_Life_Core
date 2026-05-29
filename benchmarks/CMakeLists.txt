add_executable(clc_core_benchmarks
    core_benchmarks.cpp
)

target_link_libraries(clc_core_benchmarks PRIVATE CityLifeCore::core)
target_compile_features(clc_core_benchmarks PRIVATE cxx_std_20)
