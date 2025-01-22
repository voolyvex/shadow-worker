extends Node2D

func _ready():
    print("Starting world generation test...")
    
    # Get the WorldSystem node
    var world_system = $WorldSystem
    if not world_system:
        push_error("WorldSystem node not found!")
        return
    
    # Test basic world generation
    var params = {
        "width": 50,
        "height": 50,
        "noise_scale": 0.1,
        "seed": 12345  # Fixed seed for reproducible testing
    }
    
    # Clear any existing world
    world_system.clear_world()
    
    # Generate new world
    world_system.generate_world(params)
    
    # Verify tilemap
    var tilemap = world_system.get_node("TileMap")
    if not tilemap:
        push_error("TileMap node not found!")
        return
    
    # Check if tiles were placed
    var used_cells = tilemap.get_used_cells(0)  # Layer 0 is ground
    if used_cells.size() == 0:
        push_error("No tiles were generated!")
    else:
        print("Generated " + str(used_cells.size()) + " tiles")
    
    # Check object placement
    var objects = world_system.get_placeable_objects()
    print("Placed " + str(objects.size()) + " objects")
    
    print("World generation test completed") 