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

2. **Psychological Systems**
   - Resonance state progression
   - Reality perception layers
   - Inner truth manifestations
   - Consciousness thresholds

3. **Base Game Systems**
   - Player movement and interaction
   - World generation framework
   - Basic object placement

### In Progress
1. **Integration Systems**
   - Collective consciousness mechanics
   - Reality distortion effects
   - Psychological event propagation

2. **Visual Systems**
   - Surreal state visualization
   - Psychological manifestations
   - Reality distortion effects

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

### Data Flow
1. Personality states influence world generation
2. Environmental factors affect psychological states
3. NPC interactions modify resonance levels
4. Collective consciousness shapes reality

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