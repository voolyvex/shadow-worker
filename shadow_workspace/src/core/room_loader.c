#include "room_loader.h"
#include "../../include/logger.h"
#include <stdlib.h>
#include <string.h>

RoomTemplate* LoadRoomTemplate(const char* templatePath) {
    if (!templatePath) return NULL;
    
    // Allocate room template
    RoomTemplate* template = (RoomTemplate*)malloc(sizeof(RoomTemplate));
    if (!template) return NULL;
    
    // Initialize with default values
    template->width = 0;
    template->height = 0;
    template->tiles = NULL;
    template->objects = NULL;
    template->spawnPoints = NULL;
    template->spawnCount = 0;
    template->connections = NULL;
    template->connectionCount = 0;
    
    // TODO: Load template data from file
    // For now, return empty template
    return template;
}

void UnloadRoomTemplate(RoomTemplate* template) {
    if (!template) return;
    
    // Free tiles
    if (template->tiles) {
        for (int i = 0; i < template->height; i++) {
            free(template->tiles[i]);
        }
        free(template->tiles);
    }
    
    // Free objects
    if (template->objects) {
        for (int i = 0; i < template->height; i++) {
            free(template->objects[i]);
        }
        free(template->objects);
    }
    
    // Free spawn points and connections
    free(template->spawnPoints);
    free(template->connections);
    
    // Free template itself
    free(template);
}

bool ValidateRoomTemplate(const RoomTemplate* template) {
    if (!template) return false;
    
    // Check dimensions
    if (template->width <= 0 || template->height <= 0) return false;
    
    // Check required arrays
    if (!template->tiles || !template->objects) return false;
    
    // Check spawn points
    if (template->spawnCount > 0 && !template->spawnPoints) return false;
    
    // Check connections
    if (template->connectionCount > 0 && !template->connections) return false;
    
    return true;
}

bool CheckConnectionCompatibility(const RoomTemplate* room1, const RoomTemplate* room2) {
    if (!room1 || !room2) return false;
    
    // For now, assume all rooms are compatible
    return true;
} 