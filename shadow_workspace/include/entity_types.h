#ifndef ENTITY_TYPES_H
#define ENTITY_TYPES_H

#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include "constants.h"

#define MAX_COMPONENTS 6
#define MAX_COMPONENT_TYPES 32
#define COMPONENT_ARRAY_ALIGNMENT 16

// Entity dimensions
#define NPC_WIDTH 32
#define NPC_HEIGHT 32

// Forward declarations
struct World;
struct Entity;

// Entity component flags
typedef enum ComponentFlags {
    COMPONENT_NONE = 0,
    COMPONENT_TRANSFORM = 1 << 0,
    COMPONENT_PHYSICS = 1 << 1,
    COMPONENT_RENDER = 1 << 2,
    COMPONENT_COLLIDER = 1 << 3,
    COMPONENT_AI = 1 << 4,
    COMPONENT_PLAYER_CONTROL = 1 << 5
} ComponentFlags;

// Component Registry for managing component arrays
typedef struct ComponentRegistry {
    void* componentArrays[MAX_COMPONENT_TYPES];
    size_t componentSizes[MAX_COMPONENT_TYPES];
    size_t componentCounts[MAX_COMPONENT_TYPES];
    size_t componentCapacities[MAX_COMPONENT_TYPES];
} ComponentRegistry;

// Component Registry management functions
ComponentRegistry* CreateComponentRegistry(void);
void DestroyComponentRegistry(ComponentRegistry* registry);
void* GetComponentArray(ComponentRegistry* registry, ComponentFlags type);
void* AddComponentToRegistry(ComponentRegistry* registry, ComponentFlags type, size_t componentSize);
void RemoveComponentFromRegistry(ComponentRegistry* registry, ComponentFlags type, size_t entityId);

#ifdef __cplusplus
extern "C" {
#endif

// Component management function declarations
void AddComponent(struct Entity* entity, ComponentFlags component);
void RemoveComponent(struct Entity* entity, ComponentFlags component);
bool HasComponent(const struct Entity* entity, ComponentFlags component);

// Entity types
typedef enum {
    ENTITY_TYPE_NONE,
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_NPC,
    ENTITY_TYPE_OBJECT,
    ENTITY_TYPE_COUNT
} EntityType;

// NPC States
typedef enum {
    NPC_STATE_NONE = 0,
    NPC_STATE_IDLE,
    NPC_STATE_PATROL,
    NPC_STATE_CHASE,
    NPC_STATE_FLEE
} NPCState;

// Component type definitions
typedef struct {
    Vector2 position;
    float rotation;
    float scale;
} TransformComponent;

typedef struct {
    Vector2 velocity;
    Vector2 acceleration;
    float friction;
    float mass;
    bool isKinematic;
} PhysicsComponent;

typedef struct {
    Texture2D* texture;
    Color color;
    Rectangle sourceRect;
    Vector2 origin;
    bool visible;
    float opacity;
} RenderComponent;

typedef struct {
    Rectangle bounds;
    bool isStatic;
    bool isTrigger;
    bool isEnabled;
} ColliderComponent;

typedef struct {
    float patrolRadius;
    float detectionRadius;
    Vector2 homePosition;
    Vector2 targetPosition;
    bool isAggressive;
    NPCState state;
    float stateTimer;
    int animationFrame;
} AIComponent;

typedef struct {
    float moveSpeed;
    float turnSpeed;
    bool isInteracting;
} PlayerControlComponent;

// Component data union
typedef union {
    TransformComponent transform;
    PhysicsComponent physics;
    RenderComponent render;
    ColliderComponent collider;
    AIComponent ai;
    PlayerControlComponent playerControl;
} ComponentData;

// Entity definition
typedef struct Entity {
    EntityType type;
    ComponentFlags components;
    bool active;
    Vector2 position;
    Rectangle bounds;
    Rectangle collider;
    Color color;
    float rotation;
    float scale;
    bool visible;
    ComponentData components_data[MAX_COMPONENTS];
    void (*Update)(struct Entity* entity, struct World* world, float deltaTime);
    void (*Draw)(struct Entity* entity);
    void (*OnCollision)(struct Entity* entity, struct Entity* other);
    void (*OnDestroy)(struct Entity* entity);
} Entity;

#ifdef __cplusplus
}
#endif

#endif // ENTITY_TYPES_H 