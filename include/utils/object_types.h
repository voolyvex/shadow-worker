#ifndef SHADOW_WORKER_OBJECT_TYPES_H
#define SHADOW_WORKER_OBJECT_TYPES_H

// Object types
typedef enum {
    OBJECT_TYPE_NONE,
    OBJECT_TYPE_PLAYER,
    OBJECT_TYPE_NPC,
    OBJECT_TYPE_ITEM,
    OBJECT_TYPE_OBSTACLE,
    OBJECT_TYPE_TRIGGER,
    OBJECT_TYPE_COUNT
} ObjectType;

// Function declarations
const char* get_object_type_name(ObjectType type);
ObjectType get_object_type_from_name(const char* name);

#endif // SHADOW_WORKER_OBJECT_TYPES_H 