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
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/external/raylib/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/suites
)

# Link libraries
target_link_libraries(sw_test_suite PRIVATE 
    raylib
    CoreLib
    EntityLib
    ${PROJECT_NAME}
    m # Add math library for Linux compatibility
)

# Add compiler flags for testing
if(MSVC)
    target_compile_options(sw_test_suite PRIVATE ${PROJECT_WARNINGS})
    
    # Temporarily disable specific warnings during development
    target_compile_options(sw_test_suite PRIVATE
        /wd4013  # Undefined functions
        /wd4028  # Parameter list mismatch
        /wd4133  # Incompatible types
        /wd4047  # Different levels of indirection
        /wd4244  # Conversion from double to float
    )
else()
    target_compile_options(sw_test_suite PRIVATE ${PROJECT_WARNINGS})
endif()

# Add test coverage if supported
if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(sw_test_suite PRIVATE --coverage)
    target_link_options(sw_test_suite PRIVATE --coverage)
endif()

# Ensure test resources directory exists
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/resources)

# Copy test resources maintaining directory structure
add_custom_command(TARGET sw_test_suite POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/resources
)

# Register the test with CTest
add_test(
    NAME AllTests
    COMMAND sw_test_suite
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Configure test properties
set_tests_properties(AllTests PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAILED"
    PASS_REGULAR_EXPRESSION "PASSED"
    TIMEOUT 300
) 