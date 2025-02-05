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