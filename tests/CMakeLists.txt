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

# Test output directory
set(TEST_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)
file(MAKE_DIRECTORY ${TEST_OUTPUT_DIR})

# Test resources setup
set(TEST_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(TEST_RESOURCES_OUTPUT_DIR ${TEST_OUTPUT_DIR}/resources)

# Copy test resources
add_custom_command(TARGET sw_test_suite POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${TEST_RESOURCES_DIR} ${TEST_RESOURCES_OUTPUT_DIR}
)

# Register the test with CTest
add_test(
    NAME AllTests
    COMMAND sw_test_suite
    WORKING_DIRECTORY ${TEST_OUTPUT_DIR}
)

# Update test properties
set_tests_properties(AllTests PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAILED"
    PASS_REGULAR_EXPRESSION "PASSED"
    TIMEOUT 300
    WORKING_DIRECTORY ${TEST_OUTPUT_DIR}
    ENVIRONMENT "TEST_RESOURCES_DIR=${TEST_RESOURCES_OUTPUT_DIR}"
) 