#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/warning_suppression.h"
#include "../include/entity.h"
#include "../include/entity_pool.h"
#include "../include/world.h"
#include "../include/constants.h"
#include "../include/entity_types.h"

BEGIN_EXTERNAL_WARNINGS

// Memory alignment for entity pool
#define POOL_MEMORY_ALIGNMENT 16
#define MAX_POOL_SIZE 10000

// Add size type safety
#define SAFE_SIZE_T(x) ((size_t)((x) > SIZE_MAX ? SIZE_MAX : (x)))
#define SAFE_ARRAY_INDEX(x, max) ((size_t)((x) >= (max) ? ((max) - 1) : (x)))

// Internal helper functions
static size_t FindFreeSlot(EntityPool* pool);
static void InitializePool(EntityPool* pool, size_t capacity);
static float GetDistanceBetweenPoints(Vector2 a, Vector2 b);

// Function declarations with proper parameter lists
EntityPool* CreateEntityPool(size_t initialCapacity);
void DestroyEntityPool(EntityPool* pool);
Entity* CreateEntity(EntityPool* pool, EntityType type, Vector2 position);
void RemoveEntity(EntityPool* pool, Entity* entity);
void UpdateEntityPool(EntityPool* pool, World* world, float deltaTime);
void DrawEntityPool(EntityPool* pool);

// Helper function for aligned memory allocation
static void* AlignedAlloc(size_t size, size_t alignment) {
    void* ptr = NULL;
#ifdef _WIN32
    ptr = _aligned_malloc(SAFE_SIZE_T(size), alignment);
#else
    if (posix_memalign(&ptr, alignment, SAFE_SIZE_T(size)) != 0) {
        ptr = NULL;
    }
#endif
    return ptr;
}

static void AlignedFree(void* ptr) {
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

EntityPool* CreateEntityPool(size_t initialCapacity) {
    EntityPool* pool = (EntityPool*)calloc(1, sizeof(EntityPool));
    if (!pool) return NULL;
    
    InitializePool(pool, initialCapacity);
    if (!pool->entities || !pool->active) {
        DestroyEntityPool(pool);
        return NULL;
    }
    
    return pool;
}

void DestroyEntityPool(EntityPool* pool) {
    if (!pool) return;
    
    if (pool->entities) {
        for (size_t i = 0; i < pool->count; i++) {
            if (pool->entities[i].OnDestroy) {
                pool->entities[i].OnDestroy(&pool->entities[i]);
            }
        }
        free(pool->entities);
    }
    
    free(pool);
}

Entity* CreateEntity(EntityPool* pool, EntityType type, Vector2 position) {
    if (!pool || pool->count >= pool->capacity) return NULL;
    
    Entity* entity = &pool->entities[pool->count++];
    memset(entity, 0, sizeof(Entity));
    
    entity->type = type;
    entity->position = position;
    entity->active = true;
    entity->visible = true;
    entity->scale = 1.0f;
    entity->color = WHITE;
    
    // Set up collision bounds
    entity->bounds = (Rectangle){
        position.x,
        position.y,
        NPC_WIDTH,
        NPC_HEIGHT
    };
    
    entity->collider = entity->bounds;
    
    // Initialize based on type
    switch (type) {
        case ENTITY_TYPE_PLAYER:
            AddComponent(entity, COMPONENT_TRANSFORM | COMPONENT_PHYSICS | COMPONENT_PLAYER_CONTROL);
            entity->components_data[COMPONENT_PLAYER_CONTROL].playerControl.moveSpeed = PLAYER_SPEED;
            break;
            
        case ENTITY_TYPE_NPC:
            AddComponent(entity, COMPONENT_TRANSFORM | COMPONENT_PHYSICS | COMPONENT_AI);
            entity->components_data[COMPONENT_AI].ai.state = ENTITY_STATE_IDLE;
            entity->components_data[COMPONENT_AI].ai.detectionRadius = NPC_DETECTION_RADIUS;
            entity->components_data[COMPONENT_AI].ai.homePosition = position;
            break;
            
        case ENTITY_TYPE_OBJECT:
            AddComponent(entity, COMPONENT_TRANSFORM | COMPONENT_COLLIDER);
            entity->components_data[COMPONENT_COLLIDER].collider.isStatic = true;
            break;
            
        default:
            break;
    }
    
    return entity;
}

void RemoveEntity(EntityPool* pool, Entity* entity) {
    if (!pool || !entity || pool->count == 0) return;
    
    // Find the entity in the pool
    size_t index = entity - pool->entities;
    if (index >= pool->count) return;
    
    // Call destroy callback if it exists
    if (entity->OnDestroy) {
        entity->OnDestroy(entity);
    }
    
    // Move the last entity to this position if it's not the last one
    if (index < pool->count - 1) {
        pool->entities[index] = pool->entities[pool->count - 1];
    }
    
    pool->count--;
}

void UpdateEntityPool(EntityPool* pool, World* world, float deltaTime) {
    if (!pool || !world) return;
    
    for (size_t i = 0; i < pool->count; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->active || !entity->Update) continue;
        
        entity->Update(entity, world, deltaTime);
    }
}

void DrawEntityPool(EntityPool* pool) {
    if (!pool) return;
    
    for (size_t i = 0; i < pool->count; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->active || !entity->visible || !entity->Draw) continue;
        
        entity->Draw(entity);
    }
}

Entity* GetEntityAt(EntityPool* pool, Vector2 position, float radius) {
    if (!pool) return NULL;
    
    for (size_t i = 0; i < pool->count; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->active) continue;
        
        if (Vector2Distance(entity->position, position) <= radius) {
            return entity;
        }
    }
    
    return NULL;
}

Entity* GetEntityByType(EntityPool* pool, EntityType type) {
    if (!pool) return NULL;
    
    for (size_t i = 0; i < pool->count; i++) {
        if (pool->entities[i].active && pool->entities[i].type == type) {
            return &pool->entities[i];
        }
    }
    
    return NULL;
}

void UnloadEntity(Entity* entity) {
    if (!entity) return;
    
    // Unload any resources based on type
    switch (entity->type) {
        case ENTITY_TYPE_PLAYER:
            // Unload player-specific resources
            break;
            
        case ENTITY_TYPE_NPC:
            // Unload NPC-specific resources
            break;
            
        case ENTITY_TYPE_OBJECT:
            // Unload object-specific resources
            break;
            
        default:
            break;
    }
    
    // Reset entity data
    memset(entity, 0, sizeof(Entity));
}

// Helper functions
static void UpdateObject(Entity* entity, World* world, float deltaTime) {
    if (!entity || !world) return;
    
    // Update object based on type
    switch (entity->state) {
        case ENTITY_STATE_IDLE:
            // Check for interactions
            break;
            
        case ENTITY_STATE_INTERACT:
            // Update active state
            break;
            
        default:
            break;
    }
}

static void DrawObject(Entity* entity) {
    if (!entity) return;
    
    // Draw the object based on its type and state
    Color tint = WHITE;
    if (entity->state == ENTITY_STATE_INTERACT) {
        tint = YELLOW;
    }
    
    DrawRectangleV(entity->position, (Vector2){TILE_SIZE, TILE_SIZE}, tint);
}

static void DrawNPC(Entity* entity) {
    if (!entity) return;
    
    // Draw the NPC based on its state
    Color tint = WHITE;
    switch (entity->state) {
        case ENTITY_STATE_CHASE:
            tint = RED;
            break;
        case ENTITY_STATE_FLEE:
            tint = GREEN;
            break;
        default:
            break;
    }
    
    DrawRectangleV(entity->position, (Vector2){TILE_SIZE, TILE_SIZE}, tint);
}

static void DrawPlayer(Entity* entity) {
    if (!entity) return;
    
    // Draw the player
    DrawRectangleV(entity->position, (Vector2){TILE_SIZE, TILE_SIZE}, BLUE);
}

// Internal helper function implementations
static size_t FindFreeSlot(EntityPool* pool) {
    for (size_t i = 0; i < pool->capacity; i++) {
        if (!pool->active[i]) return i;
    }
    return SIZE_MAX;
}

static void InitializePool(EntityPool* pool, size_t capacity) {
    pool->capacity = capacity;
    pool->count = 0;
    pool->status = POOL_OK;
    
    pool->entities = (Entity*)calloc(capacity, sizeof(Entity));
    pool->active = (bool*)calloc(capacity, sizeof(bool));
}

static float GetDistanceBetweenPoints(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

Entity* GetNearestEntity(EntityPool* pool, Vector2 position, float maxDistance) {
    if (!pool) return NULL;

    Entity* nearest = NULL;
    float minDistance = maxDistance;

    for (size_t i = 0; i < pool->capacity; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->active || !HasComponent(entity, COMPONENT_TRANSFORM)) continue;

        TransformComponent* transform = &entity->components_data[COMPONENT_TRANSFORM].transform;
        float distance = Vector2Distance(transform->position, position);

        if (distance < minDistance) {
            minDistance = distance;
            nearest = entity;
        }
    }

    return nearest;
}

Entity* GetEntityAtPoint(EntityPool* pool, Vector2 point) {
    if (!pool) return NULL;

    for (size_t i = 0; i < pool->capacity; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->active || !HasComponent(entity, COMPONENT_COLLIDER)) continue;

        ColliderComponent* collider = &entity->components_data[COMPONENT_COLLIDER].collider;
        if (CheckCollisionPointRec(point, collider->bounds)) {
            return entity;
        }
    }

    return NULL;
}

PoolStatus GrowPool(EntityPool* pool) {
    if (!pool) return POOL_INVALID_ENTITY;
    if (pool->capacity >= MAX_POOL_SIZE) return POOL_FULL;

    size_t newCapacity = pool->capacity * POOL_GROWTH_FACTOR;
    if (newCapacity > MAX_POOL_SIZE) newCapacity = MAX_POOL_SIZE;

    // Reallocate entities array with alignment
    Entity* newEntities = (Entity*)AlignedAlloc(newCapacity * sizeof(Entity), POOL_MEMORY_ALIGNMENT);
    if (!newEntities) return POOL_OUT_OF_MEMORY;

    // Reallocate active flags array
    bool* newActive = (bool*)malloc(newCapacity * sizeof(bool));
    if (!newActive) {
        AlignedFree(newEntities);
        return POOL_OUT_OF_MEMORY;
    }

    // Copy existing data
    memcpy(newEntities, pool->entities, pool->capacity * sizeof(Entity));
    memcpy(newActive, pool->active, pool->capacity * sizeof(bool));

    // Initialize new memory
    memset(newEntities + pool->capacity, 0, (newCapacity - pool->capacity) * sizeof(Entity));
    memset(newActive + pool->capacity, 0, (newCapacity - pool->capacity) * sizeof(bool));

    // Free old arrays
    AlignedFree(pool->entities);
    free(pool->active);

    // Update pool
    pool->entities = newEntities;
    pool->active = newActive;
    pool->capacity = newCapacity;

    return POOL_OK;
}

void CompactPool(EntityPool* pool) {
    if (!pool) return;

    size_t write = 0;
    for (size_t read = 0; read < pool->capacity; read++) {
        if (pool->active[read]) {
            if (write != read) {
                pool->entities[write] = pool->entities[read];
                pool->active[write] = true;
                pool->active[read] = false;
            }
            write++;
        }
    }
}

void ClearPool(EntityPool* pool) {
    if (!pool) return;

    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i]) {
            DestroyEntity(&pool->entities[i]);
            pool->active[i] = false;
        }
    }

    pool->count = 0;
    pool->status = POOL_OK;
}

size_t GetActiveCount(EntityPool* pool) {
    return pool ? pool->count : 0;
}

float GetPoolUtilization(EntityPool* pool) {
    if (!pool || pool->capacity == 0) return 0.0f;
    return (float)pool->count / (float)pool->capacity;
}

Entity** GetEntitiesInRadius(EntityPool* pool, Vector2 center, float radius, size_t* count) {
    if (!pool || !count) return NULL;
    *count = 0;

    // First pass: count matching entities
    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i]) {
            Entity* entity = &pool->entities[i];
            TransformComponent* transform = GetTransformComponent(entity);
            if (transform) {
                float dist = Vector2Distance(center, transform->position);
                if (dist <= radius) {
                    (*count)++;
                }
            }
        }
    }

    if (*count == 0) return NULL;

    // Allocate result array
    Entity** result = (Entity**)malloc(*count * sizeof(Entity*));
    if (!result) {
        *count = 0;
        return NULL;
    }

    // Second pass: fill array
    size_t index = 0;
    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i]) {
            Entity* entity = &pool->entities[i];
            TransformComponent* transform = GetTransformComponent(entity);
            if (transform) {
                float dist = Vector2Distance(center, transform->position);
                if (dist <= radius) {
                    result[index++] = entity;
                }
            }
        }
    }

    return result;
}

Entity** GetEntitiesByType(EntityPool* pool, EntityType type, size_t* count) {
    if (!pool || !count) return NULL;
    *count = 0;

    // First pass: count matching entities
    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && pool->entities[i].type == type) {
            (*count)++;
        }
    }

    if (*count == 0) return NULL;

    // Allocate result array
    Entity** result = (Entity**)malloc(*count * sizeof(Entity*));
    if (!result) {
        *count = 0;
        return NULL;
    }

    // Second pass: fill array
    size_t index = 0;
    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && pool->entities[i].type == type) {
            result[index++] = &pool->entities[i];
        }
    }

    return result;
}

Entity** GetCollidingEntities(EntityPool* pool, Rectangle bounds, size_t* count) {
    if (!pool || !count) return NULL;
    *count = 0;

    // Count colliding entities first
    size_t maxCount = 0;
    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && CheckCollisionRecs(bounds, pool->entities[i].collider)) {
            maxCount++;
        }
    }

    if (maxCount == 0) return NULL;

    // Allocate array for results
    Entity** results = (Entity**)malloc(maxCount * sizeof(Entity*));
    if (!results) return NULL;

    // Fill array with colliding entities
    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && CheckCollisionRecs(bounds, pool->entities[i].collider)) {
            results[*count] = &pool->entities[i];
            (*count)++;
        }
    }

    return results;
}

bool CheckEntityCollision(Entity* entity1, Entity* entity2) {
    if (!entity1 || !entity2) return false;
    return CheckCollisionRecs(entity1->collider, entity2->collider);
}

void HandleCollisions(EntityPool* pool) {
    if (!pool) return;

    for (size_t i = 0; i < pool->capacity; i++) {
        Entity* entity1 = &pool->entities[i];
        if (!entity1->active || !HasComponent(entity1, COMPONENT_COLLIDER)) continue;

        for (size_t j = i + 1; j < pool->capacity; j++) {
            Entity* entity2 = &pool->entities[j];
            if (!entity2->active || !HasComponent(entity2, COMPONENT_COLLIDER)) continue;

            if (CheckEntityCollision(entity1, entity2)) {
                // Handle collision response
                ColliderComponent* collider1 = &entity1->components_data[COMPONENT_COLLIDER].collider;
                ColliderComponent* collider2 = &entity2->components_data[COMPONENT_COLLIDER].collider;

                if (!collider1->isStatic && !collider2->isStatic) {
                    // Both entities are dynamic, split the response
                    TransformComponent* transform1 = &entity1->components_data[COMPONENT_TRANSFORM].transform;
                    TransformComponent* transform2 = &entity2->components_data[COMPONENT_TRANSFORM].transform;

                    // Calculate overlap and adjust positions
                    float dx = (collider1->bounds.x + collider1->bounds.width/2) - (collider2->bounds.x + collider2->bounds.width/2);
                    float dy = (collider1->bounds.y + collider1->bounds.height/2) - (collider2->bounds.y + collider2->bounds.height/2);
                    float overlapX = (collider1->bounds.width + collider2->bounds.width)/2 - fabsf(dx);
                    float overlapY = (collider1->bounds.height + collider2->bounds.height)/2 - fabsf(dy);

                    if (overlapX > 0 && overlapY > 0) {
                        if (overlapX < overlapY) {
                            transform1->position.x += (dx > 0 ? overlapX/2 : -overlapX/2);
                            transform2->position.x += (dx > 0 ? -overlapX/2 : overlapX/2);
                        } else {
                            transform1->position.y += (dy > 0 ? overlapY/2 : -overlapY/2);
                            transform2->position.y += (dy > 0 ? -overlapY/2 : overlapY/2);
                        }
                    }
                }
            }
        }
    }
}

// Safe type conversions
static float IntToFloat(int value) {
    return (float)value;
}

static int FloatToInt(float value) {
    return (int)(value + 0.5f); // Round to nearest integer
}

END_EXTERNAL_WARNINGS 