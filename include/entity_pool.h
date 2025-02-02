#ifndef ENTITY_POOL_H
#define ENTITY_POOL_H

#include <raylib.h>
#include <stdbool.h>
#include "entity_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct World;

// Pool configuration
#define INITIAL_POOL_SIZE 128
#define MAX_POOL_SIZE 1024
#define POOL_GROWTH_FACTOR 2

// Pool status flags
typedef enum {
    POOL_OK = 0,
    POOL_FULL = 1,
    POOL_INVALID_ENTITY = 2,
    POOL_OUT_OF_MEMORY = 3
} PoolStatus;

// Entity pool structure
typedef struct EntityPool {
    Entity* entities;
    bool* active;
    int capacity;
    int count;
    PoolStatus status;
} EntityPool;

// Core pool functions
EntityPool* CreateEntityPool(void);
void DestroyEntityPool(EntityPool* pool);
void UpdateEntityPool(EntityPool* pool, struct World* world, float deltaTime);
void DrawEntityPool(EntityPool* pool);

// Entity management
Entity* SpawnEntity(EntityPool* pool, EntityType type, Vector2 position);
void RemoveEntity(EntityPool* pool, Entity* entity);
Entity* GetEntityByType(EntityPool* pool, EntityType type);
Entity* GetEntityAtPosition(EntityPool* pool, Vector2 position);

// Pool maintenance
PoolStatus GrowPool(EntityPool* pool);
void CompactPool(EntityPool* pool);
void ClearPool(EntityPool* pool);
int GetActiveCount(EntityPool* pool);
float GetPoolUtilization(EntityPool* pool);

// Query functions
Entity** GetEntitiesInRadius(EntityPool* pool, Vector2 center, float radius, int* count);
Entity** GetEntitiesByType(EntityPool* pool, EntityType type, int* count);
Entity** GetCollidingEntities(EntityPool* pool, Rectangle bounds, int* count);

#ifdef __cplusplus
}
#endif

#endif // ENTITY_POOL_H 