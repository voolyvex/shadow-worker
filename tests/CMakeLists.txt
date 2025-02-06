enable_testing()

# Test configuration
set(TEST_SOURCES
    test_main.c
    test_env.c
    suites/test_core_systems.c
    suites/test_world.c
    suites/test_memory.c
    suites/test_map.c
    suites/test_texture_manager.c
    suites/test_resource_manager.c
    suites/test_logger.c
    suites/test_map_gen.c
    suites/test_integration.c
)

# Define test include directories
set(TEST_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/entities
    ${CMAKE_SOURCE_DIR}/include/world
    ${CMAKE_SOURCE_DIR}/include/resource
    ${CMAKE_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/tests/include
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

# Create test executable
add_executable(sw_test_suite ${TEST_SOURCES})

# Set include directories for test executable
target_include_directories(sw_test_suite PRIVATE ${TEST_INCLUDE_DIRS})

# Link with project libraries
target_link_libraries(sw_test_suite PRIVATE
    CoreLib
    EntitiesLib
    WorldLib
    ResourceLib
    UtilsLib
    raylib
)

# Copy test resources
add_custom_command(TARGET sw_test_suite POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/tests/resources $<TARGET_FILE_DIR:sw_test_suite>/test_resources
)

# Add tests to CTest
add_test(NAME CoreSystemsTest COMMAND sw_test_suite --suite=core)
add_test(NAME WorldTest COMMAND sw_test_suite --suite=world)
add_test(NAME MemoryTest COMMAND sw_test_suite --suite=memory)
add_test(NAME MapTest COMMAND sw_test_suite --suite=map)
add_test(NAME TextureManagerTest COMMAND sw_test_suite --suite=texture)
add_test(NAME ResourceManagerTest COMMAND sw_test_suite --suite=resource)
add_test(NAME IntegrationTest COMMAND sw_test_suite --suite=integration)

# Set test properties
set_tests_properties(CoreSystemsTest WorldTest MemoryTest MapTest 
    TextureManagerTest ResourceManagerTest IntegrationTest
    PROPERTIES
    ENVIRONMENT "TEST_RESOURCES_PATH=${CMAKE_BINARY_DIR}/test_resources"
    TIMEOUT 30
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
        /wd4312  # Type cast to pointer of greater size
    )
else()
    target_compile_options(sw_test_suite PRIVATE ${PROJECT_WARNINGS})
endif()

# Add test coverage if supported
if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(sw_test_suite PRIVATE --coverage)
    target_link_options(sw_test_suite PRIVATE --coverage)
endif()

# Set output directories
set_target_properties(sw_test_suite PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
    DEBUG_POSTFIX "d"
)

# Test output directory
set(TEST_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
if(CMAKE_BUILD_TYPE)
    string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
    set(TEST_OUTPUT_DIR "${TEST_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}")
endif()

# Test executable path with debug postfix handling
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TEST_EXECUTABLE "${TEST_OUTPUT_DIR}/sw_test_suited${CMAKE_EXECUTABLE_SUFFIX}")
else()
    set(TEST_EXECUTABLE "${TEST_OUTPUT_DIR}/sw_test_suite${CMAKE_EXECUTABLE_SUFFIX}")
endif()

# Test resources setup
set(TEST_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(TEST_RESOURCES_OUTPUT_DIR ${TEST_OUTPUT_DIR}/resources)

# Copy test resources
add_custom_command(TARGET sw_test_suite PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_RESOURCES_OUTPUT_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${TEST_RESOURCES_DIR} ${TEST_RESOURCES_OUTPUT_DIR}
    COMMENT "Copying test resources to ${TEST_RESOURCES_OUTPUT_DIR}"
)

# Also copy main resources for tests that need them
add_custom_command(TARGET sw_test_suite PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_OUTPUT_DIR}/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${TEST_OUTPUT_DIR}/resources
    COMMENT "Copying main resources to ${TEST_OUTPUT_DIR}/resources"
)

# Register the test with CTest
add_test(
    NAME AllTests
    COMMAND ${TEST_EXECUTABLE}
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