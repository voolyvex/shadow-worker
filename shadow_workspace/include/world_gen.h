#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include "world.h"
#include "constants.h"
#include <raylib.h>

// World generation functions
World* GenerateWorld(int width, int height, float gravity);
bool GenerateEstate(World* world);
bool GenerateCourtyard(World* world);
bool GenerateGardens(World* world);

// Room template management
bool LoadTemplates(World* world);
void UnloadTemplates(World* world);

#endif // WORLD_GEN_H 