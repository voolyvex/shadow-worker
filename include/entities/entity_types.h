#ifndef ENTITY_TYPES_H
#define ENTITY_TYPES_H

#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include "../utils/constants.h"
#include "../core/component_flags.h"

// Entity dimensions and constants
#define MAX_COMPONENT_TYPES 32
#define MAX_COMPONENTS 16
#define COMPONENT_ARRAY_ALIGNMENT 16
#define NPC_WIDTH 32
#define NPC_HEIGHT 32
#define NPC_DETECTION_RADIUS 200.0f
#define NPC_SAFE_DISTANCE 300.0f
#define PLAYER_SPEED 200.0f
#define NPC_SPEED 150.0f

// Animation constants
#define ANIMATION_FRAME_TIME 0.1f
#define ANIMATION_FRAME_COUNT 4
#define ARRIVAL_THRESHOLD 5.0f

// State durations
#define IDLE_DURATION 3.0f
#define PATROL_DURATION 5.0f
#define FLEE_DURATION 2.0f

// Forward declarations
struct World;
struct Entity;

// Entity states
typedef enum {
    ENTITY_STATE_NONE,
    ENTITY_STATE_IDLE,
    ENTITY_STATE_PATROL,
    ENTITY_STATE_CHASE,
    ENTITY_STATE_FLEE,
    ENTITY_STATE_ATTACK,
    ENTITY_STATE_INTERACT,
    ENTITY_STATE_DEAD
} EntityState;

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
    EntityState state;
    float stateTimer;
    int animationFrame;
    float animationTimer;
    float moveSpeed;
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
    EntityState state;
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