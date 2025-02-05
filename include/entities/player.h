#ifndef SHADOW_WORKER_PLAYER_H
#define SHADOW_WORKER_PLAYER_H

#include <raylib.h>
#include "../../include/world.h"

typedef struct PlayerData {
    Vector2 position;
    float speed;
    int health;
    int stamina;
    int resonance;
    bool isMoving;
    Vector2 velocity;
    float footstepTimer;
    float footstepInterval;
} PlayerData;

PlayerData* CreatePlayer(void);
void DestroyPlayer(void);
void UpdatePlayer(float deltaTime);
void DrawPlayer(void);
float PlayerClamp(float value, float min, float max);
Vector2 GetPlayerPosition(const World* world);

#endif // SHADOW_WORKER_PLAYER_H 