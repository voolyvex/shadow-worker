# Core library for Shadow Worker

# Collect source files
file(GLOB CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/entities/*.c"
)

add_library(shadow_worker_core
    world.c
    game_impl.c
    entity.c
    entity_pool.c
    core/entity.c
    core/logger.c
    core/map.c
    core/resource_manager.c
    core/room_loader.c
    core/sound_manager.c
    core/world_gen.c
)

target_include_directories(shadow_worker_core
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/raylib/src
)

target_link_libraries(shadow_worker_core
    PUBLIC
    raylib
)

# Main executable
add_executable(sw_game
    main.c
)

target_link_libraries(sw_game
    PRIVATE
    shadow_worker_core
)
