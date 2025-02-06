#ifndef CORE_COMPONENT_FLAGS_H
#define CORE_COMPONENT_FLAGS_H

#include <stdint.h>

/**
 * @brief Component type flags using bit fields for efficient component presence checking
 * 
 * The component system uses a bit-field approach where each component type is represented
 * by a single bit. This allows for efficient component presence checking and component
 * combination validation.
 */
typedef enum ComponentFlags {
    /** No components attached */
    COMPONENT_NONE           = 0,
    
    /** 
     * @brief Transform Component (position, rotation, scale)
     * - Required for any entity that exists in the game world
     * - Handles basic spatial properties
     * - Parent component for hierarchical transformations
     */
    COMPONENT_TRANSFORM     = (1 << 0),
    
    /**
     * @brief Physics Component
     * - Handles movement, forces, and physical interactions
     * - Controls velocity, acceleration, and mass properties
     * - Required for any entity affected by physics
     */
    COMPONENT_PHYSICS       = (1 << 1),
    
    /**
     * @brief Render Component
     * - Controls how an entity is displayed
     * - Manages textures, colors, and visual properties
     * - Required for any visible entity
     */
    COMPONENT_RENDER        = (1 << 2),
    
    /**
     * @brief Collider Component
     * - Defines collision boundaries and behavior
     * - Controls interaction with other physical entities
     * - Required for any entity that needs collision detection
     */
    COMPONENT_COLLIDER      = (1 << 3),
    
    /**
     * @brief AI Component
     * - Controls non-player entity behavior
     * - Manages state machines and decision making
     * - Required for NPCs and autonomous entities
     */
    COMPONENT_AI            = (1 << 4),
    
    /**
     * @brief Player Component
     * - Handles player-specific behavior and input
     * - Controls player movement and interactions
     * - Only one instance should exist per player
     */
    COMPONENT_PLAYER        = (1 << 5),
    
    // Common component combinations for entity archetypes
    /** Static objects (buildings, decorations) */
    COMPONENT_STATIC = COMPONENT_TRANSFORM | COMPONENT_RENDER,
    
    /** Moving objects with physics (items, projectiles) */
    COMPONENT_DYNAMIC = COMPONENT_TRANSFORM | COMPONENT_PHYSICS | COMPONENT_RENDER | COMPONENT_COLLIDER,
    
    /** Non-player characters with AI behavior */
    COMPONENT_NPC = COMPONENT_TRANSFORM | COMPONENT_PHYSICS | COMPONENT_RENDER | COMPONENT_COLLIDER | COMPONENT_AI,
    
    /** Complete player entity configuration */
    COMPONENT_PLAYER_FULL = COMPONENT_TRANSFORM | COMPONENT_PHYSICS | COMPONENT_RENDER | COMPONENT_COLLIDER | COMPONENT_PLAYER
} ComponentFlags;

/** Mask covering all valid component bits */
#define COMPONENT_MASK ((1 << 6) - 1)  // 6 components total

/**
 * @brief Check if an entity has all required components
 * @param flags Current component flags
 * @param required Required component combination
 * @return true if all required components are present
 */
#define HAS_ALL_COMPONENTS(flags, required) (((flags) & (required)) == (required))

/**
 * @brief Check if an entity has any of the specified components
 * @param flags Current component flags
 * @param mask Component mask to check against
 * @return true if any specified component is present
 */
#define HAS_ANY_COMPONENT(flags, mask) (((flags) & (mask)) != 0)

/**
 * @brief Validate if a component flag is valid
 * @param flag Component flag to validate
 * @return true if the flag is within valid range
 */
#define IS_VALID_COMPONENT(flag) (((flag) & COMPONENT_MASK) != 0)

#endif // CORE_COMPONENT_FLAGS_H 