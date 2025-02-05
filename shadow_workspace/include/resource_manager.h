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
    size_t textureCount;
    size_t waveCount;
    size_t musicCount;
    size_t fontCount;
    size_t shaderCount;
    size_t textureCapacity;
    size_t waveCapacity;
    size_t musicCapacity;
    size_t fontCapacity;
    size_t shaderCapacity;
    bool initialized;
} ResourceManager;

// Resource manager functions
ResourceManager* GetResourceManager(void);
ResourceManager* CreateResourceManager(void);
void DestroyResourceManager(ResourceManager* manager);
void UnloadResourceManager(void);
void UnloadAllResources(ResourceManager* manager);
void ReloadAllResources(ResourceManager* manager);

// Generic resource functions
void* GetResource(ResourceManager* manager, const char* name);
void UnloadResource(ResourceManager* manager, const char* name);
bool LoadResource(ResourceManager* manager, const char* name, const char* filename);

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

// Resource retrieval functions - updated to be const-correct
const Texture2D* GetGameTexture(const ResourceManager* manager, const char* name);
const Wave* GetGameWave(const ResourceManager* manager, const char* name);
const Music* GetGameMusic(const ResourceManager* manager, const char* name);
const Font* GetGameFont(const ResourceManager* manager, const char* name);
const Shader* GetGameShader(const ResourceManager* manager, const char* name);
const Sound* GetGameSound(const ResourceManager* manager, const char* name);

// Resource management functions
bool IsResourceLoaded(const ResourceManager* manager, const char* name);
size_t GetResourceCount(const ResourceManager* manager);

// Resource addition functions
bool AddGameTexture(ResourceManager* manager, const char* filename, const char* name);
bool AddGameWave(ResourceManager* manager, const char* filename, const char* name);
bool AddGameMusic(ResourceManager* manager, const char* filename, const char* name);
bool AddGameFont(ResourceManager* manager, const char* filename, const char* name);
bool AddGameShader(ResourceManager* manager, const char* vsFilename, const char* fsFilename, const char* name);

#endif // RESOURCE_MANAGER_H 