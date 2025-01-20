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

3. **Base Game Systems**
   - Player movement and interaction
   - World generation framework
   - Basic object placement
   - Test-driven development workflow
   - Mock system implementations
   - State management and cleanup

### In Progress
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