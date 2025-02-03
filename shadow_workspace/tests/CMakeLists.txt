enable_testing()

# Collect all test source files
file(GLOB TEST_SOURCES 
    "*.c"
    "suites/*.c"
)

# Create test executable
add_executable(sw_test_suite ${TEST_SOURCES})

# Include directories
target_include_directories(sw_test_suite PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

# Link libraries
target_link_libraries(sw_test_suite PRIVATE 
    raylib 
    shadow_worker_core
)

# Add compiler flags for testing
target_compile_definitions(sw_test_suite PRIVATE
    TESTING=1
    $<$<CONFIG:Debug>:_DEBUG>
)

# Add test coverage if supported
if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(sw_test_suite PRIVATE --coverage)
    target_link_options(sw_test_suite PRIVATE --coverage)
endif()

# Copy test resources
add_custom_command(TARGET sw_test_suite POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/tests/resources $<TARGET_FILE_DIR:sw_test_suite>/resources
)

# Register the test
add_test(
    NAME AllTests
    COMMAND sw_test_suite
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}) 