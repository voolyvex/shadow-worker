#include "../../include/entity.h"
#include "logger.h"
#include <stdlib.h>

EntityPool* InitEntityPool(int capacity) {
    LOG_INFO(LOG_CORE, "Initializing entity pool with capacity: %d", capacity);
    Logger_BeginTimer("entity_pool_init");
    
    EntityPool* pool = (EntityPool*)malloc(sizeof(EntityPool));
    if (!pool) {
        LOG_ERROR(LOG_CORE, "Failed to allocate memory for entity pool");
        Logger_EndTimer("entity_pool_init");
        return NULL;
    }
    
    pool->entities = (Entity*)malloc(capacity * sizeof(Entity));
    if (!pool->entities) {
        free(pool);
        LOG_ERROR(LOG_CORE, "Failed to allocate memory for entities");
        Logger_EndTimer("entity_pool_init");
        return NULL;
    }
    
    pool->capacity = capacity;
    pool->count = 0;
    
    Logger_EndTimer("entity_pool_init");
    LOG_INFO(LOG_CORE, "Entity pool initialized successfully");
    return pool;
}

void UnloadEntityPool(EntityPool* pool) {
    if (!pool) {
        LOG_WARN(LOG_CORE, "Attempting to unload NULL entity pool");
        return;
    }
    
    LOG_INFO(LOG_CORE, "Unloading entity pool with %d entities", pool->count);
    Logger_BeginTimer("entity_pool_unload");
    
    for (int i = 0; i < pool->count; i++) {
        if (pool->entities[i].data) {
            free(pool->entities[i].data);
        }
    }
    
    free(pool->entities);
    free(pool);
    
    Logger_EndTimer("entity_pool_unload");
    LOG_INFO(LOG_CORE, "Entity pool unloaded successfully");
}

Entity* CreateEntity(EntityPool* pool, EntityType type, Vector2 position) {
    if (!pool) {
        LOG_ERROR(LOG_CORE, "Attempting to create entity with NULL pool");
        return NULL;
    }
    
    if (pool->count >= pool->capacity) {
        LOG_ERROR(LOG_CORE, "Entity pool at capacity (%d), cannot create new entity", pool->capacity);
        return NULL;
    }
    
    Logger_BeginTimer("entity_create");
    
    Entity* entity = &pool->entities[pool->count++];
    entity->type = type;
    entity->position = position;
    entity->rotation = 0.0f;
    entity->scale = 1.0f;
    entity->bounds = (Rectangle){position.x, position.y, 32, 32}; // Default size
    entity->sprite = NULL;
    entity->data = NULL;
    entity->Update = NULL;
    entity->Draw = NULL;
    entity->OnCollision = NULL;
    entity->OnInteraction = NULL;
    
    Logger_EndTimer("entity_create");
    LOG_DEBUG(LOG_CORE, "Created entity of type %d at position (%.2f, %.2f)", type, position.x, position.y);
    
    return entity;
}

void DestroyEntity(EntityPool* pool, Entity* entity) {
    if (!pool || !entity) {
        LOG_WARN(LOG_CORE, "Attempting to destroy invalid entity or pool");
        return;
    }
    
    int index = (int)(entity - pool->entities);
    if (index < 0 || index >= pool->count) {
        LOG_ERROR(LOG_CORE, "Invalid entity index %d (count: %d)", index, pool->count);
        return;
    }
    
    Logger_BeginTimer("entity_destroy");
    
    if (entity->data) {
        free(entity->data);
    }
    
    // Move last entity to this position if it's not the last one
    if (index < pool->count - 1) {
        pool->entities[index] = pool->entities[pool->count - 1];
        LOG_DEBUG(LOG_CORE, "Moved entity from position %d to %d", pool->count - 1, index);
    }
    
    pool->count--;
    
    Logger_EndTimer("entity_destroy");
    LOG_DEBUG(LOG_CORE, "Destroyed entity at index %d, new count: %d", index, pool->count);
}

void UpdateEntities(EntityPool* pool, float deltaTime) {
    if (!pool) {
        LOG_WARN(LOG_CORE, "Attempting to update NULL entity pool");
        return;
    }
    
    Logger_BeginTimer("entities_update");
    
    for (int i = 0; i < pool->count; i++) {
        Entity* entity = &pool->entities[i];
        if (entity->Update) {
            entity->Update(entity, deltaTime);
        }
    }
    
    Logger_EndTimer("entities_update");
    LOG_TRACE(LOG_CORE, "Updated %d entities (dt: %.4f)", pool->count, deltaTime);
}

void DrawEntities(EntityPool* pool) {
    if (!pool) {
        LOG_WARN(LOG_CORE, "Attempting to draw NULL entity pool");
        return;
    }
    
    Logger_BeginTimer("entities_draw");
    
    for (int i = 0; i < pool->count; i++) {
        Entity* entity = &pool->entities[i];
        if (entity->Draw) {
            entity->Draw(entity);
        }
    }
    
    Logger_EndTimer("entities_draw");
    LOG_TRACE(LOG_CORE, "Drew %d entities", pool->count);
}

Entity* GetEntityByType(EntityPool* pool, EntityType type) {
    if (!pool) {
        LOG_WARN(LOG_CORE, "Attempting to get entity from NULL pool");
        return NULL;
    }
    
    Logger_BeginTimer("get_entity_by_type");
    
    for (int i = 0; i < pool->count; i++) {
        if (pool->entities[i].type == type) {
            Logger_EndTimer("get_entity_by_type");
            LOG_DEBUG(LOG_CORE, "Found entity of type %d at index %d", type, i);
            return &pool->entities[i];
        }
    }
    
    Logger_EndTimer("get_entity_by_type");
    LOG_DEBUG(LOG_CORE, "No entity found of type %d", type);
    return NULL;
} 