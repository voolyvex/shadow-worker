#ifndef SHADOW_WORKER_RESOURCE_MANAGER_H
#define SHADOW_WORKER_RESOURCE_MANAGER_H

#include <raylib.h>
#include <stdbool.h>

// Maximum resource counts
#define MAX_TEXTURES 100
#define MAX_SOUNDS 50
#define MAX_FONTS 20
#define MAX_SHADERS 10

// Resource types
typedef enum ResourceType {
    RESOURCE_TEXTURE,
    RESOURCE_SOUND,
    RESOURCE_FONT,
    RESOURCE_SHADER
} ResourceType;

// Resource pool structures
typedef struct TextureResource {
    Texture2D texture;
    char* name;
    bool isLoaded;
} TextureResource;

typedef struct SoundResource {
    Sound sound;
    char* name;
    bool isLoaded;
} SoundResource;

typedef struct FontResource {
    Font font;
    char* name;
    bool isLoaded;
} FontResource;

typedef struct ShaderResource {
    Shader shader;
    char* name;
    bool isLoaded;
} ShaderResource;

// Resource manager structure
typedef struct ResourceManager {
    TextureResource texturePool[MAX_TEXTURES];
    SoundResource soundPool[MAX_SOUNDS];
    FontResource fontPool[MAX_FONTS];
    ShaderResource shaderPool[MAX_SHADERS];
    int textureCount;
    int soundCount;
    int fontCount;
    int shaderCount;
} ResourceManager;

// Function declarations
bool InitResourceManager(void);
void UnloadResourceManager(void);
ResourceManager* GetResourceManager(void);

// Resource loading functions
Texture2D* LoadGameTexture(const char* name, const char* path);
Sound* LoadGameSound(const char* name, const char* path);
Font* LoadGameFont(const char* name, const char* path);
Shader* LoadGameShader(const char* name, const char* vsPath, const char* fsPath);

// Resource retrieval functions
Texture2D* GetTexture(const char* name);
Sound* GetSound(const char* name);
Font* GetFont(const char* name);
Shader* GetShader(const char* name);

// Utility functions
void PrintResourceStats(ResourceManager* mgr);
bool IsResourceLoaded(ResourceManager* mgr, const char* name, ResourceType type);
void UnloadResourceBatch(ResourceManager* mgr, ResourceType type);
const char* GetResourceError(void);

#endif // SHADOW_WORKER_RESOURCE_MANAGER_H 