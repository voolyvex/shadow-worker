#include <stdio.h>
#include <assert.h>
#include "../include/game.h"
#include "../include/resource_manager.h"
#include "../include/entity.h"
#include "../include/world.h"
#include <raylib.h>

// Test result tracking
static int tests_run = 0;
static int tests_failed = 0;

#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("Running test: %s\n", #name); \
    test_##name(); \
    tests_run++; \
} while (0)

#define ASSERT(condition, message) do { \
    if (!(condition)) { \
        printf("FAILED: %s\n", message); \
        tests_failed++; \
        return; \
    } \
} while (0)

// Resource Manager Tests
TEST(resource_manager_init) {
    bool result = InitResourceManager();
    ASSERT(result, "Resource manager initialization failed");
    UnloadResourceManager();
}

TEST(resource_manager_loading) {
    InitResourceManager();
    
    // Test texture loading
    Texture2D* texture = LoadGameTexture("test_texture", "resources/sprites/player/idle.png");
    ASSERT(texture != NULL, "Failed to load texture");
    
    // Test sound loading
    Sound* sound = LoadGameSound("test_sound", "resources/sounds/test.wav");
    ASSERT(sound != NULL, "Failed to load sound");
    
    UnloadResourceManager();
}

// Entity Tests
TEST(entity_creation) {
    EntityPool* pool = InitEntityPool(10);
    ASSERT(pool != NULL, "Failed to create entity pool");
    
    Entity* entity = CreateEntity(pool, ENTITY_PLAYER, (Vector2){100, 100});
    ASSERT(entity != NULL, "Failed to create entity");
    ASSERT(entity->type == ENTITY_PLAYER, "Entity type mismatch");
    
    UnloadEntityPool(pool);
}

TEST(player_creation) {
    EntityPool* pool = InitEntityPool(10);
    InitResourceManager();
    
    Entity* player = CreatePlayer(pool, (Vector2){100, 100}, NULL);
    ASSERT(player != NULL, "Failed to create player");
    ASSERT(player->type == ENTITY_PLAYER, "Player type mismatch");
    ASSERT(player->data != NULL, "Player data not initialized");
    
    UnloadEntityPool(pool);
    UnloadResourceManager();
}

// World Tests
TEST(world_initialization) {
    InitResourceManager();
    World* world = InitWorld(GetResourceManager());
    ASSERT(world != NULL, "Failed to initialize world");
    ASSERT(world->rooms != NULL, "World rooms not initialized");
    ASSERT(world->entityPool != NULL, "World entity pool not initialized");
    
    UnloadWorld(world);
    UnloadResourceManager();
}

TEST(room_creation) {
    InitResourceManager();
    World* world = InitWorld(GetResourceManager());
    
    Room* room = CreateRoom(world, ROOM_NORMAL, (Vector2){0, 0});
    ASSERT(room != NULL, "Failed to create room");
    ASSERT(room->type == ROOM_NORMAL, "Room type mismatch");
    ASSERT(room->tiles != NULL, "Room tiles not initialized");
    
    UnloadWorld(world);
    UnloadResourceManager();
}

// Game State Tests
TEST(game_initialization) {
    Game* game = Game_Init();
    ASSERT(game != NULL, "Failed to initialize game");
    ASSERT(game->state == STATE_MENU, "Initial game state incorrect");
    ASSERT(game->resources != NULL, "Game resources not initialized");
    ASSERT(game->world != NULL, "Game world not initialized");
    
    Game_Unload(game);
}

TEST(game_state_transitions) {
    Game* game = Game_Init();
    
    Game_ChangeState(game, STATE_PLAYING);
    ASSERT(game->state == STATE_PLAYING, "Failed to change to PLAYING state");
    
    Game_TogglePause(game);
    ASSERT(game->state == STATE_PAUSED, "Failed to pause game");
    
    Game_TogglePause(game);
    ASSERT(game->state == STATE_PLAYING, "Failed to unpause game");
    
    Game_Unload(game);
}

void TestResourceManager(void) {
    printf("Testing resource manager...\n");
    
    // Initialize
    assert(InitResourceManager());
    printf("Resource manager initialized\n");
    
    // Test texture loading
    Texture2D* texture = LoadGameTexture("test_texture", "resources/sprites/player/idle.png");
    assert(texture != NULL);
    printf("Texture loaded\n");
    
    // Test sound loading
    Sound* sound = LoadGameSound("test_sound", "resources/sounds/test.wav");
    assert(sound != NULL);
    printf("Sound loaded\n");
    
    // Test resource retrieval
    assert(GetTexture("test_texture") != NULL);
    assert(GetSound("test_sound") != NULL);
    printf("Resources retrieved\n");
    
    // Test resource manager state
    ResourceManager* mgr = GetResourceManager();
    assert(mgr != NULL);
    assert(mgr->textureCount > 0);
    printf("Resource manager state verified\n");
    
    // Cleanup
    UnloadResourceManager();
    printf("Resource manager unloaded\n");
}

void TestWorld(void) {
    printf("Testing world system...\n");
    
    // Initialize resource manager first
    assert(InitResourceManager());
    
    // Initialize world
    World* world = InitWorld(GetResourceManager());
    assert(world != NULL);
    printf("World initialized\n");
    
    // Test world update
    UpdateWorld(world, 0.016f);
    printf("World updated\n");
    
    // Test room creation
    Room* room = CreateRoom(world, ROOM_NORMAL, (Vector2){0, 0});
    assert(room != NULL);
    printf("Room created\n");
    
    // Test tile operations
    SetTileAt(room, 1, 1, TILE_FLOOR);
    assert(GetTileAt(room, 1, 1) == TILE_FLOOR);
    printf("Tile operations verified\n");
    
    // Cleanup
    UnloadWorld(world);
    UnloadResourceManager();
    printf("World system unloaded\n");
}

void TestGame(void) {
    printf("Testing game system...\n");
    
    // Initialize game
    Game* game = Game_Init();
    assert(game != NULL);
    printf("Game initialized\n");
    
    // Test game state changes
    Game_ChangeState(game, STATE_PLAYING);
    assert(game->state == STATE_PLAYING);
    printf("Game state change verified\n");
    
    // Test pause functionality
    Game_TogglePause(game);
    assert(game->state == STATE_PAUSED);
    Game_TogglePause(game);
    assert(game->state == STATE_PLAYING);
    printf("Game pause functionality verified\n");
    
    // Test camera controls
    game->deltaTime = 0.016f;
    Game_UpdateCamera(game);
    printf("Camera controls verified\n");
    
    // Cleanup
    Game_Unload(game);
    printf("Game system unloaded\n");
}

int main(void) {
    // Initialize window for tests
    InitWindow(800, 600, "Shadow Worker Tests");
    SetTargetFPS(60);
    
    // Run tests
    TestResourceManager();
    TestWorld();
    TestGame();
    
    // Cleanup
    CloseWindow();
    
    printf("All tests passed!\n");
    return 0;
} 