#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H

#include <raylib.h>

// Wrapper structs for resources
typedef struct {
    Texture2D texture;
    const char* name;
} TextureResource;

typedef struct {
    Wave wave;
    const char* name;
} WaveResource;

typedef struct {
    Music music;
    const char* name;
} MusicResource;

typedef struct {
    Font font;
    const char* name;
} FontResource;

typedef struct {
    Shader shader;
    const char* name;
} ShaderResource;

#endif // RESOURCE_TYPES_H 