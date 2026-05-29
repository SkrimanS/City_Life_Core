add_executable(clc_core_benchmarks
    core_benchmarks.cpp
)

set_target_properties(clc_core_benchmarks PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(clc_core_benchmarks PRIVATE CityLifeCore::core)
target_compile_features(clc_core_benchmarks PRIVATE cxx_std_20)
