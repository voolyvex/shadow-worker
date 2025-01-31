#ifndef SHADOW_WORKER_ENTITY_H
#define SHADOW_WORKER_ENTITY_H

#include <raylib.h>
#include <stdbool.h>

// Forward declarations
typedef struct EntityPool EntityPool;

// Entity types
typedef enum EntityType {
    ENTITY_NONE,
    ENTITY_PLAYER,
    ENTITY_NPC,
    ENTITY_ITEM,
    ENTITY_OBSTACLE
} EntityType;

// Entity structure
typedef struct Entity {
    EntityType type;
    Vector2 position;
    float rotation;
    float scale;
    Rectangle bounds;
    Texture2D* sprite;
    void* data;  // Type-specific data
    
    // Function pointers for entity behavior
    void (*Update)(struct Entity* self, float deltaTime);
    void (*Draw)(struct Entity* self);
    void (*OnCollision)(struct Entity* self, struct Entity* other);
    void (*OnInteraction)(struct Entity* self, struct Entity* other);
} Entity;

// Entity pool for managing multiple entities
typedef struct EntityPool {
    Entity* entities;
    int capacity;
    int count;
} EntityPool;

// Function declarations
EntityPool* InitEntityPool(int capacity);
void UnloadEntityPool(EntityPool* pool);
Entity* CreateEntity(EntityPool* pool, EntityType type, Vector2 position);
void DestroyEntity(EntityPool* pool, Entity* entity);
void UpdateEntities(EntityPool* pool, float deltaTime);
void DrawEntities(EntityPool* pool);

#endif // SHADOW_WORKER_ENTITY_H 