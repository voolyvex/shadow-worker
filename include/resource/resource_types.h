#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H

#include <raylib.h>

// Resource structures
typedef struct TextureResource {
    const char* name;
    const char* filename;
    Texture2D texture;
} TextureResource;

typedef struct WaveResource {
    const char* name;
    const char* filename;
    Wave wave;
} WaveResource;

typedef struct MusicResource {
    const char* name;
    const char* filename;
    Music music;
} MusicResource;

typedef struct FontResource {
    const char* name;
    const char* filename;
    Font font;
} FontResource;

typedef struct ShaderResource {
    const char* name;
    const char* vsFilename;
    const char* fsFilename;
    Shader shader;
} ShaderResource;

#endif // RESOURCE_TYPES_H 