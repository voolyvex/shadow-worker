#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H

#include <raylib.h>

// Resource structures
typedef struct {
    Texture2D texture;
    const char* name;
    const char* filename;
} TextureResource;

typedef struct {
    Wave wave;
    const char* name;
    const char* filename;
} WaveResource;

typedef struct {
    Music music;
    const char* name;
    const char* filename;
} MusicResource;

typedef struct {
    Font font;
    const char* name;
    const char* filename;
} FontResource;

typedef struct {
    Shader shader;
    const char* name;
    const char* vsFilename;
    const char* fsFilename;
} ShaderResource;

#endif // RESOURCE_TYPES_H 