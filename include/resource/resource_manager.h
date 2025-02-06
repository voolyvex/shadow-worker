#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include "../utils/constants.h"
#include "resource_types.h"

// Resource types
typedef enum {
    RESOURCE_TEXTURE,
    RESOURCE_WAVE,
    RESOURCE_MUSIC,
    RESOURCE_FONT,
    RESOURCE_SHADER,
    RESOURCE_COUNT
} ResourceType;

// Resource manager structure
typedef struct ResourceManager {
    TextureResource textures[MAX_TEXTURES];
    WaveResource waves[MAX_SOUNDS];
    MusicResource music[MAX_MUSIC];
    FontResource fonts[MAX_FONTS];
    ShaderResource shaders[MAX_SHADERS];
    
    size_t textureCount;
    size_t waveCount;
    size_t musicCount;
    size_t fontCount;
    size_t shaderCount;
    bool initialized;
} ResourceManager;

// Function declarations
ResourceManager* GetResourceManager(void);

// Texture management
bool LoadGameTexture(const char* filename, const char* name);
void UnloadGameTexture(const char* name);
Texture2D* GetGameTexture(const char* name);

// Wave management
bool LoadGameWave(const char* filename, const char* name);
void UnloadGameWave(const char* name);
Wave* GetGameWave(const char* name);

// Music management
bool LoadGameMusic(const char* filename, const char* name);
void UnloadGameMusic(const char* name);
Music* GetGameMusic(const char* name);

// Font management
bool LoadGameFont(const char* filename, const char* name);
void UnloadGameFont(const char* name);
Font* GetGameFont(const char* name);

// Shader management
bool LoadGameShader(const char* vsFilename, const char* fsFilename, const char* name);
void UnloadGameShader(const char* name);
Shader* GetGameShader(const char* name);

void UnloadAllResources(ResourceManager* manager);

#endif // RESOURCE_MANAGER_H 