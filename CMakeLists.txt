cmake_minimum_required(VERSION 3.14)
project(ShadowWorker VERSION 1.0 LANGUAGES C)

# Set C standard and warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Windows-specific configuration
if(WIN32)
    enable_language(RC)
    set(CMAKE_RC_COMPILER_INIT windres)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif()

# Store original compiler flags
set(ORIGINAL_C_FLAGS ${CMAKE_C_FLAGS})

# Add compiler warnings for our code (but not for external dependencies)
if(MSVC)
    # Separate warning flags properly for MSVC
    set(PROJECT_WARNINGS 
        /W4     # Warning level 4
        /WX     # Treat warnings as errors
    )
    set(EXTERNAL_WARNINGS 
        /W3             # Warning level 3
        /wd4100        # Unreferenced formal parameter
        /wd4244        # Conversion possible loss of data
        /wd4267        # Conversion from size_t
    )
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

    # Define common warning suppressions
    set(COMMON_WARNING_SUPPRESSIONS
        /wd4100  # Unreferenced formal parameter
        /wd4101  # Unreferenced local variable
        /wd4013  # Undefined functions
        /wd4028  # Parameter list mismatch
        /wd4133  # Incompatible types
        /wd4211  # Nonstandard extension used
        /wd4047  # Different levels of indirection
        /wd4029  # Parameter list mismatch
        /wd4030  # First formal parameter list longer
        /wd4031  # Second formal parameter list longer
        /wd4244  # Conversion possible loss of data
        /wd4459  # Declaration hides global declaration
        /wd4996  # Deprecated functions
    )
endif()

# Unified build directory configuration
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Configure multi-configuration output directories
if(CMAKE_CONFIGURATION_TYPES)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${OUTPUTCONFIG})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${OUTPUTCONFIG})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${OUTPUTCONFIG})
    endforeach()
endif()

# Add build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

# Set workspace and source directories
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Configure testing
enable_testing()
include(CTest)

# Add external dependencies first
add_subdirectory(external/raylib)

# Define source directories
set(CORE_DIR ${SOURCE_DIR}/core)
set(ENTITIES_DIR ${SOURCE_DIR}/entities)
set(WORLD_DIR ${SOURCE_DIR}/world)
set(RESOURCE_DIR ${SOURCE_DIR}/resource)
set(UTILS_DIR ${SOURCE_DIR}/utils)

# Collect source files
file(GLOB CORE_SOURCES "${CORE_DIR}/*.c")
file(GLOB ENTITIES_SOURCES "${ENTITIES_DIR}/*.c")
file(GLOB WORLD_SOURCES "${WORLD_DIR}/*.c")
file(GLOB RESOURCE_SOURCES "${RESOURCE_DIR}/*.c")
file(GLOB UTILS_SOURCES "${UTILS_DIR}/*.c")

# Create libraries
add_library(CoreLib ${CORE_SOURCES})
add_library(EntitiesLib ${ENTITIES_SOURCES})
add_library(WorldLib ${WORLD_SOURCES})
add_library(ResourceLib ${RESOURCE_SOURCES})
add_library(UtilsLib ${UTILS_SOURCES})

# Set include directories for each library
target_include_directories(CoreLib PRIVATE 
    ${INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

target_include_directories(EntitiesLib PRIVATE 
    ${INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

target_include_directories(WorldLib PRIVATE 
    ${INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

target_include_directories(ResourceLib PRIVATE 
    ${INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

target_include_directories(UtilsLib PRIVATE 
    ${INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

# Set compile definitions for each library
target_compile_definitions(CoreLib PRIVATE BUILDING_CORE_LIB)
target_compile_definitions(EntitiesLib PRIVATE BUILDING_ENTITY_LIB)
target_compile_definitions(WorldLib PRIVATE BUILDING_WORLD_LIB)
target_compile_definitions(ResourceLib PRIVATE BUILDING_RESOURCE_LIB)
target_compile_definitions(UtilsLib PRIVATE BUILDING_UTILS_LIB)

# Add debug memory tracking in debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(CoreLib PRIVATE DEBUG_MEMORY_TRACKING)
    target_compile_definitions(EntitiesLib PRIVATE DEBUG_MEMORY_TRACKING)
    target_compile_definitions(WorldLib PRIVATE DEBUG_MEMORY_TRACKING)
    target_compile_definitions(ResourceLib PRIVATE DEBUG_MEMORY_TRACKING)
    target_compile_definitions(UtilsLib PRIVATE DEBUG_MEMORY_TRACKING)
endif()

# Create main executable
add_executable(${PROJECT_NAME} ${SOURCE_DIR}/main.c)

# Link libraries to main executable
target_link_libraries(${PROJECT_NAME} PRIVATE
    CoreLib
    EntitiesLib
    WorldLib
    ResourceLib
    UtilsLib
    raylib
)

# Set include directories for main executable
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

# Set warning flags for main executable
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE ${PROJECT_WARNINGS} ${COMMON_WARNING_SUPPRESSIONS})
endif()

# Copy resources to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

# Add tests
add_subdirectory(tests)

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

if(MSVC)
    target_compile_options(raylib PRIVATE
        /wd4267  # size_t to int conversion
        /wd4018  # signed/unsigned mismatch
        /wd4244  # double to float conversion
    )
endif() 