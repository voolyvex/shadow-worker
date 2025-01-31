#include "../../include/entity.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <raymath.h>  // For Vector2Distance

EntityPool* InitEntityPool(int capacity) {
    EntityPool* pool = (EntityPool*)malloc(sizeof(EntityPool));
    if (!pool) return NULL;

    pool->entities = (Entity*)malloc(capacity * sizeof(Entity));
    if (!pool->entities) {
        free(pool);
        return NULL;
    }

    pool->capacity = capacity;
    pool->count = 0;

    // Initialize all entities as inactive
    for (int i = 0; i < capacity; i++) {
        pool->entities[i].isActive = false;
    }

    return pool;
}

void UnloadEntityPool(EntityPool* pool) {
    if (!pool) return;

    if (pool->entities) {
        // Clean up any active entities
        for (int i = 0; i < pool->count; i++) {
            if (pool->entities[i].isActive && pool->entities[i].data) {
                free(pool->entities[i].data);
            }
        }
        free(pool->entities);
    }

    free(pool);
}

Entity* CreateEntity(EntityPool* pool, EntityType type, Vector2 position) {
    if (!pool || pool->count >= pool->capacity) return NULL;

    // Find first inactive slot
    int index = -1;
    for (int i = 0; i < pool->capacity; i++) {
        if (!pool->entities[i].isActive) {
            index = i;
            break;
        }
    }

    if (index == -1) return NULL;

    Entity* entity = &pool->entities[index];
    memset(entity, 0, sizeof(Entity));

    entity->type = type;
    entity->position = position;
    entity->velocity = (Vector2){0, 0};
    entity->rotation = 0.0f;
    entity->scale = 1.0f;
    entity->bounds = (Rectangle){position.x, position.y, 32, 32}; // Default size
    entity->isActive = true;
    entity->data = NULL;

    if (index >= pool->count) {
        pool->count = index + 1;
    }

    return entity;
}

void DestroyEntity(EntityPool* pool, Entity* entity) {
    if (!pool || !entity) return;
    
    int index = (int)(entity - pool->entities);
    if (index < 0 || index >= pool->count) return;
    
    // Free entity resources
    if (entity->psyche) {
        free(entity->psyche);
    }
    if (entity->data) {
        free(entity->data);
    }
    
    // Move last entity to this slot if it's not the last one
    if (index < pool->count - 1) {
        memcpy(&pool->entities[index], &pool->entities[pool->count - 1], sizeof(Entity));
    }
    
    pool->count--;
}

void UpdateEntities(EntityPool* pool, float deltaTime) {
    if (!pool) return;

    for (int i = 0; i < pool->count; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->isActive) continue;

        // Update position based on velocity
        entity->position.x += entity->velocity.x * deltaTime;
        entity->position.y += entity->velocity.y * deltaTime;

        // Update bounds
        entity->bounds.x = entity->position.x;
        entity->bounds.y = entity->position.y;

        // Call entity's update function if it exists
        if (entity->Update) {
            entity->Update(entity, deltaTime);
        }
    }
}

void DrawEntities(EntityPool* pool) {
    if (!pool) return;

    for (int i = 0; i < pool->count; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->isActive) continue;

        // Draw a simple rectangle for now
        DrawRectanglePro(
            entity->bounds,
            (Vector2){entity->bounds.width/2, entity->bounds.height/2},
            entity->rotation,
            WHITE
        );
    }
}

bool CheckEntityCollision(Entity* e1, Entity* e2) {
    if (!e1 || !e2) return false;
    return CheckCollisionRecs(e1->bounds, e2->bounds);
}

void HandleEntityCollision(Entity* e1, Entity* e2) {
    if (!e1 || !e2) return;
    
    if (e1->OnCollision) e1->OnCollision(e1, e2);
    if (e2->OnCollision) e2->OnCollision(e2, e1);
}

void ProcessEntityInteractions(EntityPool* pool) {
    if (!pool) return;

    // Check for collisions between all active entities
    for (int i = 0; i < pool->count; i++) {
        Entity* e1 = &pool->entities[i];
        if (!e1->isActive) continue;

        for (int j = i + 1; j < pool->count; j++) {
            Entity* e2 = &pool->entities[j];
            if (!e2->isActive) continue;

            // Check for collision
            if (CheckCollisionRecs(e1->bounds, e2->bounds)) {
                // Handle collision based on entity types
                switch (e1->type) {
                    case ENTITY_PLAYER:
                        // Handle player collision with other entities
                        break;
                    case ENTITY_NPC:
                        // Handle NPC collision
                        break;
                    case ENTITY_ITEM:
                        // Handle item collision
                        break;
                    case ENTITY_OBSTACLE:
                        // Handle obstacle collision
                        break;
                }
            }
        }
    }
}

void UpdatePsychologicalState(Entity* entity, float deltaTime) {
    if (!entity || !entity->psyche) return;
    
    // Apply natural decay to resonance
    entity->psyche->resonanceLevel = (float)fmax(0.0f, 
        entity->psyche->resonanceLevel - (0.1f * deltaTime));
    
    // Update consciousness based on shadow intensity
    if (entity->psyche->isResonating) {
        entity->psyche->consciousness = (float)fmin(1.0f,
            entity->psyche->consciousness + (0.2f * deltaTime));
    } else {
        entity->psyche->consciousness = (float)fmax(0.0f,
            entity->psyche->consciousness - (0.1f * deltaTime));
    }
}

void HandleResonance(Entity* source, Entity* target) {
    if (!source || !target || !source->psyche || !target->psyche) return;
    
    float strength = CalculateResonanceStrength(source->psyche, target->psyche);
    target->psyche->resonanceLevel = (float)fmin(1.0f, 
        target->psyche->resonanceLevel + strength);
    target->psyche->isResonating = true;
}

float CalculateResonanceStrength(PsychologicalProfile* p1, PsychologicalProfile* p2) {
    if (!p1 || !p2) return 0.0f;
    
    // Base resonance on enneagram compatibility and shadow intensity
    float typeCompatibility = 1.0f - (fabsf((float)p1->enneagramType - (float)p2->enneagramType) / 9.0f);
    float shadowHarmony = fabsf(p1->shadowIntensity - p2->shadowIntensity);
    
    return (typeCompatibility * 0.7f + (1.0f - shadowHarmony) * 0.3f) * 0.5f;
}

void SetEntityState(Entity* entity, EntityState newState) {
    if (!entity) return;
    entity->state = newState;
}

void MoveEntity(Entity* entity, Vector2 direction, float speed) {
    if (!entity) return;
    
    entity->velocity.x = direction.x * speed;
    entity->velocity.y = direction.y * speed;
    
    entity->position.x += entity->velocity.x;
    entity->position.y += entity->velocity.y;
    
    // Update bounds
    entity->bounds.x = entity->position.x;
    entity->bounds.y = entity->position.y;
}

Entity* FindNearestEntity(EntityPool* pool, Vector2 position, float radius, EntityType type) {
    if (!pool) return NULL;
    
    Entity* nearest = NULL;
    float minDist = radius;
    
    for (int i = 0; i < pool->count; i++) {
        Entity* entity = &pool->entities[i];
        if (!entity->isActive || entity->type != type) continue;
        
        float dist = Vector2Distance(position, entity->position);
        if (dist < minDist) {
            minDist = dist;
            nearest = entity;
        }
    }
    
    return nearest;
} 