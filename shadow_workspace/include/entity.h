#ifndef ENTITY_H
#define ENTITY_H

#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include "entity_types.h"
#include "entity_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

// Entity type definitions
// typedef enum {
//     ENTITY_NONE,
//     ENTITY_PLAYER,
//     ENTITY_NPC,
//     ENTITY_ITEM
// } EntityType;

// Forward declarations
struct World;
struct EntityPool;

// Entity management functions
extern void DestroyEntity(Entity* entity);
extern void UpdateEntity(Entity* entity, struct World* world, float deltaTime);
extern void DrawEntity(Entity* entity);

// Component access functions
TransformComponent* GetTransformComponent(Entity* entity);
PhysicsComponent* GetPhysicsComponent(Entity* entity);
RenderComponent* GetRenderComponent(Entity* entity);
ColliderComponent* GetColliderComponent(Entity* entity);
AIComponent* GetAIComponent(Entity* entity);
PlayerControlComponent* GetPlayerControlComponent(Entity* entity);

// Component initialization functions
void InitializeTransformComponent(TransformComponent* transform, Vector2 position);
void InitializePhysicsComponent(PhysicsComponent* physics);
void InitializeRenderComponent(RenderComponent* render);
void InitializeColliderComponent(ColliderComponent* collider, Rectangle bounds);
void InitializeAIComponent(AIComponent* ai);
void InitializePlayerControlComponent(PlayerControlComponent* playerControl);

// Entity functions
void UpdateEntityPosition(Entity* entity, Vector2 newPosition);
void InitEntity(Entity* entity);

// Component functions
void AddComponent(Entity* entity, ComponentFlags component);
void RemoveComponent(Entity* entity, ComponentFlags component);
bool HasComponent(const Entity* entity, ComponentFlags component);

// Entity iteration
void ForEachEntity(Entity* entities, size_t count, void (*callback)(Entity*));

#ifdef __cplusplus
}
#endif

#endif // ENTITY_H 