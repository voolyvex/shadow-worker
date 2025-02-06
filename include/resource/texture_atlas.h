#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <raylib.h>
#include <stdbool.h>

// Initialize texture atlas with given dimensions
bool InitTextureAtlas(int width, int height, int tileSize);

// Clean up texture atlas resources
void UnloadTextureAtlas(void);

// Add a texture to the atlas
bool AddTextureToAtlas(const char* name, const char* filename);

// Get the region for a named texture in the atlas
Rectangle GetTextureRegion(const char* name);

// Get the atlas texture
const Texture2D* GetAtlasTexture(void);

// Generate atlas from a directory of textures
bool GenerateAtlasFromDirectory(const char* directory);

// Save atlas metadata to file
bool SaveAtlasMetadata(const char* filename);

#endif // TEXTURE_ATLAS_H 