file(GLOB CLC_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(test_source IN LISTS CLC_TEST_SOURCES)
    get_filename_component(test_name "${test_source}" NAME_WE)
    string(MAKE_C_IDENTIFIER "${test_name}" test_target_suffix)
    set(test_target "clc_test_${test_target_suffix}")

    add_executable(${test_target} "${test_source}")
    target_link_libraries(${test_target} PRIVATE CityLifeCore::core)
    target_compile_features(${test_target} PRIVATE cxx_std_20)
    add_test(NAME ${test_target} COMMAND ${test_target})
endforeach()
