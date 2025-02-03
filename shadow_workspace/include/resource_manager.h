#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include "resource_types.h"

#define MAX_TEXTURES 100
#define MAX_SOUNDS 100
#define MAX_MUSIC 10
#define MAX_FONTS 10
#define MAX_SHADERS 10

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
    TextureResource* textures;
    WaveResource* waves;
    MusicResource* music;
    FontResource* fonts;
    ShaderResource* shaders;
    int textureCount;
    int waveCount;
    int musicCount;
    int fontCount;
    int shaderCount;
    int textureCapacity;
    int waveCapacity;
    int musicCapacity;
    int fontCapacity;
    int shaderCapacity;
    bool initialized;
} ResourceManager;

// Resource manager functions
ResourceManager* CreateResourceManager(void);
void DestroyResourceManager(ResourceManager* manager);

// Resource loading functions
bool LoadGameTexture(ResourceManager* manager, const char* filename, const char* name);
bool LoadGameWave(ResourceManager* manager, const char* filename, const char* name);
bool LoadGameMusic(ResourceManager* manager, const char* filename, const char* name);
bool LoadGameFont(ResourceManager* manager, const char* filename, const char* name);
bool LoadGameShader(ResourceManager* manager, const char* vsFilename, const char* fsFilename, const char* name);

// Resource unloading functions
void UnloadGameTexture(ResourceManager* manager, const char* name);
void UnloadGameWave(ResourceManager* manager, const char* name);
void UnloadGameMusic(ResourceManager* manager, const char* name);
void UnloadGameFont(ResourceManager* manager, const char* name);
void UnloadGameShader(ResourceManager* manager, const char* name);

// Resource retrieval functions
Texture2D* GetGameTexture(ResourceManager* manager, const char* name);
Wave* GetGameWave(ResourceManager* manager, const char* name);
Music* GetGameMusic(ResourceManager* manager, const char* name);
Font* GetGameFont(ResourceManager* manager, const char* name);
Shader* GetGameShader(ResourceManager* manager, const char* name);

// Resource management functions
void UnloadAllResources(ResourceManager* manager);
void ReloadAllResources(ResourceManager* manager);
bool IsResourceLoaded(ResourceManager* manager, const char* name);
int GetResourceCount(ResourceManager* manager);

// Resource addition functions
bool AddGameTexture(ResourceManager* manager, const char* filename, const char* name);
bool AddGameWave(ResourceManager* manager, const char* filename, const char* name);
bool AddGameMusic(ResourceManager* manager, const char* filename, const char* name);
bool AddGameFont(ResourceManager* manager, const char* filename, const char* name);
bool AddGameShader(ResourceManager* manager, const char* vsFilename, const char* fsFilename, const char* name);

#endif // RESOURCE_MANAGER_H 