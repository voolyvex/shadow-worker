#include "../../include/entities/npc.h"
#include "../../include/world.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

#define NPC_SIZE 32.0f
#define INTERACTION_DISTANCE 64.0f

// NPC-specific data structure
typedef struct NPCData {
    NPCState state;
    float interactionTimer;
    const char* dialogueText;
    bool canInteract;
} NPCData;

// Forward declarations
static void UpdateNPCInternal(Entity* self, float deltaTime);
static void DrawNPCInternal(Entity* self);
static void OnNPCCollision(Entity* self, Entity* other);
static void OnNPCInteraction(Entity* self, Entity* other);

Entity* CreateNPC(EntityPool* pool, Vector2 position, ResourceManager* resources) {
    if (!pool || !resources) return NULL;

    Entity* npc = CreateEntity(pool, ENTITY_NPC, position);
    if (!npc) return NULL;

    // Set up NPC properties
    npc->scale = 1.0f;
    npc->rotation = 0.0f;
    npc->bounds = (Rectangle){
        position.x - NPC_SIZE/2,
        position.y - NPC_SIZE/2,
        NPC_SIZE,
        NPC_SIZE
    };

    // Initialize NPC data
    NPCData* data = (NPCData*)malloc(sizeof(NPCData));
    if (!data) {
        DestroyEntity(pool, npc);
        return NULL;
    }

    data->state = NPC_IDLE;
    data->interactionTimer = 0.0f;
    data->dialogueText = "Hello, I am a shadow worker. Press E to talk.";
    data->canInteract = true;

    npc->data = data;

    // Set up function pointers
    npc->Update = UpdateNPCInternal;
    npc->Draw = DrawNPCInternal;
    npc->OnCollision = OnNPCCollision;
    npc->OnInteraction = OnNPCInteraction;

    // Load NPC sprite
    npc->sprite = LoadGameTexture("npc_idle", "resources/sprites/npc/idle.png");

    return npc;
}

static void UpdateNPCInternal(Entity* entity, float deltaTime) {
    if (!entity || !entity->data) return;

    NPCData* data = (NPCData*)entity->data;
    
    // Update interaction timer
    if (data->state == NPC_TALKING) {
        data->interactionTimer -= deltaTime;
        if (data->interactionTimer <= 0.0f) {
            data->state = NPC_IDLE;
            data->canInteract = true;
        }
    }
}

static void DrawNPCInternal(Entity* entity) {
    if (!entity) return;

    Color color = WHITE;
    NPCData* data = (NPCData*)entity->data;

    // Draw NPC sprite or placeholder
    if (entity->sprite) {
        DrawTexturePro(
            *entity->sprite,
            (Rectangle){0, 0, entity->sprite->width, entity->sprite->height},
            entity->bounds,
            (Vector2){NPC_SIZE/2, NPC_SIZE/2},
            entity->rotation,
            color
        );
    } else {
        DrawRectanglePro(
            entity->bounds,
            (Vector2){NPC_SIZE/2, NPC_SIZE/2},
            entity->rotation,
            PURPLE
        );
    }

    // Draw interaction prompt or dialogue
    if (data->state == NPC_TALKING) {
        DrawText(data->dialogueText, 
                entity->position.x - MeasureText(data->dialogueText, 20)/2,
                entity->position.y - 50,
                20,
                WHITE);
    } else if (data->canInteract) {
        DrawText("Press E to interact",
                entity->position.x - MeasureText("Press E to interact", 20)/2,
                entity->position.y - 50,
                20,
                GRAY);
    }
}

void UnloadNPC(Entity* entity) {
    if (!entity) return;
    if (entity->data) {
        free(entity->data);
        entity->data = NULL;
    }
}

static void OnNPCCollision(Entity* self, Entity* other) {
    // NPCs don't need complex collision handling
}

static void OnNPCInteraction(Entity* self, Entity* other) {
    if (!self || !other || !self->data) return;
    
    NPCData* data = (NPCData*)self->data;
    
    // Check if player is close enough and interaction is possible
    if (data->canInteract && Vector2Distance(self->position, other->position) <= INTERACTION_DISTANCE) {
        data->state = NPC_TALKING;
        data->interactionTimer = 3.0f;  // Display dialogue for 3 seconds
        data->canInteract = false;
    }
} 