#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <raylib.h>
#include "../utils/constants.h"
#include "resource_types.h"

typedef struct TextureAtlas {
    Texture2D texture;
    Rectangle* regions;
    int regionCount;
} TextureAtlas;

typedef struct TextureManager {
    TextureAtlas atlases[MAX_TEXTURE_ATLASES];
    int atlasCount;
} TextureManager;

// Create and destroy the texture manager
TextureManager* CreateTextureManager(void);
void DestroyTextureManager(TextureManager* manager);

// Atlas management
int LoadTextureAtlas(TextureManager* manager, const char* fileName, Rectangle* regions, int regionCount);
Texture2D GetTextureFromAtlas(const TextureManager* manager, int atlasId, int regionIndex);
Rectangle GetTextureRegion(const TextureManager* manager, int atlasId, int regionIndex);

// Validation
bool IsTextureManagerValid(const TextureManager* manager);
bool IsAtlasIdValid(const TextureManager* manager, int atlasId);
bool IsRegionIndexValid(const TextureManager* manager, int atlasId, int regionIndex);

#endif // TEXTURE_MANAGER_H 