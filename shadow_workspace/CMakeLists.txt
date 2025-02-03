cmake_minimum_required(VERSION 3.14)
project(shadow_worker C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add _CRT_SECURE_NO_WARNINGS to disable unsafe function warnings
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add platform-specific output directories for Windows
if(WIN32)
    # Force Debug configuration if not specified
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()
    
    # Ensure proper slashes in paths
    string(REPLACE "\\" "/" CMAKE_BINARY_DIR ${CMAKE_BINARY_DIR})
    
    # Set output directories for all configurations
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin/${OUTPUTCONFIG}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib/${OUTPUTCONFIG}")
    endforeach()
endif()

# Add raylib as a subdirectory if it exists, otherwise download it
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/raylib)
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/external)
    execute_process(
        COMMAND git clone --depth 1 --branch 4.5.0 https://github.com/raysan5/raylib.git raylib
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/external
    )
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/external/raylib)

# Collect source files
file(GLOB CORE_SOURCES "src/core/*.c")
file(GLOB ENTITY_SOURCES "src/entities/*.c")
file(GLOB MAIN_SOURCES 
    "src/main.c"
    "src/game_impl.c"
    "src/world.c"
)

set(SOURCES 
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    ${MAIN_SOURCES}
)

# Create main executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Add platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Add debug preprocessor definition
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
)

# Add sanitizer flags for Debug builds on non-Windows platforms
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
    )
endif()

# Copy resources to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

# Testing configuration
enable_testing()

# Collect test source files
file(GLOB TEST_SOURCES "tests/*.c")

# Create test executable
add_executable(${PROJECT_NAME}_tests 
    ${TEST_SOURCES}
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    "src/game_impl.c"
    "src/world.c"
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

target_link_libraries(${PROJECT_NAME}_tests PRIVATE raylib)

# Add platform-specific libraries for tests
if(WIN32)
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE winmm)
endif()

# Add test
add_test(
    NAME ${PROJECT_NAME}_tests
    COMMAND ${PROJECT_NAME}_tests
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Custom target for running tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${PROJECT_NAME}_tests
)

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

get_target_property(INCLUDE_DIRS ${PROJECT_NAME} INCLUDE_DIRECTORIES)
message("Include directories: ${INCLUDE_DIRS}")

# Add test compilation flags
target_compile_options(${PROJECT_NAME}_tests PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>
)

# Add test coverage if supported
if(CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(${PROJECT_NAME}_tests PRIVATE --coverage)
    target_link_options(${PROJECT_NAME}_tests PRIVATE --coverage)
endif()

add_subdirectory(src)
add_subdirectory(tests) 