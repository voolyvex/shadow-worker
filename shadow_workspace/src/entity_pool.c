#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/entity.h"
#include "../include/entity_pool.h"
#include "../include/world.h"

// Add size type safety
#define SAFE_SIZE_T(x) ((x) > SIZE_MAX ? SIZE_MAX : (x))
#define SAFE_ARRAY_INDEX(x, max) ((x) >= (max) ? ((max) - 1) : (x))

// Internal helper functions
static int FindFreeSlot(EntityPool* pool);
static void InitializePool(EntityPool* pool, int capacity);
static float GetDistanceBetweenPoints(Vector2 a, Vector2 b);

EntityPool* CreateEntityPool(void) {
    EntityPool* pool = (EntityPool*)malloc(sizeof(EntityPool));
    if (!pool) return NULL;

    // Allocate aligned memory for entities
    pool->entities = (Entity*)AlignedAlloc(
        SAFE_SIZE_T(INITIAL_POOL_SIZE * sizeof(Entity)), 
        POOL_MEMORY_ALIGNMENT
    );
    if (!pool->entities) {
        free(pool);
        return NULL;
    }

    // Allocate active flags array with boundary check
    pool->active = (bool*)malloc(SAFE_SIZE_T(INITIAL_POOL_SIZE * sizeof(bool)));
    if (!pool->active) {
        AlignedFree(pool->entities);
        free(pool);
        return NULL;
    }

    // Create component registry
    pool->registry = CreateComponentRegistry();
    if (!pool->registry) {
        AlignedFree(pool->entities);
        free(pool->active);
        free(pool);
        return NULL;
    }

    // Initialize pool properties with safe size types
    pool->capacity = INITIAL_POOL_SIZE;
    pool->count = 0;
    pool->firstFree = 0;
    pool->status = POOL_OK;
    pool->world = NULL;

    // Initialize arrays with boundary checks
    memset(pool->entities, 0, SAFE_SIZE_T(INITIAL_POOL_SIZE * sizeof(Entity)));
    memset(pool->active, 0, SAFE_SIZE_T(INITIAL_POOL_SIZE * sizeof(bool)));

    return pool;
}

void DestroyEntityPool(EntityPool* pool) {
    if (!pool) return;

    // Free all entities and their components
    for (size_t i = 0; i < pool->count; i++) {
        if (pool->active[i] && pool->entities[i].OnDestroy) {
            pool->entities[i].OnDestroy(&pool->entities[i]);
        }
    }

    // Free component registry
    DestroyComponentRegistry(pool->registry);

    // Free aligned memory
    AlignedFree(pool->entities);
    free(pool->active);
    free(pool);
}

void UpdateEntityPool(EntityPool* pool, struct World* world, float deltaTime) {
    if (!pool) return;

    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && pool->entities[i].Update) {
            pool->entities[i].Update(&pool->entities[i], world, deltaTime);
        }
    }
}

void DrawEntityPool(EntityPool* pool) {
    if (!pool) return;

    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && pool->entities[i].visible && pool->entities[i].Draw) {
            pool->entities[i].Draw(&pool->entities[i]);
        }
    }
}

Entity* SpawnEntity(EntityPool* pool, EntityType type, Vector2 position) {
    if (!pool) return NULL;

    // Find first free slot
    size_t entityId = pool->firstFree;
    while (entityId < pool->capacity && pool->active[entityId]) {
        entityId++;
    }

    // If no free slots, try to grow pool
    if (entityId >= pool->capacity) {
        if (GrowPool(pool) != POOL_OK) {
            pool->status = POOL_FULL;
            return NULL;
        }
        entityId = pool->firstFree;
    }

    // Initialize entity
    Entity* entity = &pool->entities[entityId];
    memset(entity, 0, sizeof(Entity));
    
    entity->type = type;
    entity->position = position;
    entity->active = true;
    entity->visible = true;
    entity->scale = 1.0f;
    entity->color = WHITE;

    // Mark slot as active
    pool->active[entityId] = true;
    pool->count++;

    // Update first free slot
    while (pool->firstFree < pool->capacity && pool->active[pool->firstFree]) {
        pool->firstFree++;
    }

    return entity;
}

void RemoveEntity(EntityPool* pool, Entity* entity) {
    if (!pool || !entity) return;

    // Find entity index
    size_t entityId = entity - pool->entities;
    if (entityId >= pool->capacity || !pool->active[entityId]) return;

    // Call destroy callback if it exists
    if (entity->OnDestroy) {
        entity->OnDestroy(entity);
    }

    // Remove components from registry
    ComponentFlags flag = 1;
    for (int i = 0; i < MAX_COMPONENT_TYPES; i++, flag <<= 1) {
        if (entity->components & flag) {
            RemoveComponentFromRegistry(pool->registry, i, entityId);
        }
    }

    // Clear entity data
    memset(entity, 0, sizeof(Entity));
    pool->active[entityId] = false;
    pool->count--;

    // Update first free slot if needed
    if (entityId < pool->firstFree) {
        pool->firstFree = entityId;
    }
}

Entity* GetEntityByType(EntityPool* pool, EntityType type) {
    if (!pool) return NULL;

    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && pool->entities[i].type == type) {
            return &pool->entities[i];
        }
    }

    return NULL;
}

Entity* GetEntityAtPosition(EntityPool* pool, Vector2 position) {
    if (!pool) return NULL;

    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && 
            CheckCollisionPointRec(position, pool->entities[i].bounds)) {
            return &pool->entities[i];
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

    int write = 0;
    for (int read = 0; read < pool->capacity; read++) {
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

    for (int i = 0; i < pool->capacity; i++) {
        if (pool->active[i]) {
            DestroyEntity(&pool->entities[i]);
            pool->active[i] = false;
        }
    }

    pool->count = 0;
    pool->status = POOL_OK;
}

int GetActiveCount(EntityPool* pool) {
    return pool ? pool->count : 0;
}

float GetPoolUtilization(EntityPool* pool) {
    if (!pool || pool->capacity == 0) return 0.0f;
    return (float)pool->count / (float)pool->capacity;
}

Entity** GetEntitiesInRadius(EntityPool* pool, Vector2 center, float radius, int* count) {
    if (!pool || !count) return NULL;
    *count = 0;

    // First pass: count matching entities
    for (int i = 0; i < pool->capacity; i++) {
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
    int index = 0;
    for (int i = 0; i < pool->capacity; i++) {
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

Entity** GetEntitiesByType(EntityPool* pool, EntityType type, int* count) {
    if (!pool || !count) return NULL;
    *count = 0;

    // First pass: count matching entities
    for (int i = 0; i < pool->capacity; i++) {
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
    int index = 0;
    for (int i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && pool->entities[i].type == type) {
            result[index++] = &pool->entities[i];
        }
    }

    return result;
}

Entity** GetCollidingEntities(EntityPool* pool, Rectangle bounds, int* count) {
    if (!pool || !count) return NULL;
    *count = 0;

    // First pass: count colliding entities
    for (int i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && HasComponent(&pool->entities[i], COMPONENT_COLLIDER)) {
            if (CheckCollisionRecs(bounds, pool->entities[i].collider.bounds)) {
                (*count)++;
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
    int index = 0;
    for (int i = 0; i < pool->capacity; i++) {
        if (pool->active[i] && HasComponent(&pool->entities[i], COMPONENT_COLLIDER)) {
            if (CheckCollisionRecs(bounds, pool->entities[i].collider.bounds)) {
                result[index++] = &pool->entities[i];
            }
        }
    }

    return result;
}

bool CheckEntityCollision(Entity* entity1, Entity* entity2) {
    if (!entity1 || !entity2 || !entity1->active || !entity2->active) return false;
    if (!HasComponent(entity1, COMPONENT_COLLIDER) || !HasComponent(entity2, COMPONENT_COLLIDER)) return false;

    ColliderComponent* collider1 = &entity1->components_data[COMPONENT_COLLIDER].collider;
    ColliderComponent* collider2 = &entity2->components_data[COMPONENT_COLLIDER].collider;

    return CheckCollisionRecs(collider1->bounds, collider2->bounds);
}

void HandleCollisions(EntityPool* pool) {
    if (!pool) return;

    for (int i = 0; i < MAX_ENTITIES; i++) {
        Entity* entity1 = &pool->entities[i];
        if (!entity1->active || !HasComponent(entity1, COMPONENT_COLLIDER)) continue;

        for (int j = i + 1; j < MAX_ENTITIES; j++) {
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

// Internal helper function implementations
static int FindFreeSlot(EntityPool* pool) {
    for (int i = 0; i < pool->capacity; i++) {
        if (!pool->active[i]) return i;
    }
    return -1;
}

static void InitializePool(EntityPool* pool, int capacity) {
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

Entity* CreateEntity(EntityPool* pool) {
    if (!pool) return NULL;

    for (int i = 0; i < MAX_ENTITIES; i++) {
        if (!pool->entities[i].active) {
            InitEntity(&pool->entities[i]);
            pool->entities[i].active = true;
            return &pool->entities[i];
        }
    }
    return NULL;
}

Entity* GetNearestEntity(EntityPool* pool, Vector2 position, float maxDistance) {
    if (!pool) return NULL;

    Entity* nearest = NULL;
    float minDistance = maxDistance;

    for (int i = 0; i < MAX_ENTITIES; i++) {
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

    for (int i = 0; i < MAX_ENTITIES; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->active || !HasComponent(entity, COMPONENT_COLLIDER)) continue;

        ColliderComponent* collider = &entity->components_data[COMPONENT_COLLIDER].collider;
        if (CheckCollisionPointRec(point, collider->bounds)) {
            return entity;
        }
    }

    return NULL;
} 