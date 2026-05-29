file(GLOB CLC_EXAMPLE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(example_source IN LISTS CLC_EXAMPLE_SOURCES)
    get_filename_component(example_name "${example_source}" NAME_WE)
    string(MAKE_C_IDENTIFIER "${example_name}" example_target_suffix)
    set(example_target "clc_example_${example_target_suffix}")

    add_executable(${example_target} "${example_source}")
    target_link_libraries(${example_target} PRIVATE CityLifeCore::core)
    target_compile_features(${example_target} PRIVATE cxx_std_20)
endforeach()
