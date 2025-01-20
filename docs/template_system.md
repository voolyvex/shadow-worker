# Template System Documentation

## Overview
The template system provides a flexible and scalable approach to room and environment generation in Shadow Worker. It supports dynamic scaling, seamless transitions, and intelligent feature preservation across different room sizes.

## Template Types

### Room Templates
```json
{
    "type": "combat",
    "difficulty": 1,
    "size": {
        "x": 6,
        "y": 6
    },
    "connections": {
        "positions": [
            {
                "x": 0,
                "y": 3,
                "type": "door",
                "roomSize": {"x": 6, "y": 6}
            }
        ]
    },
    "spawns": [
        {
            "x": 3,
            "y": 3,
            "type": "enemy",
            "enemyType": "basic"
        }
    ]
}
```

### Supported Room Types
- `combat`: Combat encounter rooms
- `treasure`: Reward and item rooms
- `shop`: Merchant and upgrade rooms
- `boss`: Boss battle arenas

### Room Sizes
- `tiny`: 4x4 tiles
- `small`: 6x6 tiles
- `medium`: 8x8 tiles
- `large`: 10x10 tiles
- `huge`: 12x12 tiles
- `boss`: 16x16 tiles

## Scaling System

### Scaling Modes
1. **PixelPerfect**
   - Nearest-neighbor scaling
   - No interpolation
   - Best for maintaining pixel art clarity

2. **Smooth**
   - Bilinear filtering
   - Mipmapping enabled
   - Suitable for larger rooms

3. **Detailed**
   - Trilinear filtering
   - Anisotropic filtering (4x)
   - Best for high-resolution assets

### Feature Preservation
The scaling system preserves:
- Door positions and alignment
- Spawn point validity
- Obstacle placement
- Room proportions
- Gameplay balance

## Integration System

### Door Alignment
- Doors must be placed on room edges
- Center alignment within wall sections
- Minimum spacing between doors
- Valid clearance for player movement

### Transitions
1. **Smooth Transitions**
   - Gradual blending between rooms
   - Height matching for seamless connections
   - Texture blending at boundaries

2. **Sharp Transitions**
   - Immediate changes between rooms
   - Useful for distinct area separation
   - Maintains architectural integrity

### Connection Matching
The system scores connections based on:
- Relative position (40% weight)
- Orientation compatibility (30% weight)
- Size compatibility (30% weight)

## Usage Examples

### Basic Template Validation
```powershell
Test-LevelTemplate -TemplatePath "templates/combat_room.json" -TemplateType "rooms"
```

### Scaling Templates
```powershell
$scaled = Get-ScaledTemplate -TemplatePath "templates/basic_room.json" -TargetSize "large" -ScaleMode "PixelPerfect"
```

### Creating Transitions
```powershell
$transition = New-TemplateTransition -SourceTemplatePath "room1.json" -TargetTemplatePath "room2.json" -TransitionType "smooth"
```

## Performance Optimization

### Memory Management
- Template caching for frequent sizes
- Resource pooling for common elements
- Efficient texture atlas usage
- Dynamic batch processing

### Loading Optimization
- Chunk-based loading system
- Asynchronous template processing
- Background transition generation
- Smart resource unloading

## Validation Rules

### Room Templates
- Must specify type and difficulty
- Valid size dimensions
- At least one connection point
- Proper spawn point placement

### Transitions
- Compatible connection points
- Valid blend distances
- Proper texture coordinates
- Collision boundary integrity

## Testing

### Running Tests
```powershell
.\tests\test_template_scaling.ps1 -Verbose
```

### Test Coverage
- Template validation
- Scaling accuracy
- Feature preservation
- Transition generation
- Performance metrics

## Best Practices

1. **Template Design**
   - Use standard room sizes
   - Place doors at standard positions
   - Maintain clear paths
   - Balance feature distribution

2. **Scaling Considerations**
   - Choose appropriate scaling modes
   - Verify feature preservation
   - Test different size combinations
   - Monitor performance impact

3. **Integration Guidelines**
   - Plan connection points carefully
   - Test transitions thoroughly
   - Validate gameplay flow
   - Consider visual coherence

## Error Handling

### Common Issues
1. Invalid template structure
2. Incompatible scaling requests
3. Connection matching failures
4. Resource loading errors

### Resolution Steps
1. Validate template format
2. Check size compatibility
3. Verify connection points
4. Monitor resource usage

## Future Enhancements

### Planned Features
1. Advanced biome transitions
2. Dynamic difficulty scaling
3. Procedural decoration
4. Enhanced lighting integration

### Performance Improvements
1. Optimized batch processing
2. Enhanced caching system
3. Smarter resource management
4. Parallel processing support 