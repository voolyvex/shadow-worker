#include "../../include/warning_suppression.h"
#include "../../include/debug.h"

BEGIN_EXTERNAL_WARNINGS
#include <raylib.h>
#include <raymath.h>
END_EXTERNAL_WARNINGS

#include "../../include/texture_manager.h"
#include "../../include/logger.h"
#include <stdlib.h>
#include <string.h>

// Other texture manager functions can go here...
// ... existing code ... 

TextureManager* CreateTextureManager(void) {
    LOG_INFO(LOG_CORE, "Creating texture manager...");
    DumpMemoryState("Before TextureManager creation");
    
    if (!IsWindowReady()) {
        LOG_ERROR(LOG_CORE, "Cannot create texture manager - window not initialized");
        return NULL;
    }

    TextureManager* manager = NULL;
    
    // Add memory allocation debugging
    LOG_INFO(LOG_CORE, "Allocating memory for texture manager...");
    manager = (TextureManager*)MemAlloc(sizeof(TextureManager));
    TRACK_ALLOC(manager, sizeof(TextureManager));
    
    if (!manager) {
        LOG_ERROR(LOG_CORE, "Failed to allocate texture manager - out of memory");
        return NULL;
    }

    LOG_INFO(LOG_CORE, "Initializing texture manager fields...");
    
    // Initialize with validation
    manager->atlasCount = 0;
    for (int i = 0; i < MAX_TEXTURES; i++) {
        LOG_DEBUG(LOG_CORE, "Initializing atlas slot %d", i);
        manager->atlases[i].texture.id = 0;
        manager->atlases[i].regions = NULL;
        manager->atlases[i].regionCount = 0;
    }

    DumpMemoryState("After TextureManager creation");
    LOG_INFO(LOG_CORE, "Texture manager created successfully");
    return manager;
}

void DestroyTextureManager(TextureManager* manager) {
    LOG_INFO(LOG_CORE, "Destroying texture manager...");
    DumpMemoryState("Before TextureManager destruction");
    
    if (!manager) {
        LOG_WARNING(LOG_CORE, "Attempted to destroy NULL texture manager");
        return;
    }

    // Free all allocated resources
    for (int i = 0; i < manager->atlasCount; i++) {
        if (manager->atlases[i].regions) {
            LOG_DEBUG(LOG_CORE, "Freeing regions for atlas %d", i);
            TRACK_FREE(manager->atlases[i].regions);
            MemFree(manager->atlases[i].regions);
            manager->atlases[i].regions = NULL;
        }
        
        if (manager->atlases[i].texture.id != 0) {
            LOG_DEBUG(LOG_CORE, "Unloading texture for atlas %d", i);
            UnloadTexture(manager->atlases[i].texture);
            manager->atlases[i].texture.id = 0;
        }
    }

    LOG_DEBUG(LOG_CORE, "Freeing texture manager");
    TRACK_FREE(manager);
    MemFree(manager);
    
    DumpMemoryState("After TextureManager destruction");
    LOG_INFO(LOG_CORE, "Texture manager destroyed successfully");
}

int LoadTextureAtlas(TextureManager* manager, const char* fileName, Rectangle* regions, int regionCount) {
    LOG_INFO(LOG_RESOURCE, "Loading texture atlas: %s", fileName ? fileName : "NULL");
    DumpMemoryState("Before LoadTextureAtlas");
    
    // Validate input parameters with detailed logging
    VALIDATE_PTR_RETURN(manager, -1);
    VALIDATE_PTR_RETURN(fileName, -1);
    VALIDATE_PTR_RETURN(regions, -1);

    if (regionCount <= 0) {
        LOG_ERROR(LOG_RESOURCE, "Invalid region count: %d", regionCount);
        return -1;
    }

    LOG_DEBUG(LOG_RESOURCE, "Current atlas count: %d, Maximum allowed: %d", manager->atlasCount, MAX_TEXTURE_ATLASES);
    
    if (manager->atlasCount >= MAX_TEXTURE_ATLASES) {
        LOG_ERROR(LOG_RESOURCE, "Maximum number of texture atlases reached (%d)", MAX_TEXTURE_ATLASES);
        return -1;
    }

    // Check file existence with path logging
    LOG_DEBUG(LOG_RESOURCE, "Checking file existence: %s", fileName);
    if (!FileExists(fileName)) {
        LOG_ERROR(LOG_RESOURCE, "Texture atlas file not found: %s", fileName);
        return -1;
    }

    // Load the texture with validation
    LOG_INFO(LOG_RESOURCE, "Loading texture from file...");
    Texture2D texture = LoadTexture(fileName);
    
    if (texture.id == 0) {
        LOG_ERROR(LOG_RESOURCE, "Failed to load texture atlas: %s (texture.id = 0)", fileName);
        return -1;
    }

    LOG_DEBUG(LOG_RESOURCE, "Loaded texture dimensions: %dx%d", texture.width, texture.height);

    // Validate texture dimensions
    if (texture.width <= 0 || texture.height <= 0) {
        LOG_ERROR(LOG_RESOURCE, "Invalid texture dimensions: %dx%d", texture.width, texture.height);
        UnloadTexture(texture);
        return -1;
    }

    // Validate regions with detailed logging
    LOG_INFO(LOG_RESOURCE, "Validating %d texture regions...", regionCount);
    for (int i = 0; i < regionCount; i++) {
        LOG_DEBUG(LOG_RESOURCE, "Validating region %d: x=%.2f, y=%.2f, w=%.2f, h=%.2f", 
                 i, regions[i].x, regions[i].y, regions[i].width, regions[i].height);
                 
        if (regions[i].x < 0 || regions[i].y < 0 ||
            regions[i].width <= 0 || regions[i].height <= 0 ||
            regions[i].x + regions[i].width > texture.width ||
            regions[i].y + regions[i].height > texture.height) {
            LOG_ERROR(LOG_RESOURCE, "Invalid region coordinates at index %d: (%.2f, %.2f, %.2f, %.2f)", 
                     i, regions[i].x, regions[i].y, regions[i].width, regions[i].height);
            UnloadTexture(texture);
            return -1;
        }
    }

    // Allocate memory for regions with validation
    LOG_INFO(LOG_RESOURCE, "Allocating memory for %d regions...", regionCount);
    Rectangle* regionsCopy = (Rectangle*)MemAlloc(regionCount * sizeof(Rectangle));
    TRACK_ALLOC(regionsCopy, regionCount * sizeof(Rectangle));
    
    if (!regionsCopy) {
        LOG_ERROR(LOG_RESOURCE, "Failed to allocate memory for texture regions (size: %zu bytes)", 
                 regionCount * sizeof(Rectangle));
        UnloadTexture(texture);
        return -1;
    }

    // Copy regions with validation
    LOG_INFO(LOG_RESOURCE, "Copying region data...");
    for (int i = 0; i < regionCount; i++) {
        regionsCopy[i] = regions[i];
        LOG_DEBUG(LOG_RESOURCE, "Copied region %d: x=%.2f, y=%.2f, w=%.2f, h=%.2f", 
                 i, regionsCopy[i].x, regionsCopy[i].y, regionsCopy[i].width, regionsCopy[i].height);
    }

    // Store the atlas
    int atlasId = manager->atlasCount;
    LOG_INFO(LOG_RESOURCE, "Storing atlas at index %d", atlasId);
    
    manager->atlases[atlasId].texture = texture;
    manager->atlases[atlasId].regions = regionsCopy;
    manager->atlases[atlasId].regionCount = regionCount;
    manager->atlasCount++;

    DumpMemoryState("After LoadTextureAtlas");
    LOG_INFO(LOG_RESOURCE, "Successfully loaded texture atlas: %s with %d regions", fileName, regionCount);
    return atlasId;
}

bool LoadGameTextures(TextureManager* manager) {
    if (!IsWindowReady()) {
        TraceLog(LOG_ERROR, "Cannot load textures - window not initialized");
        return false;
    }
    
    if (!manager) {
        TraceLog(LOG_ERROR, "Invalid texture manager");
        return false;
    }
    
    // Load all required textures
    bool success = true;
    success &= LoadGameTexture(manager, "assets/textures/player.png", "player");
    success &= LoadGameTexture(manager, "assets/textures/tiles.png", "tiles");
    success &= LoadGameTexture(manager, "assets/textures/ui.png", "ui");
    
    if (!success) {
        TraceLog(LOG_ERROR, "Failed to load one or more textures");
        return false;
    }
    
    // Verify loaded textures
    if (!VerifyTextureResources(GetResourceManager())) {
        TraceLog(LOG_ERROR, "Texture verification failed after loading");
        return false;
    }
    
    return true;
}

// ... existing code ... 

bool IsTextureManagerValid(const TextureManager* manager) {
    if (!manager) {
        LOG_ERROR(LOG_CORE, "NULL texture manager");
        return false;
    }

    if (manager->atlasCount < 0 || manager->atlasCount > MAX_TEXTURE_ATLASES) {
        LOG_ERROR(LOG_CORE, "Invalid atlas count: %d", manager->atlasCount);
        return false;
    }

    return true;
}

bool IsAtlasIdValid(const TextureManager* manager, int atlasId) {
    if (!IsTextureManagerValid(manager)) {
        return false;
    }

    if (atlasId < 0 || atlasId >= manager->atlasCount) {
        LOG_ERROR(LOG_CORE, "Invalid atlas ID: %d (max: %d)", atlasId, manager->atlasCount - 1);
        return false;
    }

    if (!manager->atlases[atlasId].regions) {
        LOG_ERROR(LOG_CORE, "Atlas %d has NULL regions", atlasId);
        return false;
    }

    return true;
}

bool IsRegionIndexValid(const TextureManager* manager, int atlasId, int regionIndex) {
    if (!IsAtlasIdValid(manager, atlasId)) {
        return false;
    }

    if (regionIndex < 0 || regionIndex >= manager->atlases[atlasId].regionCount) {
        LOG_ERROR(LOG_CORE, "Invalid region index %d for atlas %d (max: %d)", 
                 regionIndex, atlasId, manager->atlases[atlasId].regionCount - 1);
        return false;
    }

    return true;
}

Texture2D GetTextureFromAtlas(const TextureManager* manager, int atlasId, int regionIndex) {
    Texture2D empty = { 0 };
    
    if (!IsRegionIndexValid(manager, atlasId, regionIndex)) {
        return empty;
    }
    
    return manager->atlases[atlasId].texture;
}

Rectangle GetTextureRegion(const TextureManager* manager, int atlasId, int regionIndex) {
    Rectangle empty = { 0, 0, 0, 0 };
    
    if (!IsRegionIndexValid(manager, atlasId, regionIndex)) {
        return empty;
    }
    
    return manager->atlases[atlasId].regions[regionIndex];
}

// ... existing code ... 