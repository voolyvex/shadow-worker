#ifndef SHADOW_WORKER_RESOURCE_MANAGER_H
#define SHADOW_WORKER_RESOURCE_MANAGER_H

#include <raylib.h>
#include <stdbool.h>

#define MAX_TEXTURES 100
#define MAX_SOUNDS 50
#define MAX_MUSIC 10

// Resource manager structure
typedef struct ResourceManager {
    Texture2D* textures;
    char** textureNames;
    int textureCount;
    
    Sound* sounds;
    char** soundNames;
    int soundCount;
    
    Music* music;
    char** musicNames;
    int musicCount;
} ResourceManager;

// Function declarations
bool InitResourceManager(void);
void UnloadResourceManager(void);
ResourceManager* GetResourceManager(void);

// Resource loading
Texture2D* LoadGameTexture(const char* name, const char* filename);
Sound* LoadGameSound(const char* name, const char* filename);
Music* LoadGameMusic(const char* name, const char* filename);

// Resource retrieval
Texture2D* GetTexture(const char* name);
Sound* GetSound(const char* name);
Music* GetMusic(const char* name);

#endif // SHADOW_WORKER_RESOURCE_MANAGER_H 