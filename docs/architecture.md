# Shadow Worker - Psychological RPG Architecture

## Core Concepts

### Psychological Framework
The game centers around a sophisticated psychological system that integrates:

1. **Personality Core**
   - Enneagram Types (1-9) with wings and stress/growth directions
   - DSM-5 dimensions with shadow integration
   - Development levels (1-9, healthy to unhealthy)

2. **Jungian Elements**
   - Archetypes (Persona, Shadow, Anima/Animus, Self, etc.)
   - Shadow projection and integration mechanics
   - Individuation progress tracking

3. **Psychological Resonance System**
   - States: GROUNDED → RESONATING → SYNCHRONIZED → TRANSCENDENT → UNIFIED
   - Emotional contagion and psychological influence mechanics
   - Collective consciousness connections

4. **Reality Perception Layers**
   - SURFACE → SUBCONSCIOUS → COLLECTIVE → INTEGRATED → TRANSFORMATIVE
   - Reality distortion and integration mechanics
   - Psychological manifestation system

### Core Systems

1. **PersonalityProfile Class**
   - Manages personality traits, states, and development
   - Handles psychological events and transformations
   - Tracks resonance and reality perception states

2. **WorldSystem**
   - Procedural room generation with psychological theming
   - Object and NPC placement based on psychological states
   - Environmental response to collective consciousness

3. **Interaction System**
   - Ray-based interaction detection
   - Event-driven psychological influence
   - Reality manipulation through consciousness

## Implementation Status

### Completed
1. **Core Framework**
   - PersonalityProfile class with full psychological system
   - Enneagram and DSM integration
   - Shadow aspects and projections
   - Surreal state management
   - Comprehensive testing infrastructure with mock objects
   - Automated test suite with headless execution

2. **Psychological Systems**
   - Resonance state progression
   - Reality perception layers
   - Inner truth manifestations
   - Consciousness thresholds
   - Visual effects system for psychological states
   - Reality anomaly creation and management
   - Consciousness field implementation

### In Progress
3. **Base Game Systems**
   - Player movement and interaction
   - World generation framework
   - Basic object placement
   - Test-driven development workflow
   - Mock system implementations
   - State management and cleanup

1. **Integration Systems**
   - Collective consciousness mechanics
   - Reality distortion effects
   - Psychological event propagation
   - Visual effects refinement
   - Integration testing expansion
   - Performance optimization

2. **Visual Systems**
   - Surreal state visualization
   - Psychological manifestations
   - Reality distortion effects
   - Visual effect parameter tuning
   - Shader system implementation
   - Effect transition smoothing

### Next Steps
1. **Gameplay Systems**
   - Implement psychological influence mechanics
   - Add reality manipulation abilities
   - Create consciousness-based puzzles

2. **Content Creation**
   - Design psychological environments
   - Create NPC personality profiles
   - Develop consciousness-based quests

3. **Polish**
   - Add visual effects for psychological states
   - Implement sound design for resonance
   - Create UI for psychological tracking

## Technical Architecture

### Key Components
1. **Personality System**
```cpp
class PersonalityProfile {
    // Core traits and states
    EnneagramType primary_type;
    DSMDimensions dsm_traits;
    CoreTraits core_traits;
    
    // Psychological systems
    ResonanceState resonance_state;
    PerceptionLayer perception_layer;
    InnerTruth inner_truth;
    
    // Integration metrics
    float consciousness_integration;
    float reality_integration;
};
```

2. **World Generation**
```cpp
class WorldSystem {
    // Room generation
    std::vector<Room> rooms;
    TileMap* tilemap;
    
    // Object placement
    std::vector<PlaceableObject> objects;
    float object_density;
    
    // NPC management
    std::vector<NPCData> npcs;
    int min_npcs_per_room;
    int max_npcs_per_room;
};
```

3. **Visual Effects System**
```cpp
class VisualEffectsManager {
    // Core components
    RealitySystem* reality_system;
    ShaderMaterial* shader_material;
    
    // Effect management
    std::map<String, EffectParameters> active_effects;
    float global_distortion_intensity;
    
    // Effect parameters
    struct EffectParameters {
        float intensity;
        float duration;
        float elapsed_time;
        Dictionary properties;
        bool active;
    };
};
```

4. **Testing Infrastructure**
```gdscript
class TestRunner:
    # Test tracking
    var total_tests: int
    var passed_tests: int
    var failed_tests: int
    
    # Test suites
    var reality_tests: RealitySystemTests
    var effects_tests: VisualEffectsTests
    
    # Logging system
    var logger: TestLogger
    
    # Mock objects
    var mock_reality_system: MockRealitySystem
    var mock_visual_effects: MockVisualEffectsManager
```

### Test Organization
1. **Reality System Tests**
   - Anomaly creation and removal
   - Consciousness field management
   - Multiple entity handling
   - State validation

2. **Visual Effects Tests**
   - Effect start/stop functionality
   - Global intensity control
   - Multiple effect management
   - Parameter validation

3. **Mock Objects**
   - `MockRealitySystem`: Simulates reality manipulation
   - `MockVisualEffectsManager`: Handles visual effect testing
   - State reset between tests
   - Signal emission for test events

4. **Test Infrastructure**
   - Automated test execution
   - Comprehensive logging
   - State cleanup between tests
   - Asynchronous operation support
   - Headless testing capability

### Data Flow
1. Personality states influence world generation
2. Environmental factors affect psychological states
3. NPC interactions modify resonance levels
4. Collective consciousness shapes reality
5. Visual effects reflect psychological states
6. Test results validate system integrity

## Development Roadmap

### Phase 1 (Completed)
- Core personality system implementation
- Basic world generation
- Player mechanics

### Phase 2 (Current)
- Psychological resonance mechanics
- Reality perception system
- NPC consciousness integration

### Phase 3 (Planned)
- Advanced reality manipulation
- Collective consciousness events
- Environmental response system

### Phase 4 (Future)
- Quest system integration
- Advanced NPC behaviors
- Visual and audio polish

## Design Principles
1. Psychological depth over complexity
2. Meaningful player choice and consequence
3. Emergent narrative through psychological states
4. Subtle but impactful reality manipulation
5. Collective consciousness as a core mechanic

## Performance Considerations
1. Efficient psychological state updates
2. Optimized reality distortion calculations
3. Scalable NPC consciousness simulation
4. Memory-efficient world generation

## Current Structure

### Core Systems
- `WorldSystem`: Manages world generation, room creation, and object placement
- `TileMapManager`: Handles tile-based operations and collision layers
- `Constants`: Global enums and configuration values

### Scene Hierarchy
```
scenes/
├── Main.tscn (Root scene)
│   ├── WorldSystem
│   │   └── TileMap
│   ├── Player
│   └── UI
│       ├── StaminaBar
│       └── DebugOverlay
├── objects/
│   ├── base_object.tscn (Base interactable object)
│   ├── container.tscn
│   ├── workbench.tscn
│   ├── crate.tscn
│   ├── lamp.tscn
│   └── plant.tscn
└── npc.tscn
```

### Scripts Organization
```
scripts/
├── main.gd (Main game controller)
├── world_system.gd (World generation and management)
├── tilemap_manager.gd (Tile-based operations)
├── player.gd (Player movement and interactions)
├── base_object.gd (Base interactable behavior)
├── container.gd (Container-specific behavior)
├── npc.gd (NPC behavior and dialogue)
├── constants.gd (Global enums and constants)
└── debug_overlay.gd (Debug visualization)
```

### Physics Layers
1. World (Layer 1): Static environment
2. Player (Layer 2): Player character
3. Interactable (Layer 3): Objects that can be interacted with
4. NPC (Layer 4): Non-player characters

## Future Expansions

### LLM Integration
1. Dialogue System
   - Dynamic NPC conversations using LLM
   - Context-aware responses based on game state
   - Memory system for conversation history
   - Emotion and personality modeling

2. Quest Generation
   - Procedural quest generation using LLM
   - Dynamic objectives based on player actions
   - Adaptive difficulty scaling
   - Multi-path quest resolution

3. Environmental Storytelling
   - Dynamic object descriptions
   - Contextual world building
   - Adaptive narrative elements

### Visual Enhancements
1. Lighting System
   - Dynamic shadows
   - Day/night cycle
   - Light sources with unique properties
   - Visual effects for different states

2. Particle Systems
   - Environmental particles
   - Interaction feedback
   - Status effects
   - Weather system

3. UI/UX Improvements
   - Minimalist HUD
   - Context-sensitive prompts
   - Smooth transitions
   - Accessibility options

### Sound Design
1. Ambient System
   - Dynamic background music
   - Environmental sounds
   - Weather effects
   - Time-based variations

2. Interaction Audio
   - Object-specific sounds
   - Footsteps system
   - Impact sounds
   - Voice system for NPCs

3. Spatial Audio
   - 3D sound positioning
   - Sound occlusion
   - Reverb zones
   - Distance-based effects

### Gameplay Extensions
1. Reality Manipulation
   - Time manipulation
   - Space distortion
   - Object state changes
   - Environmental puzzles

2. Advanced AI
   - NPC pathfinding
   - Behavior trees
   - Group dynamics
   - Learning patterns

3. Player Progression
   - Skill system
   - Equipment upgrades
   - Knowledge accumulation
   - Reality manipulation powers

4. World Interaction
   - Complex object interactions
   - Environmental hazards
   - Dynamic weather effects
   - Time-based events

## Implementation Priorities
1. Core Systems Stabilization
   - Fix remaining bugs
   - Optimize performance
   - Complete base interactions
   - Establish testing framework

2. LLM Integration (Phase 1)
   - Basic dialogue system
   - Simple quest generation
   - Object descriptions
   - Context management

3. Visual and Audio Foundation
   - Basic lighting system
   - Essential sound effects
   - UI improvements
   - Particle effects

4. Gameplay Mechanics
   - Basic reality manipulation
   - Simple NPC AI
   - Core progression system
   - Environmental interactions

5. Advanced Features
   - Complex LLM interactions
   - Advanced visual effects
   - Spatial audio
   - Advanced AI behaviors

## Development Guidelines
1. Code Structure
   - Use dependency injection
   - Follow SOLID principles
   - Implement proper error handling
   - Maintain clear documentation

2. Scene Organization
   - Keep scenes modular
   - Use scene inheritance
   - Maintain clear node hierarchy
   - Follow naming conventions

3. Resource Management
   - Optimize asset loading
   - Use resource preloading
   - Implement proper cleanup
   - Monitor memory usage

4. Testing
   - Unit tests for core systems
   - Integration tests for LLM features
   - Performance benchmarks
   - User experience testing

## System Overview

Shadow Worker is built on a modular architecture that emphasizes clean separation of concerns and maintainable code. The core systems are designed to work together while remaining independent and testable.

## Core Systems

### 1. World System
The foundation of the game environment, managing:
- Tile-based world representation
- Collision detection
- Resource management
- Entity management
- Camera control
- Physics interactions

#### Estate Map System
A specialized subsystem for generating and managing the estate layout:
- Procedural generation of estate layouts
- Tile and object placement
- Spawn point management
- Garden and decoration placement
- Path generation
- Water feature placement

### 2. Entity System
Handles all dynamic objects in the game:
- Player character
- NPCs
- Interactive objects
- Particle effects
- Collision detection
- State management

### 3. Resource Management
Centralizes asset handling:
- Texture loading and caching
- Sound management
- Memory optimization
- Asset cleanup

### 4. Rendering System
Manages all visual aspects:
- Tile rendering
- Entity rendering
- Particle effects
- UI elements
- Camera management
- Visual effects

### 5. Input System
Handles all user interaction:
- Keyboard input
- Mouse input
- Controller support
- Input mapping
- Event propagation

### 6. Sound System
Manages audio components:
- Sound effects
- Background music
- Spatial audio
- Volume control
- Audio streaming

## Data Structures

### World
```c
typedef struct World {
    TileType* tiles;
    TileProperties* tileProperties;
    Vector2 dimensions;
    EntityPool* entityPool;
    ResourceManager* resources;
    Camera2D* camera;
    Rectangle bounds;
    float globalResonance;
    float instabilityLevel;
    bool isStable;
    Vector2* spawnPoints;
    int spawnPointCount;
} World;
```

### Estate Map
```c
typedef struct EstateMap {
    World* world;
    Vector2* spawnPoints;
    int spawnPointCount;
} EstateMap;
```

## Key Features

### 1. Procedural Generation
- Estate layout generation
- Dynamic path creation
- Garden placement
- Object distribution
- Spawn point allocation

### 2. Psychological Elements
- Resonance system
- Reality distortion
- Mental state tracking
- Environmental responses
- NPC behavior adaptation

### 3. Environmental Interaction
- Dynamic lighting
- Weather effects
- Time system
- Environmental hazards
- Interactive objects

## File Structure

```
shadow-worker/
├── src/
│   ├── core/
│   │   ├── map.c          # Estate map implementation
│   │   ├── map.h          # Estate map interface
│   │   └── map_test.c     # Estate map unit tests
│   ├── entities/
│   ├── systems/
│   └── utils/
├── include/
│   ├── world.h
│   └── entity.h
├── resources/
│   ├── maps/
│   │   ├── tileset_main.png
│   │   └── tileset_config.json
│   └── objects/
└── docs/
    └── architecture.md
```

## Build System
- CMake-based build system
- Cross-platform compatibility
- Automated testing
- Resource management
- Development tools

## Testing Strategy
- Unit tests for core systems
- Integration tests
- Performance benchmarks
- Memory leak detection
- Automated test suite

## Future Considerations
- Enhanced procedural generation
- Advanced AI behaviors
- Extended environment interaction
- Additional psychological elements
- Performance optimizations