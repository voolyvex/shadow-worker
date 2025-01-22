# Shadow Worker - Unity Transition Plan

## Project Structure
```
Assets/
├── Scripts/
│   ├── Core/
│   │   ├── PersonalitySystem/
│   │   ├── WorldSystem/
│   │   └── VisualEffects/
│   ├── Player/
│   ├── NPC/
│   └── UI/
├── Scenes/
│   ├── Main.unity
│   └── Test.unity
├── Prefabs/
│   ├── Player/
│   ├── NPC/
│   └── Objects/
├── Resources/
│   ├── Tiles/
│   └── Effects/
└── Tests/
    ├── EditMode/
    └── PlayMode/
```

## Core Systems Migration

### 1. Personality System
```csharp
namespace ShadowWorker.Core
{
    public class PersonalityProfile : MonoBehaviour
    {
        [SerializeField] private EnneagramType primaryType;
        [SerializeField] private DSMDimensions dsmTraits;
        
        public ResonanceState CurrentResonanceState { get; private set; }
        public PerceptionLayer CurrentPerceptionLayer { get; private set; }
        
        private float consciousnessIntegration;
        private float realityIntegration;
        
        // Unity lifecycle methods
        private void Awake()
        {
            InitializePersonality();
        }
        
        // Core functionality
        public void UpdatePsychologicalState()
        {
            // State update logic
        }
    }
}
```

### 2. World System
```csharp
namespace ShadowWorker.Core
{
    public class WorldSystem : MonoBehaviour
    {
        [SerializeField] private GameObject roomPrefab;
        [SerializeField] private int minRooms = 5;
        [SerializeField] private int maxRooms = 10;
        
        private Grid grid;
        private List<Room> rooms;
        
        private void Start()
        {
            InitializeWorld();
        }
        
        private void InitializeWorld()
        {
            // World generation logic
        }
    }
}
```

### 3. Visual Effects System
```csharp
namespace ShadowWorker.Core
{
    public class VisualEffectsManager : MonoBehaviour
    {
        [SerializeField] private Material distortionMaterial;
        
        private Dictionary<string, EffectParameters> activeEffects;
        
        private void Awake()
        {
            activeEffects = new Dictionary<string, EffectParameters>();
        }
        
        public void ApplyEffect(string effectName, float intensity)
        {
            // Effect application logic
        }
    }
}
```

## Key Differences and Adaptations

### 1. Scene Management
- Replace Godot's scene tree with Unity's GameObject hierarchy
- Utilize Unity's Prefab system instead of Godot's scene instances
- Implement ScriptableObjects for data-driven design

### 2. Physics System
- Transition from Godot's physics layers to Unity's layer-based system
- Update collision detection to use Unity's Collider2D components
- Implement Unity's new input system for player controls

### 3. Scripting Differences
- Convert GDScript to C# using Unity's MonoBehaviour system
- Replace Godot signals with Unity Events and C# events
- Utilize Unity's Coroutines for time-based operations

## Testing Strategy

### 1. Edit Mode Tests
```csharp
namespace ShadowWorker.Tests.EditMode
{
    public class PersonalitySystemTests
    {
        [Test]
        public void TestPersonalityStateTransitions()
        {
            // Test implementation
        }
    }
}
```

### 2. Play Mode Tests
```csharp
namespace ShadowWorker.Tests.PlayMode
{
    public class WorldSystemTests : IPrebuildSetup
    {
        [UnityTest]
        public IEnumerator TestRoomGeneration()
        {
            // Test implementation
            yield return null;
        }
    }
}
```

## Migration Phases

### Phase 1: Core Framework
1. Set up Unity project with proper structure
2. Implement basic PersonalitySystem
3. Create fundamental WorldSystem
4. Establish testing framework

### Phase 2: Systems Integration
1. Develop player controller and camera system
2. Implement NPC behavior system
3. Create UI framework
4. Set up visual effects system

### Phase 3: Content Migration
1. Import and adapt existing assets
2. Convert room templates
3. Migrate NPC profiles
4. Transfer psychological content

### Phase 4: Polish & Optimization
1. Implement Unity-specific optimizations
2. Add visual polish using Unity's particle system
3. Enhance lighting and post-processing
4. Performance testing and profiling

## Unity-Specific Enhancements

### 1. Performance Optimizations
- Object pooling for frequently instantiated objects
- Efficient Unity-specific data structures
- Built-in profiler integration
- Addressable Assets system for resource management

### 2. Visual Improvements
- Universal Render Pipeline (URP) for enhanced 2D lighting
- Post-processing effects for psychological states
- Shader Graph for visual effects
- Particle system for environmental feedback

### 3. Development Tools
- Custom Unity Editor tools for content creation
- Debug visualization tools
- Scene validation tools
- Automated build pipeline

## Next Steps
1. Install Unity and required packages
2. Set up version control for Unity project
3. Create initial project structure
4. Begin core systems implementation

## Required Unity Packages
- 2D Feature Set
- Universal RP
- Test Framework
- Input System
- TextMeshPro
- Addressables
- Cinemachine 