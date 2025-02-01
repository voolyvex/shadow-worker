#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../include/world.h"
#include "../include/entity.h"
#include "../include/logger.h"
#include "../include/entities/player.h"

void test_world_creation(void) {
    World* world = CreateWorld();
    assert(world != NULL);
    assert((int)world->dimensions.x == ESTATE_WIDTH);
    assert((int)world->dimensions.y == ESTATE_HEIGHT);
    DestroyWorld(world);
}

void test_world_tile_access(void) {
    World* world = CreateWorld();
    assert(world != NULL);

    // Test setting and getting tiles
    SetTileAt(world, 5, 5, TILE_WALL);
    assert(GetTileAt(world, 5, 5) == TILE_WALL);

    SetTileAt(world, 10, 10, TILE_FLOOR);
    assert(GetTileAt(world, 10, 10) == TILE_FLOOR);

    // Test bounds checking
    assert(GetTileAt(world, -1, 5) == TILE_GRASS); // Default tile for out of bounds
    assert(GetTileAt(world, 5, -1) == TILE_GRASS);
    assert(GetTileAt(world, ESTATE_WIDTH + 1, 5) == TILE_GRASS);
    assert(GetTileAt(world, 5, ESTATE_HEIGHT + 1) == TILE_GRASS);

    DestroyWorld(world);
}

void test_player_creation(void) {
    PlayerData* player = CreatePlayer();
    assert(player != NULL);
    assert(player->health == 100);
    assert(player->stamina == 100);
    assert(player->resonance == 0);
    DestroyPlayer();
}

void test_player_movement(void) {
    PlayerData* player = CreatePlayer();
    assert(player != NULL);

    Vector2 initial_pos = player->position;
    player->velocity = (Vector2){10, 0};
    UpdatePlayer(0.016f); // One frame at 60 FPS

    assert(player->position.x > initial_pos.x);
    assert(player->position.y == initial_pos.y);

    DestroyPlayer();
}

int main(void) {
    Logger_Init("test_log.txt");
    LOG_INFO(LOG_CORE, "Starting tests...");

    test_world_creation();
    test_world_tile_access();
    test_player_creation();
    test_player_movement();

    LOG_INFO(LOG_CORE, "All tests passed!");
    Logger_Shutdown();
    return 0;
} 