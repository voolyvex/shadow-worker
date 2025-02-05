cmake_minimum_required(VERSION 3.14)
project(ShadowWorker VERSION 1.0 LANGUAGES C)

# Set C standard and warnings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Set workspace and source directories
set(WORKSPACE_DIR ${CMAKE_SOURCE_DIR}/shadow_workspace)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# Output directories with proper debug/release handling
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${WORKSPACE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${WORKSPACE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${WORKSPACE_DIR}/lib)

if(WIN32)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${WORKSPACE_DIR}/bin/${OUTPUTCONFIG}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${WORKSPACE_DIR}/lib/${OUTPUTCONFIG}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${WORKSPACE_DIR}/lib/${OUTPUTCONFIG}")
    endforeach()
endif()

# Configure testing
enable_testing()
include(CTest)

# Add external dependencies first
add_subdirectory(external/raylib)

# Define source files using file(GLOB ...)
file(GLOB CORE_SOURCES
    "${SOURCE_DIR}/core/*.c"
)

file(GLOB ENTITY_SOURCES
    "${SOURCE_DIR}/entities/*.c"
)

file(GLOB GAME_SOURCES
    "${SOURCE_DIR}/*.c"
)

# Create static libraries for each component
add_library(CoreLib STATIC ${CORE_SOURCES})
add_library(EntityLib STATIC ${ENTITY_SOURCES})

# Set properties for the libraries
set_target_properties(CoreLib EntityLib PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    ARCHIVE_OUTPUT_DIRECTORY "${WORKSPACE_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${WORKSPACE_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${WORKSPACE_DIR}/bin"
)

# Add include directories for libraries
target_include_directories(CoreLib PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SOURCE_DIR}
    ${SOURCE_DIR}/core
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

target_include_directories(EntityLib PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SOURCE_DIR}
    ${SOURCE_DIR}/entities
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

# Create main executable
add_executable(${PROJECT_NAME} ${GAME_SOURCES})

# Enable exports for linking with tests
set_target_properties(${PROJECT_NAME} PROPERTIES
    ENABLE_EXPORTS ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    RUNTIME_OUTPUT_DIRECTORY "${WORKSPACE_DIR}/bin"
    DEBUG_POSTFIX "d"
    PDB_OUTPUT_DIRECTORY "${WORKSPACE_DIR}/bin"
)

# Add include directories for main executable
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SOURCE_DIR}
    ${SOURCE_DIR}/core
    ${SOURCE_DIR}/entities
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    CoreLib
    EntityLib
    raylib
    ${OPENGL_LIBRARIES}
    ${CMAKE_DL_LIBS}
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
    
    # Windows-specific linker flags
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
        $<$<CONFIG:Debug>:/INCREMENTAL:NO>
        $<$<CONFIG:Debug>:/OPT:NOREF>
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/INCREMENTAL:NO>
    )
endif()

# Ensure proper symbol export/import
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        GRAPHICS_API_OPENGL_33
        PLATFORM_DESKTOP
        BUILD_LIBTYPE_SHARED
    )
    
    target_compile_definitions(CoreLib PRIVATE 
        BUILDING_CORE_LIB
        $<$<CONFIG:Debug>:_DEBUG>
    )
    target_compile_definitions(EntityLib PRIVATE 
        BUILDING_ENTITY_LIB
        $<$<CONFIG:Debug>:_DEBUG>
    )
endif()

# Apply warning flags
if(MSVC)
    foreach(TARGET CoreLib EntityLib ${PROJECT_NAME})
        target_compile_options(${TARGET} PRIVATE 
            ${PROJECT_WARNINGS}
            ${COMMON_WARNING_SUPPRESSIONS}
        )
    endforeach()
endif()

# Copy resources to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/../resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
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