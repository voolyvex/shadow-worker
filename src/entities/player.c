#include "../../include/entities/player.h"
#include "../../include/entity.h"
#include "../../include/game.h"
#include "../../include/world.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

#define PLAYER_SPEED 200.0f
#define PLAYER_SIZE 32.0f

// Forward declarations
static void UpdatePlayerInternal(Entity* self, float deltaTime);
static void DrawPlayerInternal(Entity* self);
static void OnPlayerCollision(Entity* self, Entity* other);
static void OnPlayerInteraction(Entity* self, Entity* other);
static bool CheckWallCollision(Entity* player, Vector2 newPosition);

Entity* CreatePlayer(EntityPool* pool, Vector2 position, ResourceManager* resources) {
    if (!pool || !resources) return NULL;

    Entity* player = CreateEntity(pool, ENTITY_PLAYER, position);
    if (!player) return NULL;

    // Set up player properties
    player->scale = 1.0f;
    player->rotation = 0.0f;
    player->bounds = (Rectangle){
        position.x - PLAYER_SIZE/2,
        position.y - PLAYER_SIZE/2,
        PLAYER_SIZE,
        PLAYER_SIZE
    };

    // Initialize player data
    PlayerData* data = (PlayerData*)malloc(sizeof(PlayerData));
    if (!data) {
        // Handle error
        return NULL;
    }

    data->health = 100.0f;
    data->stamina = 100.0f;
    data->resonance = 0.0f;
    data->isInvulnerable = false;
    data->isInteracting = false;
    data->world = NULL;  // Will be set by the game

    player->data = data;
    player->state = ENTITY_IDLE;

    // Set up function pointers
    player->Update = UpdatePlayerInternal;
    player->Draw = DrawPlayerInternal;
    player->OnCollision = OnPlayerCollision;
    player->OnInteraction = OnPlayerInteraction;

    // Load player sprite
    player->sprite = LoadGameTexture("player_idle", "resources/sprites/player/idle.png");

    return player;
}

static bool CheckWallCollision(Entity* player, Vector2 newPosition) {
    if (!player || !((PlayerData*)player->data)->world) return true;
    
    World* world = ((PlayerData*)player->data)->world;
    Room* currentRoom = world->currentRoom;
    if (!currentRoom) return true;
    
    // Calculate player bounds at new position
    Rectangle playerRect = {
        newPosition.x - PLAYER_SIZE/2.0f,
        newPosition.y - PLAYER_SIZE/2.0f,
        PLAYER_SIZE,
        PLAYER_SIZE
    };
    
    // Convert player bounds to tile coordinates
    int startTileX = (int)floorf((playerRect.x) / TILE_SIZE);
    int startTileY = (int)floorf((playerRect.y) / TILE_SIZE);
    int endTileX = (int)ceilf((playerRect.x + playerRect.width) / TILE_SIZE);
    int endTileY = (int)ceilf((playerRect.y + playerRect.height) / TILE_SIZE);
    
    // Check all tiles that the player bounds intersect with
    for (int y = startTileY; y <= endTileY; y++) {
        for (int x = startTileX; x <= endTileX; x++) {
            TileType tile = GetTileAt(currentRoom, x, y);
            if (IsTileSolid(tile)) {
                Rectangle tileRect = {
                    (float)(x * TILE_SIZE),
                    (float)(y * TILE_SIZE),
                    (float)TILE_SIZE,
                    (float)TILE_SIZE
                };
                
                if (CheckCollisionRecs(playerRect, tileRect)) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

static void UpdatePlayerInternal(Entity* entity, float deltaTime) {
    if (!entity || !entity->data) return;

    PlayerData* data = (PlayerData*)entity->data;

    // Handle movement
    Vector2 movement = {0};
    if (IsKeyDown(KEY_W)) movement.y -= 1;
    if (IsKeyDown(KEY_S)) movement.y += 1;
    if (IsKeyDown(KEY_A)) movement.x -= 1;
    if (IsKeyDown(KEY_D)) movement.x += 1;

    // Normalize movement vector if moving diagonally
    if (movement.x != 0 && movement.y != 0) {
        movement = Vector2Normalize(movement);
    }

    // Calculate new position
    Vector2 newPosition = {
        entity->position.x + movement.x * PLAYER_SPEED * deltaTime,
        entity->position.y + movement.y * PLAYER_SPEED * deltaTime
    };
    
    // Check for wall collisions before updating position
    if (!CheckWallCollision(entity, newPosition)) {
        entity->position = newPosition;
        entity->bounds.x = entity->position.x - PLAYER_SIZE/2;
        entity->bounds.y = entity->position.y - PLAYER_SIZE/2;
    }

    // Update state
    if (movement.x != 0 || movement.y != 0) {
        entity->state = ENTITY_MOVING;
    } else {
        entity->state = ENTITY_IDLE;
    }

    // Update resonance
    if (IsKeyPressed(KEY_SPACE)) {
        data->resonance = fminf(data->resonance + 0.1f, 1.0f);
    } else {
        data->resonance = fmaxf(data->resonance - 0.05f * deltaTime, 0.0f);
    }

    // Handle interaction input
    if (IsKeyPressed(KEY_E)) {
        data->isInteracting = true;
    } else {
        data->isInteracting = false;
    }
}

static void DrawPlayerInternal(Entity* entity) {
    if (!entity) return;

    Color color = WHITE;
    if (((PlayerData*)entity->data)->resonance > 0.0f) {
        color = ColorAlpha(SKYBLUE, ((PlayerData*)entity->data)->resonance);
    }

    if (entity->sprite) {
        DrawTexturePro(
            *entity->sprite,
            (Rectangle){0, 0, entity->sprite->width, entity->sprite->height},
            entity->bounds,
            (Vector2){PLAYER_SIZE/2, PLAYER_SIZE/2},
            entity->rotation,
            color
        );
    } else {
        DrawRectanglePro(
            entity->bounds,
            (Vector2){PLAYER_SIZE/2, PLAYER_SIZE/2},
            entity->rotation,
            color
        );
    }
}

void UnloadPlayer(Entity* entity) {
    if (!entity) return;
    if (entity->data) {
        free(entity->data);
        entity->data = NULL;
    }
}

static void OnPlayerCollision(Entity* self, Entity* other) {
    if (!self || !other) return;
    
    // Simple collision resolution - move back
    Vector2 overlap = {
        other->bounds.x + other->bounds.width/2 - (self->bounds.x + self->bounds.width/2),
        other->bounds.y + other->bounds.height/2 - (self->bounds.y + self->bounds.height/2)
    };
    
    float dist = Vector2Length(overlap);
    if (dist > 0) {
        overlap = Vector2Scale(overlap, -1.0f/dist);
        self->position = Vector2Add(self->position, overlap);
        self->bounds.x = self->position.x - self->bounds.width/2;
        self->bounds.y = self->position.y - self->bounds.height/2;
    }
}

static void OnPlayerInteraction(Entity* self, Entity* other) {
    if (!self || !other) return;
    
    // Handle resonance between player and other entity
    HandleResonance(self, other);
} 