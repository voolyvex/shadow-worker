#ifndef SHADOW_WORKER_RESOURCE_MANAGER_H
#define SHADOW_WORKER_RESOURCE_MANAGER_H

#include <raylib.h>
#include <stdbool.h>

// Maximum resource counts
#define MAX_TEXTURES 100
#define MAX_SOUNDS 50
#define MAX_MUSIC 20

// Resource manager structure
typedef struct ResourceManager {
    // Textures
    Texture2D* textures;
    char** textureNames;
    int textureCount;
    
    // Audio
    Wave* waves;
    char** waveNames;
    int waveCount;
    
    // Music
    Music* music;
    char** musicNames;
    int musicCount;
} ResourceManager;

// Function declarations
bool InitResourceManager(void);
void UnloadResourceManager(void);
ResourceManager* GetResourceManager(void);

// Resource loading functions
Texture2D* LoadGameTexture(const char* name);
Wave* LoadGameSound(const char* filename);
Music* LoadGameMusic(const char* filename);

// Resource retrieval functions
Texture2D* GetTexture(const char* name);
Wave* GetWave(const char* name);
Music* GetMusic(const char* name);

// Resource loading utilities
bool LoadResources(const char* resourcePath);

#endif // SHADOW_WORKER_RESOURCE_MANAGER_H 