#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <raylib.h>

// Opaque type for texture manager
typedef struct TextureManager TextureManager;

// Create and destroy the texture manager
TextureManager* CreateTextureManager(void);
void DestroyTextureManager(TextureManager* manager);

// Load a texture atlas with regions
int LoadTextureAtlas(TextureManager* manager, const char* fileName, Rectangle* regions, int regionCount);

// Get texture and region information
Texture2D GetTextureFromAtlas(TextureManager* manager, int atlasIndex, int regionIndex);
Rectangle GetTextureRegion(TextureManager* manager, int atlasIndex, int regionIndex);

#endif // TEXTURE_MANAGER_H 